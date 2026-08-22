#ifndef LA_ITEM_STACKABLE_H
#define LA_ITEM_STACKABLE_H

#include "../liblogicanalyzer.h"
#include "model.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct LAItemStackable LAItemStackable;
typedef LAErrorCode (*LAItemStackableCallback)(LAItemStackable *stackable, void *node, size_t index, void *data, bool *stopIter);

typedef LAErrorCode (*LAItemStackableFnPush)(LAItemStackable *stackable, void *data, size_t length);
typedef LAErrorCode (*LAItemStackableFnPop)(LAItemStackable *stackable, void **data, size_t *length);
typedef LAErrorCode (*LAItemStackableFnPeek)(LAItemStackable *stackable, void **data, size_t *length);

typedef struct {
  const LAItemModelVTable     modelVTable;
  const LAItemStackableFnPush push;
  const LAItemStackableFnPop  pop;
  const LAItemStackableFnPeek peek;
} LAItemStackableVTable;

extern const LAItemStackableVTable LAItemStackableVTableBase;

struct LAItemStackable {
  LAItemModel model;
};

#define LA_ITEM_STACKABLE(x)          ((LAItemStackable *)(x))
#define LA_ITEM_STACKABLE_CALLBACK(x) ((LAItemStackableCallback)(x))

LAErrorCode LAItemStackableInit(LAItemStackable *stackable, size_t maxNodeCount, bool isInfinite, size_t defaultNodeSize, bool useDefaultSize);
LAErrorCode LAItemStackableDestroy(LAItemStackable **stackable);
LAErrorCode LAItemStackableIncreaseCount(LAItemStackable *stackable, const size_t amount);
LAErrorCode LAItemStackableDecreaseCount(LAItemStackable *stackable, const size_t amount);
LAErrorCode LAItemStackableResetCount(LAItemStackable *stackable);
LAErrorCode LAItemStackableSetNodeLength(LAItemStackable *stackable, const size_t length);
LAErrorCode LAItemStackableGetNodeLength(const LAItemStackable *stackable, size_t *length);
LAErrorCode LAItemStackableIsInfinite(const LAItemStackable *stackable, bool *isInfinite);
LAErrorCode LAItemStackableIsFinite(const LAItemStackable *stackable, bool *isFinite);
LAErrorCode LAItemStackableIsMutable(const LAItemStackable *stackable, bool *isMutable);
LAErrorCode LAItemStackableGetNodeCount(const LAItemStackable *stackable, size_t *count);
LAErrorCode LAItemStackableSetMaxNodeCount(LAItemStackable *stackable, size_t count);
LAErrorCode LAItemStackableGetMaxNodeCount(const LAItemStackable *stackable, size_t *count);
LAErrorCode LAItemStackableIter(LAItemStackable *stackable, LAItemStackableCallback callback, void *data);
LAErrorCode LAItemStackablePush(LAItemStackable *stackable, void *data, size_t length);
LAErrorCode LAItemStackablePop(LAItemStackable *stackable, void **data, size_t *length);
LAErrorCode LAItemStackablePeek(const LAItemModel *stackable, void **data, size_t *length);

#endif //LA_ITEM_STACKABLE_H
