#ifndef LA_VECTOR_STORE_H
#define LA_VECTOR_STORE_H

#include "../liblogicanalyzer.h"
#include "model.h"
#include "itemListable.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct LAVectorStore           LAVectorStore;
typedef struct _la_vector_store_node   LAVectorStoreNode;
typedef struct _la_vector_store_config LAVectorStoreConfig;

typedef LAErrorCode (*LAVectorStoreCallback)(LAVectorStore *vector, LAVectorStoreNode *node, size_t index, void *data, bool *stopIter);

typedef LAErrorCode (*LAVectorStoreFnOptimizeSize)(LAVectorStore *vector);
typedef LAErrorCode (*LAVectorStoreFnResize)(LAVectorStore *vector, const size_t targetLength);
typedef LAErrorCode (*LAVectorStoreFnCompact)(LAVectorStore *vector);
typedef LAErrorCode (*LAVectorStoreFnClearAt)(LAVectorStore *vector, const size_t index, void **data, size_t *length);
typedef LAErrorCode (*LAVectorStoreFnFindCLear)(LAVectorStore *vector, LAVectorStoreCallback callback, void *data, bool *found);
typedef LAErrorCode (*LAVectorStoreFnFindClearAll)(LAVectorStore *vector, LAVectorStoreCallback callback, void *data, size_t *matches);

struct _la_vector_store_config {
  double growthFactor;
  double growthThreshold;
  double shrinkFactor;
  double shrinkThreshold;
  size_t capacity;
};


typedef struct {
  const LAItemListableVTable          listableVTable;
  const LAVectorStoreFnOptimizeSize   optimizeSize;
  const LAVectorStoreFnResize         resize;
  const LAVectorStoreFnCompact        compact;
  const LAVectorStoreFnClearAt        clearAt;
  const LAVectorStoreFnFindRemove     findClear;
  const LAVectorStoreFnFindRemoveAll  findClearAll;
} LAVectorStoreVTable;


struct _la_vector_store_node {
  size_t length;
  uint8_t data[];
};

struct LAVectorStore {
  LAItemListable listable;
  LAVectorStoreNode **nodes;
  size_t highestIndex;
  LAVectorStoreConfig config;
};

extern const LAVectorStoreVTable LAVectorStoreVTableBase;

#define LA_VECTOR_STORE(x) ((LAVectorStore *)(x))
#define LA_VECTOR_STORE_CALLBACK(x) ((LAVectorStoreCallback)(x))

LAErrorCode LAVectorStoreCreateNode(LAVectorStoreNode **node, const size_t nodeLength, const size_t dataLength, const void *data, const LAVectorStoreSentinel sentinel, const bool useNodeLength, void *extraData);
LAErrorCode LAVectorStoreInit(LAVectorStore *vector, size_t maxNodeCount, bool isInfinite, size_t defaultNodeSize, bool useDefaultSize);
LAErrorCode LAVectorStoreDestroy(LAVectorStore **vector);
LAErrorCode LAVectorStoreIncreaseCount(LAVectorStore *vector, const size_t amount);
LAErrorCode LAVectorStoreDecreaseCount(LAVectorStore *vector, const size_t amount);
LAErrorCode LAVectorStoreResetCount(LAVectorStore *vector);
LAErrorCode LAVectorStoreSetNodeLength(LAVectorStore *vector, const size_t length);
LAErrorCode LAVectorStoreGetNodeLength(const LAVectorStore *vector, size_t *length);
LAErrorCode LAVectorStoreSetMaxNodeCount(LAVectorStore *list, size_t count);
LAErrorCode LAVectorStoreGetMaxNodeCount(const LAVectorStore *list, size_t *count);
LAErrorCode LAVectorStoreIsInfinite(const LAVectorStore *vector, bool *isInfinite);
LAErrorCode LAVectorStoreIsFinite(const LAVectorStore *vector, bool *isFinite);
LAErrorCode LAVectorStoreIsMutable(const LAVectorStore *vector, bool *isMutable);
LAErrorCode LAVectorStoreGetNodeCount(const LAVectorStore *vector, size_t *count);
LAErrorCode LAVectorStoreIter(LAVectorStore *vector, LAVectorStoreCallback callback, void *data);
LAErrorCode LAVectorStoreInsert(LAVectorStore *vector, void *data, size_t length);
LAErrorCode LAVectorStoreRemove(LAVectorStore *vector, void **data, size_t *length);
LAErrorCode LAVectorStoreGet(const LAVectorStore *vector, void **data, size_t *length);
LAErrorCode LAVectorStoreInsertAt(LAVectorStore *vector, const size_t index, void *data, size_t length);
LAErrorCode LAVectorStoreRemoveAt(LAVectorStore *vector, const size_t index, void **data, size_t *length);
LAErrorCode LAVectorStoreGetAt(const LAVectorStore *vector, const size_t index, void **data, size_t *length);
LAErrorCode LAVectorStoreFind(const LAVectorStore *vector, LAVectorStoreCallback callback, void *data, bool *found, size_t *foundIndex, void **nodeData);
LAErrorCode LAVectorStoreFindRemove(LAVectorStore *vector, LAVectorStoreCallback callback, void *data, bool *found);
LAErrorCode LAVectorStoreFindRemoveAll(LAVectorStore *vector, LAVectorStoreCallback callback, void *data, size_t *matches);
LAErrorCode LAVectorStoreOptimizeSize(LAVectorStore *vector);
LAErrorCode LAVectorStoreResize(LAVectorStore *vector, const size_t targetLength);
LAErrorCode LAVectorStoreCompact(LAVectorStore *vector);
LAErrorCode LAVectorStoreClearAt(LAVectorStore *vector, const size_t index, void **data, size_t *length);
LAErrorCode LAVectorStoreFindCLear(LAVectorStore *vector, LAVectorStoreCallback callback, void *data, bool *found);
LAErrorCode LAVectorStoreFindClearAll(LAVectorStore *vector, LAVectorStoreCallback callback, void *data, size_t *matches);

#endif //LA_VECTOR_STORE_H
