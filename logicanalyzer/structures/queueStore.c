#include "../liblogicanalyzer.h"
#include "../utils.h"
#include "model.h"
#include "itemStackable.h"
#include "listStore.h"
#include "queueStore.h"
#include "error.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

const LAQueueStoreVTable LAQueueStoreVTableBase = {
  .stackableVTable = {
    {
      .destroy         = (LAItemModelFnDestroy) LAQueueStoreDestroy,
      .increaseCount   = (LAItemModelFnIncreaseCount) LAQueueStoreIncreaseCount,
      .decreaseCount   = (LAItemModelFnDecreaseCount) LAQueueStoreDecreaseCount,
      .resetCount      = (LAItemModelFnResetCount) LAQueueStoreResetCount,
      .setNodeLength   = (LAItemModelFnSetNodeLength) LAQueueStoreSetNodeLength,
      .getNodeLength   = (LAItemModelFnGetNodeLength) LAQueueStoreGetNodeLength,
      .isInfinite      = (LAItemModelFnIsInfinite) LAQueueStoreIsInfinite,
      .isFinite        = (LAItemModelFnIsFinite) LAQueueStoreIsFinite,
      .isMutable       = (LAItemModelFnIsMutable) LAQueueStoreIsMutable,
      .getNodeCount    = (LAItemModelFnGetNodeCount) LAQueueStoreGetNodeCount,
      .setMaxNodeCount = (LAItemModelFnSetMaxNodeCount) LAQueueStoreSetMaxNodeCount,
      .getMaxNodeCount = (LAItemModelFnGetMaxNodeCount) LAQueueStoreGetMaxNodeCount,
      .iter            = (LAItemModelFnIter) LAQueueStoreIter,
      .insert          = (LAItemModelFnInsert) LAQueueStorePush,
      .remove          = (LAItemModelFnRemove) LAQueueStorePop,
      .get             = (LAItemModelFnGet) LAQueueStorePeek
    },
    .push = (LAItemStackableFnPush) LAQueueStorePush,
    .pop  = (LAItemStackableFnPop)  LAQueueStorePop,
    .peek = (LAItemStackableFnPeek) LAQueueStorePeek
  } 
};

LAErrorCode LAQueueStoreInit(LAQueueStore *queue, size_t maxNodeCount, bool isInfinite, size_t defaultNodeSize, bool useDefaultSize){
  LA_CHECK_NULLPTR(queue);
  
  LAErrorCode code = LA_NO_ERROR;
  queue->stackable.model.vtable = (LAItemModelVTable *) &(LAQueueStoreVTableBase);

  // Init list
  code = LAListStoreInit(&(queue->list), maxNodeCount, isInfinite, defaultNodeSize, useDefaultSize);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(queue->list)), LA_ITEM_MODEL(queue));
  if(code) return code;
  
  return code;  
}

LAErrorCode LAQueueStoreDestroy(LAQueueStore **queue){
  LA_CHECK_NULLPTR(queue);

  LAErrorCode code = LA_NO_ERROR;
  LAItemStackable *stackable = LA_ITEM_STACKABLE(*queue);
  code = LAItemStackableDestroy(&stackable);
  if(code) return code;

  LAListStore *list = &((*queue)->list);
  code = LAListStoreDestroy(&list);
  if(code) return code;
  
  return code;  
}

LAErrorCode LAQueueStoreIncreaseCount(LAQueueStore *queue, const size_t amount){
  LA_CHECK_NULLPTR(queue);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreIncreaseCount(&(queue->list), amount);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(queue->list)), LA_ITEM_MODEL(queue));
  if(code) return code;
  
  return code;  
}

LAErrorCode LAQueueStoreDecreaseCount(LAQueueStore *queue, const size_t amount){
  LA_CHECK_NULLPTR(queue);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreDecreaseCount(&(queue->list), amount);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(queue->list)), LA_ITEM_MODEL(queue));
  if(code) return code;
  
  return code;  
}

LAErrorCode LAQueueStoreResetCount(LAQueueStore *queue){
  LA_CHECK_NULLPTR(queue);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreResetCount(&(queue->list));
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(queue->list)), LA_ITEM_MODEL(queue));
  if(code) return code;
  
  return code;  
}

