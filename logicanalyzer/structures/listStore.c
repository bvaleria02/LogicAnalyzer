#include "../liblogicanalyzer.h"
#include "../utils.h"
#include "model.h"
#include "itemListable.h"
#include "listStore.h"
#include "error.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const LAListStoreVTable LAListStoreVTableBase = {
  .listableVTable = {
    .modelVTable = {
      .destroy         = (LAItemModelFnDestroy) LAListStoreDestroy,
      .increaseCount   = (LAItemModelFnIncreaseCount) LAListStoreIncreaseCount,
      .decreaseCount   = (LAItemModelFnDecreaseCount) LAListStoreDecreaseCount,
      .resetCount      = (LAItemModelFnResetCount) LAListStoreResetCount,
      .setNodeLength   = (LAItemModelFnSetNodeLength) LAListStoreSetNodeLength,
      .getNodeLength   = (LAItemModelFnGetNodeLength) LAListStoreGetNodeLength,
      .isInfinite      = (LAItemModelFnIsInfinite) LAListStoreIsInfinite,
      .isFinite        = (LAItemModelFnIsFinite) LAListStoreIsFinite,
      .isMutable       = (LAItemModelFnIsMutable) LAListStoreIsMutable,
      .getNodeCount    = (LAItemModelFnGetNodeCount) LAListStoreGetNodeCount,
      .setMaxNodeCount = (LAItemModelFnSetMaxNodeCount) LAListStoreSetMaxNodeCount,
      .getMaxNodeCount = (LAItemModelFnGetMaxNodeCount) LAListStoreGetMaxNodeCount,
      .iter            = (LAItemModelFnIter) LAListStoreIter,
      .insert          = (LAItemModelFnInsert) LAListStoreInsert,
      .remove          = (LAItemModelFnRemove) LAListStoreRemove,
      .get             = (LAItemModelFnGet) LAListStoreGet,
    },
    .insertAt        = (LAItemListableFnInsertAt) LAListStoreInsertAt,
    .removeAt        = (LAItemListableFnRemoveAt) LAListStoreRemoveAt,
    .getAt           = (LAItemListableFnGetAt) LAListStoreGetAt,
    .find            = (LAItemListableFnFind) LAListStoreFind,
    .findRemove      = (LAItemListableFnFindRemove) LAListStoreFindRemove,
    .findRemoveAll   = (LAItemListableFnFindRemoveAll) LAListStoreFindRemoveAll,
  }
};

LAErrorCode LAListStoreCreateNode(LAListStoreNode **node, const size_t nodeLength, const size_t dataLength, const void *data, const LAListStoreSentinel sentinel, const bool useNodeLength){
  LA_HANDLE_NULLPTR(node, LA_PROPAGATE_ERROR);
  // data can be null
  LAErrorCode code = LA_NO_ERROR;
  bool isNodeAllocated = false;

  size_t dataBufferLength = (useNodeLength) ? nodeLength : dataLength;  
  if(data     == NULL)                        dataBufferLength = 0;
  if(sentinel != LA_LIST_STORE_SENTINEL_NONE) dataBufferLength = 0;

  size_t totalLength = sizeof(LAListStoreNode) + dataBufferLength;
  (*node) = (LAListStoreNode *)malloc(totalLength);
  if((*node) == NULL) goto malloc_error;
  isNodeAllocated = true;

  (*node)->prev     = NULL;
  (*node)->next     = NULL;
  (*node)->sentinel = sentinel;
  (*node)->length   = dataBufferLength;

  if(dataBufferLength != 0){
    memset((*node)->data, 0x00, dataBufferLength);
    
    size_t copyLength = dataLength;
    if(useNodeLength && (dataLength > nodeLength)){
      copyLength = nodeLength;
    }
    
    memcpy((*node)->data, data, copyLength);
  }

  return LA_NO_ERROR;

malloc_error:
  code = LA_ERROR_MALLOC;
  goto cleanup;

cleanup:
  if(isNodeAllocated && ((*node) != NULL)) free((*node));
  (*node) = NULL;
  return code;
}

