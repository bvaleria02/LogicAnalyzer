#include "../error.h"
#include "../utils.h"
#include "../liblogicanalyzer.h"
#include "model.h"
#include "error.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const LAItemModelVTable LAItemModelVTableBase = {
  .destroy         = (LAItemModelFnDestroy) LAItemModelDestroy,
  .increaseCount   = (LAItemModelFnIncreaseCount) LAItemModelIncreaseCount,
  .decreaseCount   = (LAItemModelFnDecreaseCount) LAItemModelDecreaseCount,
  .resetCount      = (LAItemModelFnResetCount) LAItemModelResetCount,
  .setNodeLength   = (LAItemModelFnSetNodeLength) LAItemModelSetNodeLength,
  .getNodeLength   = (LAItemModelFnGetNodeLength) LAItemModelGetNodeLength,
  .isInfinite      = (LAItemModelFnIsInfinite) LAItemModelIsInfinite,
  .isFinite        = (LAItemModelFnIsFinite) LAItemModelIsFinite,
  .isMutable       = (LAItemModelFnIsMutable) LAItemModelIsMutable,
  .getNodeCount    = (LAItemModelFnGetNodeCount) LAItemModelGetNodeCount,
  .setMaxNodeCount = (LAItemModelFnSetMaxNodeCount) LAItemModelSetMaxNodeCount,
  .getMaxNodeCount = (LAItemModelFnGetMaxNodeCount) LAItemModelGetMaxNodeCount,
  .iter            = (LAItemModelFnIter) LAItemModelIter,
  .insert          = (LAItemModelFnInsert) LAItemModelInsert,
  .remove          = (LAItemModelFnRemove) LAItemModelRemove,
  .get             = (LAItemModelFnGet) LAItemModelGet,
  .isEmpty         = (LAItemModelFnIsEmpty) LAItemModelIsEmpty,
};

LAErrorCode LAItemModelCopyMetadata(LAItemModel *modelSrc, LAItemModel *modelDest){
  LA_CHECK_NULLPTR(modelSrc);
  LA_CHECK_NULLPTR(modelDest);

  size_t vtableOffset = sizeof(LAItemModelVTable *);
  
  memcpy(
         ((uint8_t *)modelDest) + vtableOffset,
         ((uint8_t *)modelSrc)  + vtableOffset,
         sizeof(LAItemModel) - vtableOffset
        );

  return LA_NO_ERROR;
}

LAErrorCode LAItemModelInit(LAItemModel *model, size_t maxNodeCount, bool isInfinite, size_t defaultNodeSize, bool useDefaultSize){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);

  model->vtable            = (LAItemModelVTable *) &LAItemModelVTableBase;
  model->nodeCount         = 0;
  model->defaultNodeLength = defaultNodeSize;
  model->maxNodeCount      = maxNodeCount;
  model->isInfinite        = isInfinite;
  model->isMutable         = true;
  model->useDefaultSize    = useDefaultSize;
  
  return LA_NO_ERROR;  
}


LAErrorCode LAItemModelDestroy(LAItemModel **model){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);

  if((*model) == NULL){
    return LA_ERROR_NULLPTR;
  }

  //free((*model));
  (*model) = NULL;
  
  return LA_NO_ERROR;  
}

LAErrorCode LAItemModelIncreaseCount(LAItemModel *model, const size_t amount){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);

  size_t newCount = model->nodeCount + amount;
  if(newCount < model->nodeCount) return LA_ERROR_INT_OVERFLOW;

  model->nodeCount = newCount;
  return LA_NO_ERROR;  
}

LAErrorCode LAItemModelDecreaseCount(LAItemModel *model, const size_t amount){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);
  
  size_t newCount = model->nodeCount - amount;
  if(newCount > model->nodeCount) return LA_ERROR_INT_UNDERFLOW;

  model->nodeCount = newCount;
  return LA_NO_ERROR;  
}

LAErrorCode LAItemModelResetCount(LAItemModel *model){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);

  model->nodeCount = 0;

  return LA_NO_ERROR;  
}

LAErrorCode LAItemModelSetNodeLength(LAItemModel *model, const size_t length){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);

  model->defaultNodeLength = length;

  return LA_NO_ERROR;  
}

LAErrorCode LAItemModelGetNodeLength(const LAItemModel *model, size_t *length){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);

  (*length) = model->defaultNodeLength;

  return LA_NO_ERROR;  
}

LAErrorCode LAItemModelIsInfinite(const LAItemModel *model, bool *isInfinite){
  LA_HANDLE_NULLPTR(model,      LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(isInfinite, LA_PROPAGATE_ERROR);

  (*isInfinite) = model->isInfinite;

  return LA_NO_ERROR;  
}

LAErrorCode LAItemModelIsFinite(const LAItemModel *model, bool *isFinite){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(isFinite, LA_PROPAGATE_ERROR);

  (*isFinite) = !(model->isInfinite);

  return LA_NO_ERROR;  
}

LAErrorCode LAItemModelIsMutable(const LAItemModel *model, bool *isMutable){
  LA_HANDLE_NULLPTR(model,     LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(isMutable, LA_PROPAGATE_ERROR);

  (*isMutable) = model->isMutable;

  return LA_NO_ERROR;  
}

LAErrorCode LAItemModelGetNodeCount(const LAItemModel *model, size_t *count){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(count, LA_PROPAGATE_ERROR);

  (*count) = model->nodeCount;

  return LA_NO_ERROR;  
}

LAErrorCode LAItemModelSetMaxNodeCount(LAItemModel *model, size_t count){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);

  model->maxNodeCount = count;

  return LA_NO_ERROR;  
}

LAErrorCode LAItemModelGetMaxNodeCount(const LAItemModel *model, size_t *count){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(count, LA_PROPAGATE_ERROR);

  bool isInfinite = false;
  LAErrorCode code = LAItemModelIsInfinite(model, &isInfinite);
  if(code) return code;
  
  (*count) = (isInfinite) ? SIZE_MAX : model->maxNodeCount;

  return LA_NO_ERROR;  
}

LAErrorCode LAItemModelIter(LAItemModel *model, LAItemModelCallback callback, void *data){
  LA_HANDLE_NULLPTR(model,    LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(callback, LA_PROPAGATE_ERROR);

  (void) model;
  (void) callback;
  (void) data;
  return LALogStructureErrorBase(model, "itemModel", "iter");
}

LAErrorCode LAItemModelInsert(LAItemModel *model, void *data, size_t length){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data, LA_PROPAGATE_ERROR);

  (void) model;
  (void) data;
  (void) length;
  return LALogStructureErrorBase((void *)model, "itemModel", "insert");
}

LAErrorCode LAItemModelRemove(LAItemModel *model, void **data, size_t *length){
  LA_CHECK_NULLPTR(model);
  LA_CHECK_NULLPTR(data);
  LA_CHECK_NULLPTR(length);

  (void) model;
  (void) data;
  (void) length;
  return LALogStructureErrorBase((void *)model, "itemModel", "remove");
}

LAErrorCode LAItemModelGet(const LAItemModel *model, void **data, size_t *length){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);

  (void) model;
  (void) data;
  (void) length;
  return LALogStructureErrorBase((void *)model, "itemModel", "get");
}

LAErrorCode LAItemModelIsEmpty(const LAItemModel *model, bool *isEmpty){
  LA_CHECK_NULLPTR(model);
  LA_CHECK_NULLPTR(isEmpty);

  (void) model;
  (void) isEmpty;
  return LALogStructureErrorBase((void *)model, "itemModel", "isEmpty");
}
