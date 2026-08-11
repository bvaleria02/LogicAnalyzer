#include "../liblogicanalyzer.h"
#include "model.h"
#include "linkstore.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

LAErrorCode LALinkedListCreateNode(LAListNode **node, LAListNodeSpecial special, size_t size, void *data){
  LA_HANDLE_NULLPTR(node, LA_PROPAGATE_ERROR);

  (*node) = (LAListNode *)malloc(sizeof(LAListNode) + size);
  if((*node) == NULL) return LA_ERROR_MALLOC;

  (*node)->special = special;
  (*node)->prev    = NULL;
  (*node)->next    = NULL;

  if(data != NULL){
    memcpy((*node)->data, data, size);
  }

  return LA_NO_ERROR;
}

LAErrorCode LALinkedListDestroyNode(LAListNode *node){
  LA_HANDLE_NULLPTR(node, LA_PROPAGATE_ERROR);

  if(node != NULL) free(node);
  
  return LA_NO_ERROR;
}

LAErrorCode LALinkedListInit(LAListStore *ls, ssize_t maxLength, bool isMutable, size_t nodeSize){
  LA_HANDLE_NULLPTR(ls, LA_PROPAGATE_ERROR);

  LAErrorCode code = LAItemModelInit(
                          LA_ITEM_MODEL(ls),
                          maxLength,
                          isMutable,
                          nodeSize
                      );
  if(code) return code;

  ls->model.vtable.destroy     = (LAItemModelCallbackDestroy)LALinkedListDestroy;
  ls->model.vtable.appendEnd   = (LAItemModelCallbackAppend)LALinkedListAppendEnd;
  ls->model.vtable.appendStart = (LAItemModelCallbackAppend)LALinkedListAppendStart;
  ls->model.vtable.insert      = (LAItemModelCallbackInsert)LALinkedListInsert;
  ls->model.vtable.removeEnd   = (LAItemModelCallbackRemove)LALinkedListRemoveEnd;
  ls->model.vtable.removeStart = (LAItemModelCallbackRemove)LALinkedListRemoveStart;
  ls->model.vtable.remove      = (LAItemModelCallbackRemoveIndex)LALinkedListRemove;
  ls->model.vtable.iter        = (LAItemModelCallbackIter)LALinkedListIter;

  code = LALinkedListCreateNode(&(ls->head), LA_LIST_SPECIAL_END,   ls->model.nodeSize, NULL);
  if(code) goto malloc_error;
  code = LALinkedListCreateNode(&(ls->tail), LA_LIST_SPECIAL_START, ls->model.nodeSize, NULL);
  if(code) goto malloc_error;

  ls->head->prev = ls->tail;
  ls->tail->next = ls->head;
  
  return LA_NO_ERROR;

malloc_error:
  code = LA_ERROR_MALLOC;
  goto cleanup;

cleanup:
  if(ls->tail != NULL){
    LALinkedListDestroyNode(ls->tail);
    ls->tail = NULL;
  }
  
  if(ls->head != NULL){
    LALinkedListDestroyNode(ls->head);
    ls->head = NULL;
  }
  
  return code;
}

LAErrorCode LALinkedListDestroy(LAItemModel *model){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);

  LAListStore *ls = LA_LIST_STORE(model);
  LAListNode *next = NULL;
  LAListNode *actual = ls->tail;

  while(actual != NULL){
    next = actual->next;
    
    LALinkedListDestroyNode(actual);
      
    actual = next;
  }
  
  return LA_NO_ERROR;
}

LAErrorCode LALinkedListInsertNodeBetween(LAListNode *left, LAListNode *right, LAListNode *mid){
  LA_HANDLE_NULLPTR(left,  LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(right, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(mid,   LA_PROPAGATE_ERROR);

  left->next  = mid;
  mid->prev   = left;
  mid->next   = right;
  right->prev = mid;
  
  return LA_NO_ERROR;
}

LAErrorCode LALinkedListAppendStart(LAItemModel *model, void *data){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data,  LA_PROPAGATE_ERROR);

  if((model->maxLength != -1) && ((size_t)model->length >= (size_t)model->maxLength)){
    return LA_ERROR_MAX_LENGTH;
  }
  
  LAListStore *ls = LA_LIST_STORE(model);
  LAErrorCode code = LA_NO_ERROR;
  
  LAListNode *node = NULL;
  code = LALinkedListCreateNode(&node, LA_LIST_SPECIAL_NORMAL, ls->model.nodeSize, data);
  if(code) return code;

  LAListNode *left  = ls->tail;
  LAListNode *right = ls->tail->next;

  code = LALinkedListInsertNodeBetween(left, right, node);
  if(code) goto handle_error;
    
  ls->model.length += 1;
  return LA_NO_ERROR;  

handle_error:
  if(node != NULL) LALinkedListDestroyNode(node);
  return code;
}

LAErrorCode LALinkedListAppendEnd(LAItemModel *model, void *data){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(data,  LA_PROPAGATE_ERROR);

  if((model->maxLength != -1) && ((size_t)model->length >= (size_t)model->maxLength)){
    return LA_ERROR_MAX_LENGTH;
  }
  
  LAListStore *ls = LA_LIST_STORE(model);
  LAErrorCode code = LA_NO_ERROR;
  
  LAListNode *node = NULL;
  code = LALinkedListCreateNode(&node, LA_LIST_SPECIAL_NORMAL, ls->model.nodeSize, data);
  if(code) return code;

  LAListNode *right = ls->head;
  LAListNode *left  = ls->head->prev;

  code = LALinkedListInsertNodeBetween(left, right, node);
  if(code) goto handle_error;

  ls->model.length += 1;
  return LA_NO_ERROR;  

handle_error:
  if(node != NULL) LALinkedListDestroyNode(node);
  return code;
}

LAErrorCode LALinkedListInsert(LAItemModel *model, size_t index, void *data){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);

  if((model->maxLength != -1) && ((size_t)model->length >= (size_t)model->maxLength)){
    return LA_ERROR_MAX_LENGTH;
  }
  
  LAListStore *ls = LA_LIST_STORE(model);
  LAListNode *node    = ls->tail;
  LAListNode *newNode = NULL;
  size_t counter = 0;
  LAErrorCode code = LA_NO_ERROR;
  bool hasReached = false;

  while(node != NULL){
    // Don't count the sentinel start
    if(node->special == LA_LIST_SPECIAL_START){
      node = node->next;
      continue;
    }

    // Handle sentinel end
    if(node->special == LA_LIST_SPECIAL_END){
      break;
    }

    // Handle non matching index
    if(counter != index){
      node = node->next;
      counter++;
      continue;
    }
    
    // Actual logic
    code = LALinkedListCreateNode(&newNode, LA_LIST_SPECIAL_NORMAL, ls->model.nodeSize, data);
    if(code) return code;

    LAListNode *right = node;
    LAListNode *left  = node->prev;

    code = LALinkedListInsertNodeBetween(left, right, node);
    if(code) goto handle_error;
    hasReached = true;
    model->length += 1;
    break;
  }
  
  return (hasReached) ? LA_NO_ERROR : LA_ERROR_OUTOFBOUND;
  
handle_error:
  if(node != NULL) LALinkedListDestroyNode(node);
  return code;
}

