#ifndef LA_ITEM_LISTABLE_H
#define LA_ITEM_LISTABLE_H

#include "../liblogicanalyzer.h"
#include "model.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct LAItemListable LAItemListable;
typedef LAErrorCode (*LAItemListableCallback)(LAItemListable *listable, void *node, size_t index, void *data, bool *stopIter);

typedef LAErrorCode (*LAItemListableFnInsertAt)(LAItemListable *listable, const size_t index, void *data, size_t length);
typedef LAErrorCode (*LAItemListableFnRemoveAt)(LAItemListable *listable, const size_t, void **data, size_t *length);
typedef LAErrorCode (*LAItemListableFnGetAt)(LAItemListable *listable, const size_t index, void **data, size_t *length);
typedef LAErrorCode (*LAItemListableFnFind)(const LAItemListable *model, LAItemListableCallback callback, void *data, bool *found, size_t *foundIndex, void **node);
typedef LAErrorCode (*LAItemListableFnFindRemove)(LAItemListable *model, LAItemListableCallback callback, void *data, bool *found);
typedef LAErrorCode (*LAItemListableFnFindRemoveAll)(LAItemListable *model, LAItemListableCallback callback, void *data, size_t *matches);

typedef struct {
  const LAItemModelVTable              modelVTable;
  const LAItemListableFnInsertAt       insertAt;
  const LAItemListableFnRemoveAt       removeAt;
  const LAItemListableFnGetAt          getAt;
  const LAItemListableFnFind           find;
  const LAItemListableFnFindRemove     findRemove;
  const LAItemListableFnFindRemoveAll  findRemoveAll;
} LAItemListableVTable;

extern const LAItemListableVTable LAItemListableVTableBase;

struct LAItemListable {
  LAItemModel model;
};

#define LA_ITEM_LISTABLE(x) ((LAItemListable *)(x))
#define LA_ITEM_LISTABLE_CALLBACK(x) ((LAItemListableCallback)(x))

LAErrorCode LAItemListableDestroy(LAItemListable **listable);
LAErrorCode LAItemListableIncreaseCount(LAItemListable *listable, const size_t amount);
LAErrorCode LAItemListableDecreaseCount(LAItemListable *listable, const size_t amount);
LAErrorCode LAItemListableResetCount(LAItemListable *listable);
LAErrorCode LAItemListableSetNodeLength(LAItemListable *listable, const size_t length);
LAErrorCode LAItemListableGetNodeLength(const LAItemListable *listable, size_t *length);
LAErrorCode LAItemListableIsInfinite(const LAItemListable *listable, bool *isInfinite);
LAErrorCode LAItemListableIsFinite(const LAItemListable *listable, bool *isFinite);
LAErrorCode LAItemListableIsMutable(const LAItemListable *listable, bool *isMutable);
LAErrorCode LAItemListableGetNodeCount(const LAItemListable *listable, size_t *count);
LAErrorCode LAItemListableSetMaxNodeCount(LAItemListable *listable, size_t count);
LAErrorCode LAItemListableGetMaxNodeCount(const LAItemListable *listable, size_t *count);
LAErrorCode LAItemListableIter(LAItemListable *listable, LAItemListableCallback callback, void *data);
LAErrorCode LAItemListableInsert(LAItemListable *listable, void *data, size_t length);
LAErrorCode LAItemListableRemove(LAItemListable *listable, void **data, size_t *length);
LAErrorCode LAItemListableGet(const LAItemListable *listable, void **data, size_t *length);
LAErrorCode LAItemListableInsertAt(LAItemListable *listable, const size_t index, void *data, size_t length);
LAErrorCode LAItemListableRemoveAt(LAItemListable *listable, const size_t index, void **data, size_t *length);
LAErrorCode LAItemListableGetAt(const LAItemListable *listable, const size_t index, void **data, size_t *length);
LAErrorCode LAItemListableFind(const LAItemListable *listable, LAItemListableCallback callback, void *data, bool *found, size_t *foundIndex, void **node);
LAErrorCode LAItemListableFindRemove(LAItemListable *listable, LAItemListableCallback callback, void *data, bool *found);
LAErrorCode LAItemListableFindRemoveAll(LAItemListable *listable, LAItemListableCallback callback, void *data, size_t *matches);
LAErrorCode LAItemListableIsEmpty(const LAItemListable *listable, bool *isEmpty);

#endif //LA_ITEM_LISTABLE_H
