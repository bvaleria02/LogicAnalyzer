#ifndef LA_ITEM_MODEL_H
#define LA_ITEM_MODEL_H

#include "../error.h"
#include "../liblogicanalyzer.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct LAItemModel LAItemModel;

typedef LAErrorCode (*LAItemModelCallback)(LAItemModel *model, void *node, size_t index, void *data, bool *stopIter);

typedef LAErrorCode (*LAItemModelFnDestroy)(LAItemModel **model);
typedef LAErrorCode (*LAItemModelFnIncreaseCount)(LAItemModel *model, const size_t amount);
typedef LAErrorCode (*LAItemModelFnDecreaseCount)(LAItemModel *model, const size_t amount);
typedef LAErrorCode (*LAItemModelFnResetCount)(LAItemModel *model);
typedef LAErrorCode (*LAItemModelFnSetNodeLength)(LAItemModel *model, const size_t length);
typedef LAErrorCode (*LAItemModelFnGetNodeLength)(const LAItemModel *model, size_t *length);
typedef LAErrorCode (*LAItemModelFnIsInfinite)(const LAItemModel *model, bool *isInfinite);
typedef LAErrorCode (*LAItemModelFnIsFinite)(const LAItemModel *model, bool *isFinite);
typedef LAErrorCode (*LAItemModelFnIsMutable)(const LAItemModel *model, bool *isMutable);
typedef LAErrorCode (*LAItemModelFnGetNodeCount)(const LAItemModel *model, size_t *count);
typedef LAErrorCode (*LAItemModelFnSetMaxNodeCount)(LAItemModel *model, size_t count);
typedef LAErrorCode (*LAItemModelFnGetMaxNodeCount)(const LAItemModel *model, size_t *count);
typedef LAErrorCode (*LAItemModelFnGetNodeCount)(const LAItemModel *model, size_t *count);
typedef LAErrorCode (*LAItemModelFnIter)(LAItemModel *model, LAItemModelCallback callback, void *data);
typedef LAErrorCode (*LAItemModelFnInsert)(LAItemModel *model, void *data, size_t length);
typedef LAErrorCode (*LAItemModelFnRemove)(LAItemModel *model, void **data, size_t *length);
typedef LAErrorCode (*LAItemModelFnGet)(const LAItemModel *model, void **data, size_t *length);
typedef LAErrorCode (*LAItemModelFnIsEmpty)(const LAItemModel *model, bool *isEmpty);

typedef struct {
  const LAItemModelFnDestroy          destroy;
  const LAItemModelFnIncreaseCount    increaseCount;
  const LAItemModelFnDecreaseCount    decreaseCount;
  const LAItemModelFnResetCount       resetCount;
  const LAItemModelFnSetNodeLength    setNodeLength;
  const LAItemModelFnGetNodeLength    getNodeLength;
  const LAItemModelFnIsInfinite       isInfinite;
  const LAItemModelFnIsFinite         isFinite;
  const LAItemModelFnIsMutable        isMutable;
  const LAItemModelFnGetNodeCount     getNodeCount;
  const LAItemModelFnSetMaxNodeCount  setMaxNodeCount;
  const LAItemModelFnGetMaxNodeCount  getMaxNodeCount;
  const LAItemModelFnIter             iter;
  const LAItemModelFnInsert           insert;
  const LAItemModelFnRemove           remove;
  const LAItemModelFnGet              get;
  const LAItemModelFnIsEmpty          isEmpty;
} LAItemModelVTable;

extern const LAItemModelVTable LAItemModelVTableBase;

struct LAItemModel {
  LAItemModelVTable *vtable;
  size_t nodeCount;
  size_t defaultNodeLength;
  size_t maxNodeCount;
  bool isInfinite;
  bool isMutable;
  bool useDefaultSize;
};

#define LA_ITEM_MODEL(x) ((LAItemModel *)(x))
#define LA_ITEM_MODEL_CALLBACK(x) ((LAItemModelCallback)(x))

LAErrorCode LAItemModelCopyMetadata(LAItemModel *modelSrc, LAItemModel *modelDest);
LAErrorCode LAItemModelInit(LAItemModel *model, size_t maxNodeCount, bool isInfinite, size_t defaultNodeSize, bool useDefaultSize);
LAErrorCode LAItemModelDestroy(LAItemModel **model);
LAErrorCode LAItemModelIncreaseCount(LAItemModel *model, const size_t amount);
LAErrorCode LAItemModelDecreaseCount(LAItemModel *model, const size_t amount);
LAErrorCode LAItemModelResetCount(LAItemModel *model);
LAErrorCode LAItemModelSetNodeLength(LAItemModel *model, const size_t length);
LAErrorCode LAItemModelGetNodeLength(const LAItemModel *model, size_t *length);
LAErrorCode LAItemModelIsInfinite(const LAItemModel *model, bool *isInfinite);
LAErrorCode LAItemModelIsFinite(const LAItemModel *model, bool *isFinite);
LAErrorCode LAItemModelIsMutable(const LAItemModel *model, bool *isMutable);
LAErrorCode LAItemModelGetNodeCount(const LAItemModel *model, size_t *count);
LAErrorCode LAItemModelSetMaxNodeCount(LAItemModel *model, size_t count);
LAErrorCode LAItemModelGetMaxNodeCount(const LAItemModel *model, size_t *count);
LAErrorCode LAItemModelIter(LAItemModel *model, LAItemModelCallback callback, void *data);
LAErrorCode LAItemModelInsert(LAItemModel *model, void *data, size_t length);
LAErrorCode LAItemModelRemove(LAItemModel *model, void **data, size_t *length);
LAErrorCode LAItemModelGet(const LAItemModel *model, void **data, size_t *length);
LAErrorCode LAItemModelIsEmpty(const LAItemModel *model, bool *isEmpty);

#endif //LA_ITEM_MODEL_H
