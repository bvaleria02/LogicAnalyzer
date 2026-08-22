#ifndef LA_QUEUE_STORE_H
#define LA_QUEUE_STORE_H

#include "../liblogicanalyzer.h"
#include "model.h"
#include "itemStackable.h"
#include "itemListable.h"
#include "listStore.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct LAQueueStore         LAQueueStore;
typedef LAListStoreNode LADequeStoreNode;
typedef LAListStoreCallback LADequeStoreCallback;

typedef LAErrorCode (*LAQueueStoreCallback)(LAQueueStore *queue, void *node, size_t index, void *data, bool *stopIter);

typedef struct {
  const LAItemStackableVTable  stackableVTable;
} LAQueueStoreVTable;

extern const LAQueueStoreVTable LAQueueStoreVTableBase;

struct LAQueueStore {
  LAItemStackable stackable;
  LAListStore list;
};

#define LA_QUEUE_STORE(x)          ((LAQueueStore *)(x))
#define LA_QUEUE_STORE_CALLBACK(x) ((LAQueueStoreCallback)(x))

LAErrorCode LAQueueStoreInit(LAQueueStore *queue, size_t maxNodeCount, bool isInfinite, size_t defaultNodeSize, bool useDefaultSize);
LAErrorCode LAQueueStoreDestroy(LAQueueStore **queue);
LAErrorCode LAQueueStoreIncreaseCount(LAQueueStore *queue, const size_t amount);
LAErrorCode LAQueueStoreDecreaseCount(LAQueueStore *queue, const size_t amount);
LAErrorCode LAQueueStoreResetCount(LAQueueStore *queue);
LAErrorCode LAQueueStoreSetNodeLength(LAQueueStore *queue, const size_t length);
LAErrorCode LAQueueStoreGetNodeLength(const LAQueueStore *queue, size_t *length);
LAErrorCode LAQueueStoreIsInfinite(const LAQueueStore *queue, bool *isInfinite);
LAErrorCode LAQueueStoreIsFinite(const LAQueueStore *queue, bool *isFinite);
LAErrorCode LAQueueStoreIsMutable(const LAQueueStore *queue, bool *isMutable);
LAErrorCode LAQueueStoreGetNodeCount(const LAQueueStore *queue, size_t *count);
LAErrorCode LAQueueStoreSetMaxNodeCount(LAQueueStore *queue, size_t count);
LAErrorCode LAQueueStoreGetMaxNodeCount(const LAQueueStore *queue, size_t *count);
LAErrorCode LAQueueStoreIter(LAQueueStore *queue, LAQueueStoreCallback callback, void *data);
LAErrorCode LAQueueStorePush(LAQueueStore *queue, void *data, size_t length);
LAErrorCode LAQueueStorePop(LAQueueStore *queue, void **data, size_t *length);
LAErrorCode LAQueueStorePeek(const LAQueueStore *queue, void **data, size_t *length);

#endif //LA_QUEUE_STORE_H