LAErrorCode LAListStoreInit(LAListStore *list, size_t maxNodeCount, bool isInfinite, size_t defaultNodeSize, bool useDefaultSize){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);
  
  LAErrorCode code = LA_NO_ERROR;
  code = LAItemModelInit(LA_ITEM_MODEL(list), maxNodeCount, isInfinite, defaultNodeSize, useDefaultSize);
  if(code) goto cleanup;
  
  list->listable.model.vtable = (LAItemModelVTable *) &(LAListStoreVTableBase);

  LAListStoreNode *head = NULL;
  LAListStoreNode *tail = NULL;

  code = LAListStoreCreateNode(&head, defaultNodeSize, 0, NULL, LA_LIST_STORE_SENTINEL_HEAD, false);
  if(code) goto cleanup;
  code = LAListStoreCreateNode(&tail, defaultNodeSize, 0, NULL, LA_LIST_STORE_SENTINEL_TAIL, false);
  if(code) goto cleanup;

  head->prev = NULL;
  head->next = tail;
  tail->prev = head;
  tail->next = NULL;

  list->tail = tail;
  list->head = head;
  
  return LA_NO_ERROR;

cleanup:
  if(head != NULL) free(head);
  if(tail != NULL) free(tail);
  list->head = NULL;
  list->tail = NULL;
  LAItemModel *model = LA_ITEM_MODEL(list);
  model->vtable->destroy(&model);
  return code;
}

LAErrorCode LAListStoreDestroy(LAListStore **list){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);

  LAListStoreNode *node = (*list)->head;
  LAListStoreNode *next = NULL;
  while(node != NULL){
    next = node->next;
    free(node);
    node = next;
  }

  LAErrorCode code = LA_NO_ERROR;
  LAItemListable *listable = LA_ITEM_LISTABLE(*list);
  code = LAItemListableDestroy(&listable);
  
  return code;  
}

LAErrorCode LAListStoreIncreaseCount(LAListStore *list, const size_t amount){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableIncreaseCount(LA_ITEM_LISTABLE(list), amount);
  
  return code;  
}

LAErrorCode LAListStoreDecreaseCount(LAListStore *list, const size_t amount){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableDecreaseCount(LA_ITEM_LISTABLE(list), amount);
  
  return code;  
}

LAErrorCode LAListStoreResetCount(LAListStore *list){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableResetCount(LA_ITEM_LISTABLE(list));
  
  return code;  
}

LAErrorCode LAListStoreSetNodeLength(LAListStore *list, const size_t length){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableSetNodeLength(LA_ITEM_LISTABLE(list), length);
  
  return code;  
}

LAErrorCode LAListStoreGetNodeLength(const LAListStore *list, size_t *length){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableGetNodeLength(LA_ITEM_LISTABLE(list), length);
  
  return code;  
}

