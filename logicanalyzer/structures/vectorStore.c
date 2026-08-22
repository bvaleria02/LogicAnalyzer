
#include "../liblogicanalyzer.h"
#include "../utils.h"
#include "model.h"
#include "itemListable.h"
#include "vectorStore.h"
#include "error.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const LAVectorStoreVTable LAVectorStoreVTableBase = {
  .listableVTable = {
    .modelVTable = {
      .destroy         = (LAItemModelFnDestroy) LAVectorStoreDestroy,
      .increaseCount   = (LAItemModelFnIncreaseCount) LAVectorStoreIncreaseCount,
      .decreaseCount   = (LAItemModelFnDecreaseCount) LAVectorStoreDecreaseCount,
      .resetCount      = (LAItemModelFnResetCount) LAVectorStoreResetCount,
      .setNodeLength   = (LAItemModelFnSetNodeLength) LAVectorStoreSetNodeLength,
      .getNodeLength   = (LAItemModelFnGetNodeLength) LAVectorStoreGetNodeLength,
      .isInfinite      = (LAItemModelFnIsInfinite) LAVectorStoreIsInfinite,
      .isFinite        = (LAItemModelFnIsFinite) LAVectorStoreIsFinite,
      .isMutable       = (LAItemModelFnIsMutable) LAVectorStoreIsMutable,
      .getNodeCount    = (LAItemModelFnGetNodeCount) LAVectorStoreGetNodeCount,
      .setMaxNodeCount = (LAItemModelFnSetMaxNodeCount) LAVectorStoreSetMaxNodeCount,
      .getMaxNodeCount = (LAItemModelFnGetMaxNodeCount) LAVectorStoreGetMaxNodeCount,
      .iter            = (LAItemModelFnIter) LAVectorStoreIter,
      .insert          = (LAItemModelFnInsert) LAVectorStoreInsert,
      .remove          = (LAItemModelFnRemove) LAVectorStoreRemove,
      .get             = (LAItemModelFnGet) LAVectorStoreGet,
    },
    .insertAt        = (LAItemListableFnInsertAt) LAVectorStoreInsertAt,
    .removeAt        = (LAItemListableFnRemoveAt) LAVectorStoreRemoveAt,
    .getAt           = (LAItemListableFnGetAt) LAVectorStoreGetAt,
    .find            = (LAItemListableFnFind) LAVectorStoreFind,
    .findRemove      = (LAItemListableFnFindRemove) LAVectorStoreFindRemove,
    .findRemoveAll   = (LAItemListableFnFindRemoveAll) LAVectorStoreFindRemoveAll,
  },
  .optimizeSize   = (LAVectorStoreFnOptimizeSize) LAVectorStoreOptimizeSize,
  .resize         = (LAVectorStoreFnResize) LAVectorStoreResize,
  .compact        = (LAVectorStoreFnCompact) LAVectorStoreCompact,
  .clearAt        = (LAVectorStoreFnClearAt) LAVectorStoreClearAt,
  .findClear      = (LAVectorStoreFnFindClear) LAVectorStoreFindClear,
  .findClearAll   = (LAVectorStoreFnFindClearAll) LAVectorStoreFindClearAll,
};

LAErrorCode LASafeSizeMul(const size_t a, const size_t b, size_t *result){
  LA_CHECK_NULLPTR(result);

  // Zero case
  if((a == 0) || (b == 0)){
    (*result) = 0;
    return LA_NO_ERROR;
  }

  if(b > (SIZE_MAX / a)){
    return LA_ERROR_INT_OVERFLOW;
  }

  (*result) = a * b;
  
  
  return LA_NO_ERROR;
}

LAErrorCode LASafeSizeAdd(const size_t a, const size_t b, size_t *result){
  LA_CHECK_NULLPTR(result);

  if(b > (SIZE_MAX  - a)){
    return LA_ERROR_INT_OVERFLOW;
  }

  (*result) = a + b;
  
  
  return LA_NO_ERROR;
}

