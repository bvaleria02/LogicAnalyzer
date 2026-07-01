#ifndef LA_STRUCTS_LINKED_LIST_H
#define LA_STRUCTS_LINKED_LIST_H

#include "../liblogicanalyzer.h"
#include "model.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum {
  LA_LIST_SPECIAL_NORMAL = 0,
  LA_LIST_SPECIAL_START  = 1,
  LA_LIST_SPECIAL_END    = 2
} LAListNodeSpecial;

typedef struct _la_list_node{
  struct _la_list_node *prev;
  struct _la_list_node *next;
  LAListNodeSpecial special;
  uint8_t data[];               // abstract data
} LAListNode;

typedef struct {
  LAItemModel model;
  
  LAListNode *head; 
  LAListNode *tail; 
} LAListStore;

#define LA_LIST_STORE(x) ((LAListStore *)(x))

LAErrorCode LALinkedListInit(LAListStore *ls, ssize_t maxLength, bool isMutable, size_t nodeSize);
LAErrorCode LALinkedListDestroy(LAItemModel *model);
LAErrorCode LALinkedListAppendStart(LAItemModel *model, void *data);
LAErrorCode LALinkedListAppendEnd(LAItemModel *model, void *data);
LAErrorCode LALinkedListInsert(LAItemModel *model, size_t index, void *data);
LAErrorCode LALinkedListRemoveStart(LAItemModel *model);
LAErrorCode LALinkedListRemoveEnd(LAItemModel *model);
LAErrorCode LALinkedListRemove(LAItemModel *model, size_t index);
LAErrorCode LALinkedListIter(LAItemModel *model, LAItemModelIterCallback callback, ssize_t iterMax);

#endif //LA_STRUCTS_LINKED_LIST_H
