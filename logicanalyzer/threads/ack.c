#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <pthread.h>
#include <time.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "../structures/listStore.h"
#include "ack.h"

LAErrorCode LAGetTimeDelta(struct timespec *start, struct timespec *end, double *timeDelta){
  LA_CHECK_NULLPTR(start);
  LA_CHECK_NULLPTR(end);
  LA_CHECK_NULLPTR(timeDelta);

  int64_t deltaSeconds     = end->tv_sec  - start->tv_sec;
  int64_t deltaNanoseconds = end->tv_nsec - start->tv_nsec;

  (*timeDelta) = deltaSeconds + (deltaNanoseconds / (double) 1000000000);

  return LA_NO_ERROR;
}

LAErrorCode LAPrintACKCallback(LAListStore *list, LAListStoreNode *node, size_t index, void *data, bool *stopIter){
  struct timespec *ts = (struct timespec *)data;
  LAACKData *ackData = (LAACKData *)node->data;
  LAErrorCode code = LA_NO_ERROR;

  double timeDelta = 0.0;
  code = LAGetTimeDelta(&(ackData->ts), ts, &timeDelta);

  printf("Node: %li\tTransactionId: 0x%04X\tTime since stored: %lf\n", index, ackData->transactionId, timeDelta);

  (void) list;
  (void) stopIter;
  return code;
}

LAErrorCode LAPrintACKList(LAACK *ack){
  LA_CHECK_NULLPTR(ack);
  
  LAErrorCode code = LA_NO_ERROR;

  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  
  code = LAListStoreIter(
            &(ack->ackList),
            LAPrintACKCallback,
            (void *)(&ts)
          );
  
  return code;
}


LAErrorCode LARegisterUsingIdACK(LAACK *ack, uint16_t transactionId){
  LA_CHECK_NULLPTR(ack);
  
  LAErrorCode code = LA_NO_ERROR;

  LAACKData data;
  data.transactionId = transactionId;
  clock_gettime(CLOCK_MONOTONIC, &(data.ts));
  
  // Data is copied inside the ListStore/Doubly linked list
  LA_MUTEX(&(ack->ackMutex), {
    code = LAListStoreInsert(
                &(ack->ackList),
                (void *)(&data),
                sizeof(LAACKData)
          );         
  });

  return code;
}


LAErrorCode LARegisterACK(LAACK *ack, uint16_t *transactionId){
  LA_CHECK_NULLPTR(ack);

  LAErrorCode code = LA_NO_ERROR;

  LA_MUTEX(&(ack->transactionIdMutex), {
    size_t oldId_large = (size_t) ack->transactionId;
    size_t newId_large = oldId_large + 1;
    uint16_t newId     = (uint16_t) (newId_large & 0xFFFF);

    code = LARegisterUsingIdACK(ack, newId);
    if(code == LA_NO_ERROR){
      // Everything OK
      ack->transactionId = newId;
      if(transactionId != NULL) (*transactionId) = newId;
    }
  });
  
  return code;
}

LAErrorCode LAFindACKCallback(LAListStore *list, LAListStoreNode *node, size_t index, void *data, bool *stopIter){
  LAACKData *ackData = (LAACKData *)node->data;
  uint16_t transactionId = *(uint16_t *)data;

  if(transactionId == ackData->transactionId){
    (*stopIter) = true;
  }

  (void) index;
  (void) list;
  return LA_NO_ERROR;
}

LAErrorCode LAFindACK(LAACK *ack, uint16_t transactionId, bool *found, size_t *foundIndex){
  LA_CHECK_NULLPTR(ack);
  LA_CHECK_NULLPTR(found);
  LA_CHECK_NULLPTR(foundIndex);

  LAErrorCode code = LA_NO_ERROR;
  void *data = (void *)(&transactionId);

  LA_MUTEX(&(ack->ackMutex), {
    code = LAListStoreFind(&(ack->ackList), LAFindACKCallback, data, found, foundIndex, NULL);
  });
  return code;
}

typedef struct {
  uint16_t transactionId;
  struct timespec nodeTs;
} LAResolveACKData;

LAErrorCode LAResolveACKCallback(LAListStore *list, LAListStoreNode *node, size_t index, void *data, bool *stopIter){
  LAACKData        *ackData     = (LAACKData *)node->data;
  LAResolveACKData *resolveData = (LAResolveACKData *)data;

  if(resolveData->transactionId == ackData->transactionId){
    (*stopIter) = true;
    resolveData->nodeTs.tv_sec = ackData->ts.tv_sec;
    resolveData->nodeTs.tv_nsec = ackData->ts.tv_nsec;
  }

  (void) index;
  (void) list;
  return LA_NO_ERROR;
}

LAErrorCode LAResolveACK(LAACK *ack, uint16_t transactionId, bool *found, double *rtt){
  LA_CHECK_NULLPTR(ack);
  LA_CHECK_NULLPTR(found);

  LAErrorCode code = LA_NO_ERROR;
  LAResolveACKData data;
  data.transactionId = transactionId;

  LA_MUTEX(&(ack->ackMutex), {
    code = LAListStoreFindRemove(&(ack->ackList), LAResolveACKCallback, &data, found);
  });
  if(code) return code;

  // Return if not found
  if(!(*found)) return LA_NO_ERROR;

  // Node with transId was found
  // Return if node was found but rtt is NULL
  if(rtt == NULL) return LA_NO_ERROR;

  // RTT calculation
  struct timespec currentTs;
  clock_gettime(CLOCK_MONOTONIC, &currentTs);

  double timeDelta = 0;
  code = LAGetTimeDelta(&(data.nodeTs), &currentTs, &timeDelta);
  if(code) return code;
  
  if(rtt != NULL) (*rtt) = timeDelta * 1000;
  return LA_NO_ERROR;
}

#define LA_ACK_TIMEOUT 5.0

LAErrorCode LAResolveTimeoutACKCallback(LAListStore *list, LAListStoreNode *node, size_t index, void *data, bool *stopIter){
  LAACKData        *ackData     = (LAACKData *)node->data;
  struct timespec  *ts          = (struct timespec *)data;

  double timeDelta = 0.0;
  LAErrorCode code = LA_NO_ERROR;

  code = LAGetTimeDelta(&(ackData->ts), ts, &timeDelta);

  if(timeDelta >= LA_ACK_TIMEOUT){
    (*stopIter) = true;
  }

  (void) index;
  (void) list;
  return code;
}

LAErrorCode LAResolveTimeoutACK(LAACK *ack, size_t *matches){
  LA_CHECK_NULLPTR(ack);
  
  LAErrorCode code = LA_NO_ERROR;

  struct timespec ts = {0};
  clock_gettime(CLOCK_MONOTONIC, &ts);

  size_t matches_internal = 0;
  LA_MUTEX(&(ack->ackMutex), {
    code = LAListStoreFindRemoveAll(&(ack->ackList), LAResolveTimeoutACKCallback, &ts, &matches_internal);
  });
  if(matches != NULL) (*matches) = matches_internal;
  return code;
}