LAErrorCode LAVectorStoreInit(LAVectorStore *vector, size_t maxNodeCount, bool isInfinite, size_t defaultNodeSize, bool useDefaultSize, void *extraData){
  LA_CHECK_NULLPTR(vector);
  LA_CHECK_NULLPTR(extraData);

  LAVectorStoreConfig *config = (LAVectorStoreConfig *)extraData;
  if(config->capacity == 0) return LA_ERROR_ZEROLENGTH;
  
  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelInit(LA_ITEM_MODEL(list), maxNodeCount, isInfinite, 0, false);
  if(code) goto cleanup;
  
  vector->listable.model.vtable = (LAItemModelVTable *) &(LAVectorStoreVTableBase);

  memcpy(&(vector->config), config, sizeof(LAVectorStoreConfig));
  vector->highestIndex = 0;

  size_t nodeBufferSize = 0;
  code = LASafeMul64(sizeof(LAVectorStoreNode *), config->capacity, &nodeBufferSize);
  if(code) goto cleanup;
  
  vector->nodes = NULL;
  vector->nodes = (LAVectorStoreNode **)malloc(nodeBufferSize);
  if(vector->nodes == NULL) goto malloc_error;

  memset(vector->nodes, 0l, nodeBufferSize);

  (void) defaultNodeSize;
  (void) useDefaultSize;
  return LA_NO_ERROR;

malloc_error:
  code = LA_ERROR_MALLOC;
  goto cleanup;

cleanup:
  if(vector->nodes != NULL) free(vector->nodes);
  code = LAItemListableDestroy(&(LA_ITEM_LISTABLE(vector)));
  return code;
}

LAErrorCode LAVectorStoreDestroy(LAVectorStore **vector){
  LA_CHECK_NULLPTR(vector);

  for(size_t i = 0; i < (*vector)->config.capacity, i++){
    if((*vector)->nodes[i] != NULL) free((*vector)->nodes[i]);
  }

  if((*vector)->nodes != NULL) free((*vector)->nodes);

  LAErrorCode code = LA_NO_ERROR;
  LAItemListable *listable = LA_ITEM_LISTABLE(*vector);
  code = LAItemListableDestroy(&listable);
  
  return code;  
}

LAErrorCode LAVectorStoreIncreaseCount(LAVectorStore *vector, const size_t amount){
  LA_CHECK_NULLPTR(vector);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableIncreaseCount(LA_ITEM_LISTABLE(vector), amount);
  
  return code;  
}

LAErrorCode LAVectorStoreDecreaseCount(LAVectorStore *vector, const size_t amount){
  LA_CHECK_NULLPTR(vector);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableDecreaseCount(LA_ITEM_LISTABLE(vector), amount);
  
  return code;  
}

LAErrorCode LAVectorStoreResetCount(LAVectorStore *vector){
  LA_CHECK_NULLPTR(vector);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableResetCount(LA_ITEM_LISTABLE(vector));
  
  return code;  
}

LAErrorCode LAVectorStoreSetNodeLength(LAVectorStore *vector, const size_t length){
  LA_CHECK_NULLPTR(vector);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableSetNodeLength(LA_ITEM_LISTABLE(vector), length);
  
  return code;  
}

LAErrorCode LAVectorStoreGetNodeLength(const LAVectorStore *vector, size_t *length){
  LA_CHECK_NULLPTR(vector);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableGetNodeLength(LA_ITEM_LISTABLE(vector), length);
  
  return code;  
}

