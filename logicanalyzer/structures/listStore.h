#ifndef LA_LIST_STORE_H
#define LA_LIST_STORE_H

#include "../liblogicanalyzer.h"
#include "model.h"
#include "itemListable.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct LAListStore         LAListStore;
typedef struct _la_list_store_node LAListStoreNode;

typedef LAErrorCode (*LAListStoreCallback)(LAListStore *listStore, LAListStoreNode *node, size_t index, void *data, bool *stopIter);

typedef struct {
  LAItemListableVTable  listableVTable;
} LAListStoreVTable;

typedef enum {
    LA_LIST_STORE_SENTINEL_NONE = 0,
    LA_LIST_STORE_SENTINEL_HEAD = 1,
    LA_LIST_STORE_SENTINEL_TAIL = 2
} LAListStoreSentinel;

struct _la_list_store_node {
  struct _la_list_store_node *prev;
  struct _la_list_store_node *next;
  LAListStoreSentinel sentinel;
  size_t length;
  uint8_t data[];
};

struct LAListStore {
  LAItemListable listable;
  LAListStoreNode *head;
  LAListStoreNode *tail;
};

extern const LAListStoreVTable LAListStoreVTableBase;

#define LA_LIST_STORE(x) ((LAListStore *)(x))
#define LA_LIST_STORE_CALLBACK(x) ((LAListStoreCallback)(x))

LAErrorCode LAListStoreCreateNode(LAListStoreNode **node, const size_t nodeLength, const size_t dataLength, const void *data, const LAListStoreSentinel sentinel, const bool useNodeLength);
LAErrorCode LAListStoreInit(LAListStore *list, size_t maxNodeCount, bool isInfinite, size_t defaultNodeSize, bool useDefaultSize);
LAErrorCode LAListStoreDestroy(LAListStore **listStore);
LAErrorCode LAListStoreIncreaseCount(LAListStore *listStore, const size_t amount);
LAErrorCode LAListStoreDecreaseCount(LAListStore *listStore, const size_t amount);
LAErrorCode LAListStoreResetCount(LAListStore *listStore);
LAErrorCode LAListStoreSetNodeLength(LAListStore *listStore, const size_t length);
LAErrorCode LAListStoreGetNodeLength(const LAListStore *listStore, size_t *length);
LAErrorCode LAListStoreSetMaxNodeCount(LAListStore *list, size_t count);
LAErrorCode LAListStoreGetMaxNodeCount(const LAListStore *list, size_t *count);
LAErrorCode LAListStoreIsInfinite(const LAListStore *listStore, bool *isInfinite);
LAErrorCode LAListStoreIsFinite(const LAListStore *listStore, bool *isFinite);
LAErrorCode LAListStoreIsMutable(const LAListStore *listStore, bool *isMutable);
LAErrorCode LAListStoreGetNodeCount(const LAListStore *listStore, size_t *count);
LAErrorCode LAListStoreIter(LAListStore *listStore, LAListStoreCallback callback, void *data);
LAErrorCode LAListStoreInsert(LAListStore *listStore, void *data, size_t length);
LAErrorCode LAListStoreRemove(LAListStore *listStore, void **data, size_t *length);
LAErrorCode LAListStoreGet(const LAListStore *listStore, void **data, size_t *length);
LAErrorCode LAListStoreInsertAt(LAListStore *listStore, const size_t index, void *data, size_t length);
LAErrorCode LAListStoreRemoveAt(LAListStore *listStore, const size_t index, void **data, size_t *length);
LAErrorCode LAListStoreGetAt(const LAListStore *listStore, const size_t index, void **data, size_t *length);
LAErrorCode LAListStoreFind(const LAListStore *listStore, LAListStoreCallback callback, void *data, bool *found, size_t *foundIndex, void **nodeData);
LAErrorCode LAListStoreFindRemove(LAListStore *listStore, LAListStoreCallback callback, void *data, bool *found);
LAErrorCode LAListStoreFindRemoveAll(LAListStore *listStore, LAListStoreCallback callback, void *data, size_t *matches);
LAErrorCode LAListStoreIsEmpty(const LAListStore *list, bool *isEmpty);

#endif //LA_LIST_STORE_H