LAErrorCode LALinkedListRemoveNodeBetween(LAListNode *left, LAListNode *right, LAListNode *mid){
  LA_HANDLE_NULLPTR(left,  LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(right, LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(mid,   LA_PROPAGATE_ERROR);

  left->next  = right;
  right->prev = left;

  if(mid != NULL) free(mid);
  
  return LA_NO_ERROR;
}

LAErrorCode LALinkedListRemoveStart(LAItemModel *model){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);
 
  LAListStore *ls = LA_LIST_STORE(model);
  LAErrorCode code = LA_NO_ERROR;
  
  LAListNode *left  = ls->tail;
  LAListNode *mid   = ls->tail->next;
  LAListNode *right = NULL;

  if(mid->special == LA_LIST_SPECIAL_END){
    return LA_NO_ERROR;
  } else {
    right = mid->next;
  }

  code = LALinkedListRemoveNodeBetween(left, right, mid);
  if(code) return code;
  
  model->length -= 1;
  return code;
}

LAErrorCode LALinkedListRemoveEnd(LAItemModel *model){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);
 
  LAListStore *ls = LA_LIST_STORE(model);
  LAErrorCode code = LA_NO_ERROR;
  
  LAListNode *right  = ls->head;
  LAListNode *mid   = ls->head->prev;
  LAListNode *left = NULL;

  if(mid->special == LA_LIST_SPECIAL_START){
    return LA_NO_ERROR;
  } else {
    left = mid->prev;
  }

  code = LALinkedListRemoveNodeBetween(left, right, mid);
  if(code) return code;
  
  model->length -= 1;
  return code;
}

LAErrorCode LALinkedListRemove(LAItemModel *model, size_t index){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);
 
  LAListStore *ls = LA_LIST_STORE(model);
  LAErrorCode code = LA_NO_ERROR;

  LAListNode *node    = ls->tail;
  size_t counter = 0;
  bool hasReached = false;

  while(node != NULL){
    // Don't count the sentinel start
    if(node->special == LA_LIST_SPECIAL_START){
      node = node->next;
      continue;
    }

    // Handle sentinel end
    if(node->special == LA_LIST_SPECIAL_END){
      break;
    }

    // Handle non matching index
    if(counter != index){
      node = node->next;
      counter++;
      continue;
    }
    
    // Actual logic
    LAListNode *left  = node->prev;
    LAListNode *right = node->next;

    code = LALinkedListRemoveNodeBetween(left, right, node);
    if(code) return code;
    model->length -= 1;
    break;  
  }

  
  return (hasReached) ? LA_NO_ERROR : LA_ERROR_OUTOFBOUND;
}

LAErrorCode LALinkedListIter(LAItemModel *model, LAItemModelIterCallback callback, ssize_t iterMax){
  LA_HANDLE_NULLPTR(model,    LA_PROPAGATE_ERROR);
  LA_HANDLE_NULLPTR(callback, LA_PROPAGATE_ERROR);
  
  LAListStore *ls = LA_LIST_STORE(model);
  LAErrorCode code = LA_NO_ERROR;
  LAListNode *node = ls->tail;
  LAListNode *next = NULL;
  size_t i = 0;

  while(node != NULL){
    if(node->special == LA_LIST_SPECIAL_START){
      node = node->next;
      continue;
    }

    if(node->special == LA_LIST_SPECIAL_END){
      break;
    }

    if((iterMax != -1) && ((size_t)i >= (size_t)iterMax)) break;

    // Ensures you can free the node inside safely
    next = node->next;
    
    code = callback(model, (void *) node, i);
    if(code) return code;
    
    node = next;
    i++;
  }
  
  return code;
}