LAErrorCode LAVectorStoreIsInfinite(const LAVectorStore *vector, bool *isInfinite){
  LA_CHECK_NULLPTR(vector);
  LA_HANDLE_NULLPTR(isInfinite, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableIsInfinite(LA_ITEM_LISTABLE(vector), isInfinite);
  
  return code;  
}

LAErrorCode LAVectorStoreIsFinite(const LAVectorStore *vector, bool *isFinite){
  LA_CHECK_NULLPTR(vector);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableIsFinite(LA_ITEM_LISTABLE(vector), isFinite);
  
  return code;  
}

LAErrorCode LAVectorStoreIsMutable(const LAVectorStore *vector, bool *isMutable){
  LA_CHECK_NULLPTR(vector);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableIsMutable(LA_ITEM_LISTABLE(vector), isMutable);
  
  return code;  
}

LAErrorCode LAVectorStoreGetNodeCount(const LAVectorStore *vector, size_t *count){
  LA_CHECK_NULLPTR(vector);
  LA_HANDLE_NULLPTR(count, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableGetNodeCount(LA_ITEM_LISTABLE(vector), count);
  
  return code;  
}

LAErrorCode LAVectorStoreSetMaxNodeCount(LAVectorStore *vector, size_t count){
  LA_CHECK_NULLPTR(vector);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableSetMaxNodeCount(LA_ITEM_LISTABLE(vector), count);
  
  return code;  
}

LAErrorCode LAVectorStoreGetMaxNodeCount(const LAVectorStore *vector, size_t *count){
  LA_CHECK_NULLPTR(vector);
  LA_HANDLE_NULLPTR(count, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableGetMaxNodeCount(LA_ITEM_LISTABLE(vector), count);
  
  return code;  
}

LAErrorCode LAVectorStoreIterator_backend(LAVectorStore *vector, LAVectorStoreCallback callback, void *data, bool *found, size_t *foundIndex, LAVectorStoreNode **foundNode, size_t indexStart, bool *hasFinished){
  LA_CHECK_NULLPTR(vector);
  LA_CHECK_NULLPTR(callback);

  LAErrorCode code = LA_NO_ERROR;
  
  // Initializes for search
  if(found != NULL)       (*found)     = false;
  if(foundNode != NULL)   (*foundNode) = NULL;
  if(hasFinished != NULL) (*hasFinished) = false;
  
  bool stopIter = false;
  if(hasFinished != NULL){
    if(indexStart >= vector->config.capacity){
      (*hasFinished) = true;
      return LA_NO_ERROR;
    }
  }

  for(size_t i = 0; i < vector->config.capacity; i++){
    LAVectorStoreNode *node = vector->nodes[i];
    
    if(hasFinished != NULL){
      if(i == (vector->config.capacity - 1)){
        (*hasFinished) = true;
      }
    }
    // Skip NULL nodes
    if(node){
      continue;
    }

    // Actual iteration
    code = callback(LA_VECTOR_STORE(vector), node, i, data, &stopIter);
    if(code) break;

    // Continues if stopIter is false
    if(!stopIter){
      continue;
    }

    // stopIter is true, handle match, and break;
    if(found != NULL)      (*found)      = true;
    if(foundIndex != NULL) (*foundIndex) = i;
    if(foundNode != NULL)  (*foundNode)  = node;
    break;
  }
  
  return code;
}

LAErrorCode LAVectorStoreIter(LAVectorStore *vector, LAVectorStoreCallback callback, void *data){
  LA_CHECK_NULLPTR(vector);
  LA_CHECK_NULLPTR(callback);

  LAErrorCode code = LA_NO_ERROR;
  
  code = LAVectorStoreIterator_backend(
              vector,
              callback,
              data,
              NULL,
              NULL,
              NULL,
              NULL,
              NULL
            );
  
  return code;  
}

LAErrorCode LAVectorStoreCreateNode(LAVectorStoreNode **node, void *data, size_t length){
  LA_CHECK_NULLPTR(node);
  LA_CHECK_NULLPTR(data);

  LAErrorCode code = LA_NO_ERROR;
  size_t totalLength = 0;
  code = LASafeAdd64(sizeof(LAVectorStoreNode), length, &totalLength);
  if(code) goto cleanup;

  (*node) = NULL;
  (*node) = (LAVectorStoreNode *)malloc(totalLength);
  if((*node) == NULL) goto malloc_error;

  (*node)->length = length;
  memcpy((*node)->data, data, length);

  return LA_NO_ERROR;

malloc_error:
  code = LA_ERROR_MALLOC;
  goto cleanup;

cleanup:
  if((*node) != NULL) free((*node));
  return code;
}

LAErrorCode LAVectorStoreClearNode(LAVectorStoreNode **node, void **data, size_t *length){
  LA_CHECK_NULLPTR(node);

  LAErrorCode code = LA_NO_ERROR;

  if((*node) == NULL){
    if(data != NULL) (*data) = NULL;
    if(length != NULL) (*length) = 0;
  }

  if(length != NULL) (*length) = (*node)->length;
  
  if(data != NULL){
    (*data) = NULL;
    (*data) = (void *)malloc((*node)->length);
    if((*data) == NULL) goto malloc_error;
    memcpy((*data), (*node)->data, (*node)->length);
  }

  free((*node));
  return LA_NO_ERROR;

malloc_error:
  code = LA_ERROR_MALLOC;
  goto cleanup;

cleanup:
  if(data != NULL) {
    if((*data) != NULL) {
      free((*data));
    }
  }
  return code;
}

LAErrorCode LAVectorStoreGetNextTopIndex(LAVectorStore *vector, size_t *index){
  LA_CHECK_NULLPTR(vector);
  LA_CHECK_NULLPTR(index);
  
  if(((LA_ITEM_MODEL(vector))->nodeLength) == 0){
    (*index) = 0;
  } else {
    code = LASafeSizeAdd(vector->highestIndex, 1, index);
    if(code) goto cleanup;
  }

  // But, what if the index is higher than the capacity
  if((LA_ITEM_MODEL(vector))->isInfinite) return LA_NO_ERROR;
  
  if((*index) >= (LA_ITEM_MODEL(vector))->maxNodeLength){
    return LA_ERROR_OUTOFBOUND;
  }

  return LA_NO_ERROR;
}

LAErrorCode LAVectorStoreInsert(LAVectorStore *vector, void *data, const size_t length){
  LA_CHECK_NULLPTR(vector);
  LA_CHECK_NULLPTR(data);

  LAErrorCode code = LA_NO_ERROR;

  size_t index = 0;
  code = LAVectorStoreGetNextTopIndex(vector, &index);
  if(code) goto cleanup;

  
  LAVectorStoreNode *newNode = NULL;

  code = LAVectorStoreCreateNode(&newNode, data, length);
  if(code) goto cleanup;

  return LA_NO_ERROR;
  
cleanup:
  if(newNode != NULL) free(newNode);
  return code;
}


LAErrorCode LAVectorStoreRemove(LAVectorStore *vector, void **data, size_t *length){
  LA_CHECK_NULLPTR(vector);

  if(data != NULL) (*data) = NULL;

  LAErrorCode code = LA_NO_ERROR;
  
  LAVectorStoreNode *node = (list->tail)->prev;

  if((node != NULL) && (node->sentinel == LA_LIST_STORE_SENTINEL_NONE) && (data != NULL)){
    (*data) = malloc(node->length);
    if((*data) == NULL) return LA_ERROR_MALLOC;
    memcpy((*data), node->data, node->length);
    if(length != NULL) (*length) = node->length;
  }

  code = LAVectorStoreRemoveBetweenNodes(node);
  if(code) return code;

  if(node != NULL) free(node);
  
  (void) data;
  (void) length;
  return code;  
}

LAErrorCode LAVectorStoreGet(const LAVectorStore *vector, void **data, size_t *length){
  LA_CHECK_NULLPTR(vector);
  LA_HANDLE_NULLPTR(data,   LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;

  LAVectorStoreNode *node = (list->tail)->prev;

  if(node == NULL) return LA_ERROR_NULLPTR;
  if(node->sentinel != LA_LIST_STORE_SENTINEL_NONE){
    (*data) = NULL;
    (*length) = 0x0;
    return code;
  } else {
    (*data) = node->data;
    (*length) = node->length;
  }
  
  return code;  
}

LAErrorCode LAVectorStoreIndexSearchCallback(LAVectorStore *vector, LAVectorStoreNode *node, size_t index, void *data, bool *stopIter){
  size_t targetIndex = *(size_t *)data;

  if(targetIndex == index){
    (*stopIter) = true;
  }

  (void) list;
  (void) node;
  return LA_NO_ERROR;
}

LAErrorCode LAVectorStoreInsertAt(LAVectorStore *vector, const size_t index, void *data, size_t length){
  LA_CHECK_NULLPTR(vector);
  LA_HANDLE_NULLPTR(data,  LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;

  bool found = false;
  LAVectorStoreNode *foundNode = NULL;

  code = LAVectorStoreIterator_backend(
              LA_LIST_STORE(list),
              LAVectorStoreIndexSearchCallback,
              (void *)(&index),
              &found,
              NULL,
              &foundNode,
              NULL,
              NULL
            );

  if(!found) return LA_ERROR_OUTOFBOUND;

  LAVectorStoreNode *newNode = NULL;
  code = LAVectorStoreCreateNode(
            &newNode,
            list->listable.model.defaultNodeLength,
            length,
            data,
            LA_LIST_STORE_SENTINEL_NONE,
            list->listable.model.useDefaultSize
        );
  if(code) goto cleanup;

  code = LAVectorStoreInsertBetweenNodes(foundNode->prev, foundNode, newNode);
  if(code) goto cleanup;
  
  return LA_NO_ERROR;
  
cleanup:
  if(newNode != NULL) free(newNode);
  return code;
}

LAErrorCode LAVectorStoreRemoveAt(LAVectorStore *vector, const size_t index, void **data, size_t *length){
  LA_CHECK_NULLPTR(vector);
  
  LAErrorCode code = LA_NO_ERROR;
  bool found = false;
  LAVectorStoreNode *foundNode = NULL;

  code = LAVectorStoreIterator_backend(
              LA_LIST_STORE(list),
              LAVectorStoreIndexSearchCallback,
              (void *)(&index),
              &found,
              NULL,
              &foundNode,
              NULL,
              NULL
            );
  if(code) return code;

  if(!found) return LA_ERROR_OUTOFBOUND;
  
  if(data != NULL){
    (*data) = malloc(foundNode->length);
    if((*data) == NULL) return LA_ERROR_MALLOC;
    memcpy((*data), foundNode->data, foundNode->length);
    if(length != NULL) (*length) = foundNode->length;
  }
  
  if(foundNode != NULL) free(foundNode);

  return LA_NO_ERROR;  
}

LAErrorCode LAVectorStoreGetAt(const LAVectorStore *vector, const size_t index, void **data, size_t *length){
  LA_CHECK_NULLPTR(vector);
  LA_HANDLE_NULLPTR(data,   LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);
  
  LAErrorCode code = LA_NO_ERROR;
  bool found = false;
  LAVectorStoreNode *foundNode = NULL;

  code = LAVectorStoreIterator_backend(
              LA_LIST_STORE(list),
              LAVectorStoreIndexSearchCallback,
              (void *)(&index),
              &found,
              NULL,
              &foundNode,
              NULL,
              NULL
            );
  if(code) return code;

  if(!found) return LA_ERROR_OUTOFBOUND;

  (*data) = foundNode->data;
  (*length) = foundNode->length;
  
  return LA_NO_ERROR;  
}

LAErrorCode LAVectorStoreFind(const LAVectorStore *vector, LAVectorStoreCallback callback, void *data, bool *found, size_t *foundIndex, void **nodeData){
  LA_CHECK_NULLPTR(vector);
  LA_CHECK_NULLPTR(callback);
  LA_CHECK_NULLPTR(found);
  LA_CHECK_NULLPTR(foundIndex);

  LAErrorCode code = LA_NO_ERROR;
  LAVectorStoreNode *foundNode = NULL;
  
  code = LAVectorStoreIterator_backend(
              LA_LIST_STORE(list),
              callback,
              data,
              found,
              foundIndex,
              &foundNode,
              NULL,
              NULL
            );

  if((nodeData != NULL) && (foundNode != NULL)){
    (*nodeData) = (*found) ? foundNode->data : NULL;
  }

  return code;
}

LAErrorCode LAVectorStoreFindRemove(LAVectorStore *vector, LAVectorStoreCallback callback, void *data, bool *found){
  LA_CHECK_NULLPTR(vector);
  LA_CHECK_NULLPTR(callback);
  LA_CHECK_NULLPTR(found);

  LAErrorCode code = LA_NO_ERROR;
  LAVectorStoreNode *foundNode = NULL;
  
  code = LAVectorStoreIterator_backend(
              list,
              callback,
              data,
              found,
              NULL,
              &foundNode,
              NULL,
              NULL
            );
  
  if(code) return code;

  if((*found) && (foundNode != NULL)){
    code = LAVectorStoreRemoveBetweenNodes(foundNode);
    if(code) return code;
    if(foundNode != NULL) free(foundNode);
  }

  return code;
}

LAErrorCode LAVectorStoreFindRemoveAll(LAVectorStore *vector, LAVectorStoreCallback callback, void *data, size_t *matches){
  LA_CHECK_NULLPTR(vector);
  LA_CHECK_NULLPTR(callback);

  LAErrorCode code = LA_NO_ERROR;
  LAVectorStoreNode *foundNode = NULL;
  bool found = false;
  bool hasEnded = false;
  LAVectorStoreNode *nodeStart = NULL;

  if(matches != NULL) (*matches) = 0;
  
  do{
    // Iteration
    code = LAVectorStoreIterator_backend(
              list,
              callback,
              data,
              &found,
              NULL,
              &foundNode,
              nodeStart,
              &hasEnded
            );
    if(code) return code;

    // Handle remove
    if(found && (foundNode != NULL)){
      if(matches != NULL) (*matches) += 1;
      nodeStart = foundNode->next;
      code = LAVectorStoreRemoveBetweenNodes(foundNode);
      if(code) return code;
      if(foundNode != NULL) free(foundNode);
    }
  } while(!hasEnded);

  return code;
}