LAErrorCode LAListStoreIsInfinite(const LAListStore *list, bool *isInfinite){
  LA_HANDLE_NULLPTR(list,       LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(isInfinite, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableIsInfinite(LA_ITEM_LISTABLE(list), isInfinite);
  
  return code;  
}

LAErrorCode LAListStoreIsFinite(const LAListStore *list, bool *isFinite){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableIsFinite(LA_ITEM_LISTABLE(list), isFinite);
  
  return code;  
}

LAErrorCode LAListStoreIsMutable(const LAListStore *list, bool *isMutable){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableIsMutable(LA_ITEM_LISTABLE(list), isMutable);
  
  return code;  
}

LAErrorCode LAListStoreGetNodeCount(const LAListStore *list, size_t *count){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(count, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableGetNodeCount(LA_ITEM_LISTABLE(list), count);
  
  return code;  
}

LAErrorCode LAListStoreSetMaxNodeCount(LAListStore *list, size_t count){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableSetMaxNodeCount(LA_ITEM_LISTABLE(list), count);
  
  return code;  
}

LAErrorCode LAListStoreGetMaxNodeCount(const LAListStore *list, size_t *count){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(count, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  code = LAItemListableGetMaxNodeCount(LA_ITEM_LISTABLE(list), count);
  
  return code;  
}

LAErrorCode LAListStoreIterator_backend(LAListStore *list, LAListStoreCallback callback, void *data, bool *found, size_t *foundIndex, LAListStoreNode **foundNode, LAListStoreNode *nodeStart, bool *hasFinished){
  LA_CHECK_NULLPTR(list);
  LA_CHECK_NULLPTR(callback);

  LAErrorCode code = LA_NO_ERROR;
  
  // Initializes for search
  if(found != NULL)       (*found)     = false;
  if(foundNode != NULL)   (*foundNode) = NULL;
  if(hasFinished != NULL) (*hasFinished) = false;
  
  bool stopIter = false;
  
  LAListStoreNode *node = (nodeStart != NULL) ? nodeStart : list->head;
  size_t listIndex = 0;
  
  while(node != NULL){
    // Skip head node
    if(node->sentinel == LA_LIST_STORE_SENTINEL_HEAD){
      node = node->next;
      continue;
    }

    // Stop if tail node is reached
    if((node->sentinel == LA_LIST_STORE_SENTINEL_TAIL) || (node->next == NULL)){
      if(hasFinished != NULL) (*hasFinished) = true;
      break;
    }

    // Actual iteration in non-sentinel nodes
    code = callback(LA_LIST_STORE(list), node, listIndex, data, &stopIter);
    if(code) break;

    // Continues if stopIter is false
    if(!stopIter){
      node = node->next;
      listIndex++;
      continue;
    }

    // stopIter is true, handle match, and break;
    if(found != NULL)     (*found)      = true;
    if(foundIndex != NULL)(*foundIndex) = listIndex;
    if(foundNode != NULL) (*foundNode)  = node;
    break;
  }
  
  return code;
}

LAErrorCode LAListStoreIter(LAListStore *list, LAListStoreCallback callback, void *data){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(callback, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  
  code = LAListStoreIterator_backend(
              list,
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

LAErrorCode LAListStoreInsertBetweenNodes(LAListStoreNode *left, LAListStoreNode *right, LAListStoreNode *mid){
  LA_CHECK_NULLPTR(left);
  LA_CHECK_NULLPTR(right);
  LA_CHECK_NULLPTR(mid);

  left->next = mid;
  right->prev = mid;
  mid->prev = left;
  mid->next = right;

  return LA_NO_ERROR;
}

LAErrorCode LAListStoreInsert(LAListStore *list, void *data, const size_t length){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data,  LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;

  LAListStoreNode *newNode = NULL;

  code = LAListStoreCreateNode(
            &newNode,
            list->listable.model.defaultNodeLength,
            length,
            data,
            LA_LIST_STORE_SENTINEL_NONE,
            list->listable.model.useDefaultSize
        );
  if(code) goto cleanup;

  code = LAListStoreInsertBetweenNodes(list->tail->prev, list->tail, newNode);
  if(code) goto cleanup;
  
  return LA_NO_ERROR;
  
cleanup:
  if(newNode != NULL) free(newNode);
  return code;
}

LAErrorCode LAListStoreRemoveBetweenNodes(LAListStoreNode *node){
  LA_CHECK_NULLPTR(node);

  if(node->sentinel != LA_LIST_STORE_SENTINEL_NONE) return LA_NO_ERROR;
  if(node->prev == NULL) return LA_ERROR_NULLPTR;
  if(node->next == NULL) return LA_ERROR_NULLPTR;

  (node->prev)->next = node->next;
  (node->next)->prev = node->prev;

  return LA_NO_ERROR;
}

LAErrorCode LAListStoreRemove(LAListStore *list, void **data, size_t *length){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);

  if(data != NULL) (*data) = NULL;

  LAErrorCode code = LA_NO_ERROR;
  
  LAListStoreNode *node = (list->tail)->prev;

  if((node != NULL) && (node->sentinel == LA_LIST_STORE_SENTINEL_NONE) && (data != NULL)){
    (*data) = malloc(node->length);
    if((*data) == NULL) return LA_ERROR_MALLOC;
    memcpy((*data), node->data, node->length);
    if(length != NULL) (*length) = node->length;
  }

  code = LAListStoreRemoveBetweenNodes(node);
  if(code) return code;

  if(node != NULL) free(node);
  
  (void) data;
  (void) length;
  return code;  
}

LAErrorCode LAListStoreGet(const LAListStore *list, void **data, size_t *length){
  LA_HANDLE_NULLPTR(list,   LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data,   LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;

  LAListStoreNode *node = (list->tail)->prev;

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

LAErrorCode LAListStoreIndexSearchCallback(LAListStore *list, LAListStoreNode *node, size_t index, void *data, bool *stopIter){
  size_t targetIndex = *(size_t *)data;

  if(targetIndex == index){
    (*stopIter) = true;
  }

  (void) list;
  (void) node;
  return LA_NO_ERROR;
}

LAErrorCode LAListStoreInsertAt(LAListStore *list, const size_t index, void *data, size_t length){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data,  LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;

  bool found = false;
  LAListStoreNode *foundNode = NULL;

  code = LAListStoreIterator_backend(
              LA_LIST_STORE(list),
              LAListStoreIndexSearchCallback,
              (void *)(&index),
              &found,
              NULL,
              &foundNode,
              NULL,
              NULL
            );

  if(!found) return LA_ERROR_OUTOFBOUND;

  LAListStoreNode *newNode = NULL;
  code = LAListStoreCreateNode(
            &newNode,
            list->listable.model.defaultNodeLength,
            length,
            data,
            LA_LIST_STORE_SENTINEL_NONE,
            list->listable.model.useDefaultSize
        );
  if(code) goto cleanup;

  code = LAListStoreInsertBetweenNodes(foundNode->prev, foundNode, newNode);
  if(code) goto cleanup;
  
  return LA_NO_ERROR;
  
cleanup:
  if(newNode != NULL) free(newNode);
  return code;
}

LAErrorCode LAListStoreRemoveAt(LAListStore *list, const size_t index, void **data, size_t *length){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);
  
  LAErrorCode code = LA_NO_ERROR;
  bool found = false;
  LAListStoreNode *foundNode = NULL;
  
  if(data != NULL){
    (*data) = NULL;
  }

  code = LAListStoreIterator_backend(
              LA_LIST_STORE(list),
              LAListStoreIndexSearchCallback,
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
    (*data) = NULL;
    (*data) = malloc(foundNode->length);
    if((*data) == NULL) return LA_ERROR_MALLOC;
    memcpy((*data), foundNode->data, foundNode->length);
    if(length != NULL) (*length) = foundNode->length;
  }
  
  code = LAListStoreRemoveBetweenNodes(foundNode);
  if(code) return code;
  
  free(foundNode);

  return LA_NO_ERROR;  
}

LAErrorCode LAListStoreGetAt(const LAListStore *list, const size_t index, void **data, size_t *length){
  LA_HANDLE_NULLPTR(list,   LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data,   LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);
  
  LAErrorCode code = LA_NO_ERROR;
  bool found = false;
  LAListStoreNode *foundNode = NULL;

  code = LAListStoreIterator_backend(
              LA_LIST_STORE(list),
              LAListStoreIndexSearchCallback,
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

LAErrorCode LAListStoreFind(const LAListStore *list, LAListStoreCallback callback, void *data, bool *found, size_t *foundIndex, void **nodeData){
  LA_CHECK_NULLPTR(list);
  LA_CHECK_NULLPTR(callback);
  LA_CHECK_NULLPTR(found);
  LA_CHECK_NULLPTR(foundIndex);

  LAErrorCode code = LA_NO_ERROR;
  LAListStoreNode *foundNode = NULL;
  
  code = LAListStoreIterator_backend(
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

LAErrorCode LAListStoreFindRemove(LAListStore *list, LAListStoreCallback callback, void *data, bool *found){
  LA_CHECK_NULLPTR(list);
  LA_CHECK_NULLPTR(callback);
  LA_CHECK_NULLPTR(found);

  LAErrorCode code = LA_NO_ERROR;
  LAListStoreNode *foundNode = NULL;
  
  code = LAListStoreIterator_backend(
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
    code = LAListStoreRemoveBetweenNodes(foundNode);
    if(code) return code;
    if(foundNode != NULL) free(foundNode);
  }

  return code;
}

LAErrorCode LAListStoreFindRemoveAll(LAListStore *list, LAListStoreCallback callback, void *data, size_t *matches){
  LA_CHECK_NULLPTR(list);
  LA_CHECK_NULLPTR(callback);

  LAErrorCode code = LA_NO_ERROR;
  LAListStoreNode *foundNode = NULL;
  bool found = false;
  bool hasEnded = false;
  LAListStoreNode *nodeStart = NULL;

  if(matches != NULL) (*matches) = 0;
  
  do{
    // Iteration
    code = LAListStoreIterator_backend(
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
      code = LAListStoreRemoveBetweenNodes(foundNode);
      if(code) return code;
      if(foundNode != NULL) free(foundNode);
    }
  } while(!hasEnded);

  return code;
}

