#ifndef LA_THREADS_ACK_H
#define LA_THREADS_ACK_H

#include <pthread.h>
#ifndef _POSIX_C_SOURCE
  #define _POSIX_C_SOURCE 199309L
#endif
#include <time.h>
#include "../error.h"
#include "../structures/listStore.h"

#ifndef LAACK
  typedef struct _la_ack LAACK;
#endif

struct _la_ack {
  LAListStore ackList;
  pthread_mutex_t ackMutex;
  uint16_t transactionId;
  pthread_mutex_t transactionIdMutex;
};

typedef struct {
  uint16_t transactionId;
  struct timespec ts;
} LAACKData;

LAErrorCode LAGetTimeDelta(struct timespec *start, struct timespec *end, double *timeDelta);
LAErrorCode LAPrintACKList(LAACK *ack);
LAErrorCode LARegisterUsingIdACK(LAACK *ack, uint16_t transactionId);
LAErrorCode LARegisterACK(LAACK *ack, uint16_t *transactionId);
LAErrorCode LAFindACK(LAACK *ack, uint16_t transactionId, bool *found, size_t *foundIndex);
LAErrorCode LAResolveACK(LAACK *ack, uint16_t transactionId, bool *found, double *rtt);
LAErrorCode LAResolveTimeoutACK(LAACK *ack, size_t *matches);

#endif //LA_THREADS_ACK_H
