#ifndef LA_STRUCTS_MODEL_H
#define LA_STRUCTS_MODEL_H

#include "../liblogicanalyzer.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct _la_item_model LAItemModel;

typedef LAErrorCode (*LAItemModelIterCallback)(LAItemModel *, void *, size_t);

typedef LAErrorCode (*LAItemModelCallbackDestroy)(LAItemModel *);
typedef LAErrorCode (*LAItemModelCallbackAppend)(LAItemModel *, void *);
typedef LAErrorCode (*LAItemModelCallbackInsert)(LAItemModel *, size_t, void *);
typedef LAErrorCode (*LAItemModelCallbackRemove)(LAItemModel *);
typedef LAErrorCode (*LAItemModelCallbackRemoveIndex)(LAItemModel *, size_t);
typedef LAErrorCode (*LAItemModelCallbackIter)(LAItemModel *, LAItemModelIterCallback, ssize_t);

typedef struct {
  LAItemModelCallbackDestroy     destroy;
  LAItemModelCallbackAppend      appendEnd;
  LAItemModelCallbackAppend      appendStart;
  LAItemModelCallbackInsert      insert;
  LAItemModelCallbackRemove      removeEnd;
  LAItemModelCallbackRemove      removeStart;
  LAItemModelCallbackRemoveIndex remove;
  LAItemModelCallbackIter        iter;
} LAItemModelVTable;

struct _la_item_model {
  LAItemModelVTable vtable;
  
  size_t length;             // Node count
  ssize_t maxLength;         // Max node count (-1 means infinite)
  size_t nodeSize;           // Size (in bytes) of the nodes;
  bool isMutable;            // Can you edit it?
};

#define LA_ITEM_MODEL(x) ((LAItemModel *)(x))

LAErrorCode LAItemModelInit(LAItemModel *ls, ssize_t maxLength, bool isMutable, size_t nodeSize);
LAErrorCode LAItemModelDestroy(LAItemModel *ls);
LAErrorCode LAItemModelAppendStart(LAItemModel *ls, void *data);
LAErrorCode LAItemModelAppendEnd(LAItemModel *ls, void *data);
LAErrorCode LAItemModelInsert(LAItemModel *ls, size_t index, void *data);
LAErrorCode LAItemModelRemoveStart(LAItemModel *ls);
LAErrorCode LAItemModelRemoveEnd(LAItemModel *ls);
LAErrorCode LAItemModelRemove(LAItemModel *ls, size_t index);
LAErrorCode LAItemModelIter(LAItemModel *ls, LAItemModelIterCallback callback, ssize_t iterMax);

#endif //LA_STRUCTS_MODEL_H
