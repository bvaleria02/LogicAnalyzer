#include "../liblogicanalyzer.h"
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
      .get             = (LAItemModelFnGet) LAListStoreGet
    },
    .insertAt = (LAItemListableFnInsertAt) LAListStoreInsertAt,
    .removeAt = (LAItemListableFnRemoveAt) LAListStoreRemoveAt,
    .getAt    = (LAItemListableFnGetAt) LAListStoreGetAt
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

LAErrorCode LAListStoreIter(LAListStore *list, LAListStoreCallback callback, void *data){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(callback, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  LAListStoreNode *node = list->head;
  size_t i = 0;
  while(node != NULL){
    if(node->sentinel == LA_LIST_STORE_SENTINEL_HEAD){
      node = node->next;
      continue;
    }
    
    if(node->sentinel == LA_LIST_STORE_SENTINEL_TAIL){
      break;
    }
    
    code = callback(list, node, i, data);
    node = node->next;
    i++;
  }
  
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

LAErrorCode LAListStoreRemove(LAListStore *list){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  
  LAListStoreNode *node = (list->tail)->prev;

  code = LAListStoreRemoveBetweenNodes(node);
  if(code) return code;

  if(node != NULL) free(node);
  
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

LAErrorCode LAListStoreInsertAt(LAListStore *list, const size_t index, void *data, size_t length){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data,  LA_PROPAGATE_ERROR);

  (void) list;
  (void) index;
  (void) data;
  (void) length;
  return LALogStructureErrorBase((void *)list, "listStore", "insertAt");
}

LAErrorCode LAListStoreRemoveAt(LAListStore *list, const size_t index){
  LA_HANDLE_NULLPTR(list, LA_PROPAGATE_ERROR);
  
  (void) list;
  (void) index;
  return LALogStructureErrorBase((void *)list, "listStore", "removeAt");
}

LAErrorCode LAListStoreGetAt(const LAListStore *list, const size_t index, void **data, size_t *length){
  LA_HANDLE_NULLPTR(list,   LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data,   LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(length, LA_PROPAGATE_ERROR);
  
  (void) list;
  (void) index;
  (void) data;
  (void) length;
  return LALogStructureErrorBase((void *)list, "listStore", "getAt");
}
