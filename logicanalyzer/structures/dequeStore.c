#include "../liblogicanalyzer.h"
#include "../utils.h"
#include "model.h"
#include "itemStackable.h"
#include "listStore.h"
#include "dequeStore.h"
#include "error.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

const LADequeStoreVTable LADequeStoreVTableBase = {
  .stackableVTable = {
    {
      .destroy         = (LAItemModelFnDestroy) LADequeStoreDestroy,
      .increaseCount   = (LAItemModelFnIncreaseCount) LADequeStoreIncreaseCount,
      .decreaseCount   = (LAItemModelFnDecreaseCount) LADequeStoreDecreaseCount,
      .resetCount      = (LAItemModelFnResetCount) LADequeStoreResetCount,
      .setNodeLength   = (LAItemModelFnSetNodeLength) LADequeStoreSetNodeLength,
      .getNodeLength   = (LAItemModelFnGetNodeLength) LADequeStoreGetNodeLength,
      .isInfinite      = (LAItemModelFnIsInfinite) LADequeStoreIsInfinite,
      .isFinite        = (LAItemModelFnIsFinite) LADequeStoreIsFinite,
      .isMutable       = (LAItemModelFnIsMutable) LADequeStoreIsMutable,
      .getNodeCount    = (LAItemModelFnGetNodeCount) LADequeStoreGetNodeCount,
      .setMaxNodeCount = (LAItemModelFnSetMaxNodeCount) LADequeStoreSetMaxNodeCount,
      .getMaxNodeCount = (LAItemModelFnGetMaxNodeCount) LADequeStoreGetMaxNodeCount,
      .iter            = (LAItemModelFnIter) LADequeStoreIter,
      .insert          = (LAItemModelFnInsert) LADequeStorePush,
      .remove          = (LAItemModelFnRemove) LADequeStorePop,
      .get             = (LAItemModelFnGet) LADequeStorePeek,
      .isEmpty         = (LAItemModelFnIsEmpty) LADequeStoreIsEmpty,
    },
    .push = (LAItemStackableFnPush) LADequeStorePush,
    .pop  = (LAItemStackableFnPop)  LADequeStorePop,
    .peek = (LAItemStackableFnPeek) LADequeStorePeek
  }, 
  .pushLeft = (LADequeStoreFnPushLeft) LADequeStorePushLeft,
  .popLeft  = (LADequeStoreFnPopLeft)  LADequeStorePopLeft,
  .peekLeft = (LADequeStoreFnPeekLeft) LADequeStorePeekLeft
};

LAErrorCode LADequeStoreInit(LADequeStore *deque, size_t maxNodeCount, bool isInfinite, size_t defaultNodeSize, bool useDefaultSize){
  LA_CHECK_NULLPTR(deque);
  
  LAErrorCode code = LA_NO_ERROR;
  deque->stackable.model.vtable = (LAItemModelVTable *) &(LADequeStoreVTableBase);

  // Init list
  code = LAListStoreInit(&(deque->list), maxNodeCount, isInfinite, defaultNodeSize, useDefaultSize);
  if(code) return code;

  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStoreDestroy(LADequeStore **deque){
  LA_HANDLE_NULLPTR(deque, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  LAItemStackable *stackable = LA_ITEM_STACKABLE(*deque);
  code = LAItemStackableDestroy(&stackable);
  if(code) return code;

  LAListStore *list = &((*deque)->list);
  code = LAListStoreDestroy(&list);
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStoreIncreaseCount(LADequeStore *deque, const size_t amount){
  LA_HANDLE_NULLPTR(deque, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreIncreaseCount(&(deque->list), amount);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStoreDecreaseCount(LADequeStore *deque, const size_t amount){
  LA_HANDLE_NULLPTR(deque, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreDecreaseCount(&(deque->list), amount);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStoreResetCount(LADequeStore *deque){
  LA_HANDLE_NULLPTR(deque, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreResetCount(&(deque->list));
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStoreSetNodeLength(LADequeStore *deque, const size_t length){
  LA_HANDLE_NULLPTR(deque, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreSetNodeLength(&(deque->list), length);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStoreGetNodeLength(const LADequeStore *deque, size_t *length){
  LA_HANDLE_NULLPTR(deque, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreGetNodeLength(&(deque->list), length);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStoreIsInfinite(const LADequeStore *deque, bool *isInfinite){
  LA_HANDLE_NULLPTR(deque,      LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(isInfinite, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreIsInfinite(&(deque->list), isInfinite);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStoreIsFinite(const LADequeStore *deque, bool *isFinite){
  LA_HANDLE_NULLPTR(deque, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreIsFinite(&(deque->list), isFinite);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStoreIsMutable(const LADequeStore *deque, bool *isMutable){
  LA_HANDLE_NULLPTR(deque, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreIsMutable(&(deque->list), isMutable);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStoreGetNodeCount(const LADequeStore *deque, size_t *count){
  LA_HANDLE_NULLPTR(deque, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(count, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreGetNodeCount(&(deque->list), count);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStoreSetMaxNodeCount(LADequeStore *deque, size_t count){
  LA_HANDLE_NULLPTR(deque, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreSetMaxNodeCount(&(deque->list), count);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStoreGetMaxNodeCount(const LADequeStore *deque, size_t *count){
  LA_HANDLE_NULLPTR(deque, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(count, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreGetMaxNodeCount(&(deque->list), count);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStoreIter(LADequeStore *deque, LADequeStoreCallback callback, void *data){
  LA_HANDLE_NULLPTR(deque, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(callback, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreIter(&(deque->list), LA_LIST_STORE_CALLBACK(callback), data);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStorePush(LADequeStore *deque, void *data, size_t length){
  LA_HANDLE_NULLPTR(deque, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data,  LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreInsert(&(deque->list), data, length);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStorePop(LADequeStore *deque, void **data, size_t *length){
  LA_HANDLE_NULLPTR(deque, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreRemove(&(deque->list), data, length);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStorePeek(const LADequeStore *deque, void **data, size_t *length){
  LA_HANDLE_NULLPTR(deque,  LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data,   LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreGet(&(deque->list), data, length);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStorePushLeft(LADequeStore *deque, void *data, size_t length){
  LA_HANDLE_NULLPTR(deque, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data,  LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreInsertAt(&(deque->list), 0l, data, length);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStorePopLeft(LADequeStore *deque, void **data, size_t *length){
  LA_HANDLE_NULLPTR(deque, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreRemoveAt(&(deque->list), 0l, data, length);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStorePeekLeft(const LADequeStore *deque, void **data, size_t *length){
  LA_HANDLE_NULLPTR(deque,  LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data,   LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreGetAt(&(deque->list), 0l, data, length);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}

LAErrorCode LADequeStoreIsEmpty(const LADequeStore *deque, bool *isEmpty){
  LA_CHECK_NULLPTR(deque);
  LA_CHECK_NULLPTR(isEmpty);

  LAErrorCode code = LA_NO_ERROR;
  code = LAListStoreIsEmpty(&(deque->list), isEmpty);
  if(code) return code;
  
  code = LAItemModelCopyMetadata(LA_ITEM_MODEL(&(deque->list)), LA_ITEM_MODEL(deque));
  if(code) return code;
  
  return code;  
}
