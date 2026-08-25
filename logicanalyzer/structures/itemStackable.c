#include "../liblogicanalyzer.h"
#include "../utils.h"
#include "model.h"
#include "itemStackable.h"
#include "error.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

const LAItemStackableVTable LAItemStackableVTableBase = {
  .modelVTable = {
    .destroy         = (LAItemModelFnDestroy) LAItemStackableDestroy,
    .increaseCount   = (LAItemModelFnIncreaseCount) LAItemStackableIncreaseCount,
    .decreaseCount   = (LAItemModelFnDecreaseCount) LAItemStackableDecreaseCount,
    .resetCount      = (LAItemModelFnResetCount) LAItemStackableResetCount,
    .setNodeLength   = (LAItemModelFnSetNodeLength) LAItemStackableSetNodeLength,
    .getNodeLength   = (LAItemModelFnGetNodeLength) LAItemStackableGetNodeLength,
    .isInfinite      = (LAItemModelFnIsInfinite) LAItemStackableIsInfinite,
    .isFinite        = (LAItemModelFnIsFinite) LAItemStackableIsFinite,
    .isMutable       = (LAItemModelFnIsMutable) LAItemStackableIsMutable,
    .getNodeCount    = (LAItemModelFnGetNodeCount) LAItemStackableGetNodeCount,
    .setMaxNodeCount = (LAItemModelFnSetMaxNodeCount) LAItemStackableSetMaxNodeCount,
    .getMaxNodeCount = (LAItemModelFnGetMaxNodeCount) LAItemStackableGetMaxNodeCount,
    .iter            = (LAItemModelFnIter) LAItemStackableIter,
    .insert          = (LAItemModelFnInsert) LAItemStackablePush,
    .remove          = (LAItemModelFnRemove) LAItemStackablePop,
    .get             = (LAItemModelFnGet) LAItemStackablePeek,
    .isEmpty         = (LAItemModelFnIsEmpty) LAItemStackableIsEmpty
  },
  .push = (LAItemStackableFnPush) LAItemStackablePush,
  .pop  = (LAItemStackableFnPop)  LAItemStackablePop,
  .peek = (LAItemStackableFnPeek) LAItemStackablePeek
};

LAErrorCode LAItemStackableInit(LAItemStackable *stackable, size_t maxNodeCount, bool isInfinite, size_t defaultNodeSize, bool useDefaultSize){
  LA_HANDLE_NULLPTR(stackable, LA_PROPAGATE_ERROR);
  
  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelInit(LA_ITEM_MODEL(stackable), maxNodeCount, isInfinite, defaultNodeSize, useDefaultSize);

  stackable->model.vtable = (LAItemModelVTable *) &(LAItemStackableVTableBase);
  
  return code;  
}

LAErrorCode LAItemStackableDestroy(LAItemStackable **stackable){
  LA_HANDLE_NULLPTR(stackable, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  LAItemModel *model = LA_ITEM_MODEL(*stackable);
  code = LAItemModelDestroy(&model);
  
  return code;  
}

LAErrorCode LAItemStackableIncreaseCount(LAItemStackable *stackable, const size_t amount){
  LA_HANDLE_NULLPTR(stackable, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelIncreaseCount(LA_ITEM_MODEL(stackable), amount);
  
  return code;  
}

LAErrorCode LAItemStackableDecreaseCount(LAItemStackable *stackable, const size_t amount){
  LA_HANDLE_NULLPTR(stackable, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelDecreaseCount(LA_ITEM_MODEL(stackable), amount);
  
  return code;  
}

LAErrorCode LAItemStackableResetCount(LAItemStackable *stackable){
  LA_HANDLE_NULLPTR(stackable, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelResetCount(LA_ITEM_MODEL(stackable));
  
  return code;  
}

LAErrorCode LAItemStackableSetNodeLength(LAItemStackable *stackable, const size_t length){
  LA_HANDLE_NULLPTR(stackable, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelSetNodeLength(LA_ITEM_MODEL(stackable), length);
  
  return code;  
}

LAErrorCode LAItemStackableGetNodeLength(const LAItemStackable *stackable, size_t *length){
  LA_HANDLE_NULLPTR(stackable, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelGetNodeLength(LA_ITEM_MODEL(stackable), length);
  
  return code;  
}

LAErrorCode LAItemStackableIsInfinite(const LAItemStackable *stackable, bool *isInfinite){
  LA_HANDLE_NULLPTR(stackable,      LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(isInfinite, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelIsInfinite(LA_ITEM_MODEL(stackable), isInfinite);
  
  return code;  
}

LAErrorCode LAItemStackableIsFinite(const LAItemStackable *stackable, bool *isFinite){
  LA_HANDLE_NULLPTR(stackable, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelIsFinite(LA_ITEM_MODEL(stackable), isFinite);
  
  return code;  
}

LAErrorCode LAItemStackableIsMutable(const LAItemStackable *stackable, bool *isMutable){
  LA_HANDLE_NULLPTR(stackable, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelIsMutable(LA_ITEM_MODEL(stackable), isMutable);
  
  return code;  
}

LAErrorCode LAItemStackableGetNodeCount(const LAItemStackable *stackable, size_t *count){
  LA_HANDLE_NULLPTR(stackable, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(count, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelGetNodeCount(LA_ITEM_MODEL(stackable), count);
  
  return code;  
}

LAErrorCode LAItemStackableSetMaxNodeCount(LAItemStackable *stackable, size_t count){
  LA_HANDLE_NULLPTR(stackable, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelSetMaxNodeCount(LA_ITEM_MODEL(stackable), count);
  
  return code;  
}

LAErrorCode LAItemStackableGetMaxNodeCount(const LAItemStackable *stackable, size_t *count){
  LA_HANDLE_NULLPTR(stackable, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(count, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelGetMaxNodeCount(LA_ITEM_MODEL(stackable), count);
  
  return code;  
}

LAErrorCode LAItemStackableIter(LAItemStackable *stackable, LAItemStackableCallback callback, void *data){
  LA_HANDLE_NULLPTR(stackable, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(callback, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelIter(LA_ITEM_MODEL(stackable), LA_ITEM_MODEL_CALLBACK(callback), data);
  
  return code;  
}

LAErrorCode LAItemStackablePush(LAItemStackable *stackable, void *data, size_t length){
  LA_HANDLE_NULLPTR(stackable, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data,  LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelInsert(LA_ITEM_MODEL(stackable), data, length);
  
  return code;  
}

LAErrorCode LAItemStackablePop(LAItemStackable *stackable, void **data, size_t *length){
  LA_CHECK_NULLPTR(stackable);
  LA_CHECK_NULLPTR(data);
  LA_CHECK_NULLPTR(length);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelRemove(LA_ITEM_MODEL(stackable), data, length);

  return code;  
}

LAErrorCode LAItemStackablePeek(const LAItemStackable *stackable, void **data, size_t *length){
  LA_CHECK_NULLPTR(stackable);
  LA_CHECK_NULLPTR(data);
  LA_CHECK_NULLPTR(length);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelGet(LA_ITEM_MODEL(stackable), data, length);
  
  return code;  
}

LAErrorCode LAItemStackableIsEmpty(const LAItemStackable *stackable, bool *isEmpty){
  LA_CHECK_NULLPTR(stackable);
  LA_CHECK_NULLPTR(isEmpty);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelIsEmpty(LA_ITEM_MODEL(stackable), isEmpty);
  
  return code;  
}
