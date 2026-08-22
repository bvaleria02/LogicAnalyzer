#ifndef LA_DEQUE_STORE_H
#define LA_DEQUE_STORE_H

#include "../liblogicanalyzer.h"
#include "model.h"
#include "itemStackable.h"
#include "itemListable.h"
#include "listStore.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct LADequeStore         LADequeStore;
typedef LAListStoreNode LADequeStoreNode;
typedef LAListStoreCallback LADequeStoreCallback;

typedef LAErrorCode (*LADequeStoreFnPushLeft)(LADequeStore *deque, void *data, size_t length);
typedef LAErrorCode (*LADequeStoreFnPopLeft)(LADequeStore *deque, void **data, size_t *length);
typedef LAErrorCode (*LADequeStoreFnPeekLeft)(LADequeStore *deque, void **data, size_t *length);

typedef struct {
  const LAItemStackableVTable  stackableVTable;
  const LADequeStoreFnPushLeft pushLeft;
  const LADequeStoreFnPopLeft  popLeft;
  const LADequeStoreFnPeekLeft peekLeft;
} LADequeStoreVTable;

extern const LADequeStoreVTable LADequeStoreVTableBase;

struct LADequeStore {
  LAItemStackable stackable;
  LAListStore list;
};

#define LA_DEQUE_STORE(x)          ((LADequeStore *)(x))
#define LA_DEQUE_STORE_CALLBACK(x) ((LADequeStoreCallback)(x))

LAErrorCode LADequeStoreInit(LADequeStore *deque, size_t maxNodeCount, bool isInfinite, size_t defaultNodeSize, bool useDefaultSize);
LAErrorCode LADequeStoreDestroy(LADequeStore **deque);
LAErrorCode LADequeStoreIncreaseCount(LADequeStore *deque, const size_t amount);
LAErrorCode LADequeStoreDecreaseCount(LADequeStore *deque, const size_t amount);
LAErrorCode LADequeStoreResetCount(LADequeStore *deque);
LAErrorCode LADequeStoreSetNodeLength(LADequeStore *deque, const size_t length);
LAErrorCode LADequeStoreGetNodeLength(const LADequeStore *deque, size_t *length);
LAErrorCode LADequeStoreIsInfinite(const LADequeStore *deque, bool *isInfinite);
LAErrorCode LADequeStoreIsFinite(const LADequeStore *deque, bool *isFinite);
LAErrorCode LADequeStoreIsMutable(const LADequeStore *deque, bool *isMutable);
LAErrorCode LADequeStoreGetNodeCount(const LADequeStore *deque, size_t *count);
LAErrorCode LADequeStoreSetMaxNodeCount(LADequeStore *deque, size_t count);
LAErrorCode LADequeStoreGetMaxNodeCount(const LADequeStore *deque, size_t *count);
LAErrorCode LADequeStoreIter(LADequeStore *deque, LADequeStoreCallback callback, void *data);
LAErrorCode LADequeStorePush(LADequeStore *deque, void *data, size_t length);
LAErrorCode LADequeStorePop(LADequeStore *deque, void **data, size_t *length);
LAErrorCode LADequeStorePeek(const LADequeStore *deque, void **data, size_t *length);
LAErrorCode LADequeStorePushLeft(LADequeStore *deque, void *data, size_t length);
LAErrorCode LADequeStorePopLeft(LADequeStore *deque, void **data, size_t *length);
LAErrorCode LADequeStorePeekLeft(const LADequeStore *deque, void **data, size_t *length);

#endif //LA_DEQUE_STORE_H
