#include "../liblogicanalyzer.h"
#include "../utils.h"
#include "model.h"
#include "itemListable.h"
#include "error.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

const LAItemListableVTable LAItemListableVTableBase = {
  .modelVTable = {
    .destroy         = (LAItemModelFnDestroy) LAItemListableDestroy,
    .increaseCount   = (LAItemModelFnIncreaseCount) LAItemListableIncreaseCount,
    .decreaseCount   = (LAItemModelFnDecreaseCount) LAItemListableDecreaseCount,
    .resetCount      = (LAItemModelFnResetCount) LAItemListableResetCount,
    .setNodeLength   = (LAItemModelFnSetNodeLength) LAItemListableSetNodeLength,
    .getNodeLength   = (LAItemModelFnGetNodeLength) LAItemListableGetNodeLength,
    .isInfinite      = (LAItemModelFnIsInfinite) LAItemListableIsInfinite,
    .isFinite        = (LAItemModelFnIsFinite) LAItemListableIsFinite,
    .isMutable       = (LAItemModelFnIsMutable) LAItemListableIsMutable,
    .getNodeCount    = (LAItemModelFnGetNodeCount) LAItemListableGetNodeCount,
    .setMaxNodeCount = (LAItemModelFnSetMaxNodeCount) LAItemListableSetMaxNodeCount,
    .getMaxNodeCount = (LAItemModelFnGetMaxNodeCount) LAItemListableGetMaxNodeCount,
    .iter            = (LAItemModelFnIter) LAItemListableIter,
    .insert          = (LAItemModelFnInsert) LAItemListableInsert,
    .remove          = (LAItemModelFnRemove) LAItemListableRemove,
    .get             = (LAItemModelFnGet) LAItemListableGet,
  },
  .insertAt        = (LAItemListableFnInsertAt) LAItemListableInsertAt,
  .removeAt        = (LAItemListableFnRemoveAt) LAItemListableRemoveAt,
  .getAt           = (LAItemListableFnGetAt) LAItemListableGetAt,
  .find            = (LAItemListableFnFind) LAItemListableFind,
  .findRemove      = (LAItemListableFnFindRemove) LAItemListableFindRemove,
  .findRemoveAll   = (LAItemListableFnFindRemoveAll) LAItemListableFindRemoveAll
};

LAErrorCode LAItemListableInit(LAItemListable *listable, size_t maxNodeCount, bool isInfinite, size_t defaultNodeSize, bool useDefaultSize){
  LA_HANDLE_NULLPTR(listable, LA_PROPAGATE_ERROR);
  
  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelInit(LA_ITEM_MODEL(listable), maxNodeCount, isInfinite, defaultNodeSize, useDefaultSize);
  
  listable->model.vtable = (LAItemModelVTable *) &(LAItemListableVTableBase);
  
  return code;    
}