LAErrorCode LAQueueStoreSetNodeLength(LAQueueStore *queue, const size_t length){
  LA_CHECK_NULLPTR(queue);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreSetNodeLength(&(queue->list), length);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(queue->list)), LA_ITEM_MODEL(queue));
  if(code) return code;
  
  return code;  
}

LAErrorCode LAQueueStoreGetNodeLength(const LAQueueStore *queue, size_t *length){
  LA_CHECK_NULLPTR(queue);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreGetNodeLength(&(queue->list), length);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(queue->list)), LA_ITEM_MODEL(queue));
  if(code) return code;
  
  return code;  
}

LAErrorCode LAQueueStoreIsInfinite(const LAQueueStore *queue, bool *isInfinite){
  LA_CHECK_NULLPTR(queue);
  LA_HANDLE_NULLPTR(isInfinite, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreIsInfinite(&(queue->list), isInfinite);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(queue->list)), LA_ITEM_MODEL(queue));
  if(code) return code;
  
  return code;  
}

LAErrorCode LAQueueStoreIsFinite(const LAQueueStore *queue, bool *isFinite){
  LA_CHECK_NULLPTR(queue);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreIsFinite(&(queue->list), isFinite);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(queue->list)), LA_ITEM_MODEL(queue));
  if(code) return code;
  
  return code;  
}

LAErrorCode LAQueueStoreIsMutable(const LAQueueStore *queue, bool *isMutable){
  LA_CHECK_NULLPTR(queue);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreIsMutable(&(queue->list), isMutable);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(queue->list)), LA_ITEM_MODEL(queue));
  if(code) return code;
  
  return code;  
}

LAErrorCode LAQueueStoreGetNodeCount(const LAQueueStore *queue, size_t *count){
  LA_CHECK_NULLPTR(queue);
  LA_HANDLE_NULLPTR(count, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreGetNodeCount(&(queue->list), count);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(queue->list)), LA_ITEM_MODEL(queue));
  if(code) return code;
  
  return code;  
}

LAErrorCode LAQueueStoreSetMaxNodeCount(LAQueueStore *queue, size_t count){
  LA_CHECK_NULLPTR(queue);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreSetMaxNodeCount(&(queue->list), count);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(queue->list)), LA_ITEM_MODEL(queue));
  if(code) return code;
  
  return code;  
}

LAErrorCode LAQueueStoreGetMaxNodeCount(const LAQueueStore *queue, size_t *count){
  LA_CHECK_NULLPTR(queue);
  LA_HANDLE_NULLPTR(count, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreGetMaxNodeCount(&(queue->list), count);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(queue->list)), LA_ITEM_MODEL(queue));
  if(code) return code;
  
  return code;  
}

LAErrorCode LAQueueStoreIter(LAQueueStore *queue, LAQueueStoreCallback callback, void *data){
  LA_CHECK_NULLPTR(queue);
  LA_HANDLE_NULLPTR(callback, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreIter(&(queue->list), LA_LIST_STORE_CALLBACK(callback), data);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(queue->list)), LA_ITEM_MODEL(queue));
  if(code) return code;
  
  return code;  
}

LAErrorCode LAQueueStorePush(LAQueueStore *queue, void *data, size_t length){
  LA_CHECK_NULLPTR(queue);
  LA_HANDLE_NULLPTR(data,  LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreInsert(&(queue->list), data, length);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(queue->list)), LA_ITEM_MODEL(queue));
  if(code) return code;
  
  return code;  
}

LAErrorCode LAQueueStorePop(LAQueueStore *queue, void **data, size_t *length){
  LA_CHECK_NULLPTR(queue);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreRemoveAt(&(queue->list), 0l, data, length);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(queue->list)), LA_ITEM_MODEL(queue));
  if(code) return code;
  
  return code;  
}

LAErrorCode LAQueueStorePeek(const LAQueueStore *queue, void **data, size_t *length){
  LA_CHECK_NULLPTR(queue);
  LA_HANDLE_NULLPTR(data,   LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreGet(&(queue->list), data, length);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(queue->list)), LA_ITEM_MODEL(queue));
  if(code) return code;
  
  return code;  
}