LAErrorCode LAItemListableDestroy(LAItemListable **listable){
  LA_HANDLE_NULLPTR(listable, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  LAItemModel *model = LA_ITEM_MODEL(*listable);
  code = LAItemModelDestroy(&model);
  
  return code;  
}

LAErrorCode LAItemListableIncreaseCount(LAItemListable *listable, const size_t amount){
  LA_HANDLE_NULLPTR(listable, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelIncreaseCount(LA_ITEM_MODEL(listable), amount);
  
  return code;  
}

LAErrorCode LAItemListableDecreaseCount(LAItemListable *listable, const size_t amount){
  LA_HANDLE_NULLPTR(listable, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelDecreaseCount(LA_ITEM_MODEL(listable), amount);
  
  return code;  
}

LAErrorCode LAItemListableResetCount(LAItemListable *listable){
  LA_HANDLE_NULLPTR(listable, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelResetCount(LA_ITEM_MODEL(listable));
  
  return code;  
}

LAErrorCode LAItemListableSetNodeLength(LAItemListable *listable, const size_t length){
  LA_HANDLE_NULLPTR(listable, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelSetNodeLength(LA_ITEM_MODEL(listable), length);
  
  return code;  
}

LAErrorCode LAItemListableGetNodeLength(const LAItemListable *listable, size_t *length){
  LA_HANDLE_NULLPTR(listable, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelGetNodeLength(LA_ITEM_MODEL(listable), length);
  
  return code;  
}

LAErrorCode LAItemListableIsInfinite(const LAItemListable *listable, bool *isInfinite){
  LA_HANDLE_NULLPTR(listable,      LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(isInfinite, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelIsInfinite(LA_ITEM_MODEL(listable), isInfinite);
  
  return code;  
}

LAErrorCode LAItemListableIsFinite(const LAItemListable *listable, bool *isFinite){
  LA_HANDLE_NULLPTR(listable, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelIsFinite(LA_ITEM_MODEL(listable), isFinite);
  
  return code;  
}

LAErrorCode LAItemListableIsMutable(const LAItemListable *listable, bool *isMutable){
  LA_HANDLE_NULLPTR(listable, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelIsMutable(LA_ITEM_MODEL(listable), isMutable);
  
  return code;  
}

LAErrorCode LAItemListableGetNodeCount(const LAItemListable *listable, size_t *count){
  LA_HANDLE_NULLPTR(listable, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(count, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelGetNodeCount(LA_ITEM_MODEL(listable), count);
  
  return code;  
}

LAErrorCode LAItemListableSetMaxNodeCount(LAItemListable *listable, size_t count){
  LA_HANDLE_NULLPTR(listable, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelSetMaxNodeCount(LA_ITEM_MODEL(listable), count);
  
  return code;  
}

LAErrorCode LAItemListableGetMaxNodeCount(const LAItemListable *listable, size_t *count){
  LA_HANDLE_NULLPTR(listable, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(count, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelGetMaxNodeCount(LA_ITEM_MODEL(listable), count);
  
  return code;  
}

LAErrorCode LAItemListableIter(LAItemListable *listable, LAItemListableCallback callback, void *data){
  LA_HANDLE_NULLPTR(listable, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(callback, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelIter(LA_ITEM_MODEL(listable), LA_ITEM_MODEL_CALLBACK(callback), data);
  
  return code;  
}

LAErrorCode LAItemListableInsert(LAItemListable *listable, void *data, size_t length){
  LA_HANDLE_NULLPTR(listable, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data,  LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelInsert(LA_ITEM_MODEL(listable), data, length);
  
  return code;  
}

LAErrorCode LAItemListableRemove(LAItemListable *listable, void **data, size_t *length){
  LA_HANDLE_NULLPTR(listable, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelRemove(LA_ITEM_MODEL(listable), data, length);
  
  return code;  
}

LAErrorCode LAItemListableGet(const LAItemListable *listable, void **data, size_t *length){
  LA_HANDLE_NULLPTR(listable,  LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data,   LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelGet(LA_ITEM_MODEL(listable), data, length);
  
  return code;  
}

LAErrorCode LAItemListableInsertAt(LAItemListable *listable, const size_t index, void *data, size_t length){
  LA_HANDLE_NULLPTR(listable, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data,  LA_PROPAGATE_ERROR);

  (void) listable;
  (void) index;
  (void) data;
  (void) length;
  return LALogStructureErrorBase((void *)listable, "itemListable", "insertAt");
}

LAErrorCode LAItemListableRemoveAt(LAItemListable *listable, const size_t index, void **data, size_t *length){
  LA_HANDLE_NULLPTR(listable, LA_PROPAGATE_ERROR);
  
  (void) listable;
  (void) index;
  (void) data;
  (void) length;
  return LALogStructureErrorBase((void *)listable, "itemListable", "removeAt");
}

LAErrorCode LAItemListableGetAt(const LAItemListable *listable, const size_t index, void **data, size_t *length){
  LA_HANDLE_NULLPTR(listable,  LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data,   LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);
  
  (void) listable;
  (void) index;
  (void) data;
  (void) length;
  return LALogStructureErrorBase((void *)listable, "itemListable", "getAt");
}

LAErrorCode LAItemListableFind(const LAItemListable *listable, LAItemListableCallback callback, void *data, bool *found, size_t *foundIndex, void **node){
  LA_CHECK_NULLPTR(listable);
  LA_CHECK_NULLPTR(callback);
  LA_CHECK_NULLPTR(found);
  LA_CHECK_NULLPTR(foundIndex);
  LA_CHECK_NULLPTR(node);
  
  (void) listable;
  (void) callback;
  (void) data;
  (void) found;
  (void) foundIndex;
  (void) node;
  return LALogStructureErrorBase(listable, "itemListable", "find");
}

LAErrorCode LAItemListableFindRemove(LAItemListable *listable, LAItemListableCallback callback, void *data, bool *found){
  LA_CHECK_NULLPTR(listable);
  LA_CHECK_NULLPTR(callback);
  LA_CHECK_NULLPTR(found);
  
  (void) listable;
  (void) callback;
  (void) data;
  (void) found;
  return LALogStructureErrorBase(listable, "itemListable", "findRemove");
}

LAErrorCode LAItemListableFindRemoveAll(LAItemListable *listable, LAItemListableCallback callback, void *data, size_t *matches){
  LA_CHECK_NULLPTR(listable);
  LA_CHECK_NULLPTR(callback);
  LA_CHECK_NULLPTR(matches);
  
  (void) listable;
  (void) callback;
  (void) data;
  (void) matches;
  return LALogStructureErrorBase(listable, "itemListable", "findRemoveAll");
}
