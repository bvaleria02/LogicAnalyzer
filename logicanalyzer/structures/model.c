#include "../liblogicanalyzer.h"
#include "model.h"
#include "linkstore.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

LAErrorCode LAItemModelErrorBase(LAItemModel *model, const char *name){
  printf("[Error] VTable entry for (%p) takes to the base case of \"%s\"\n", model, name);
  return LA_ERROR_VTABLE_BASE;
}

LAErrorCode LAItemModelInit(LAItemModel *model, ssize_t maxLength, bool isMutable, size_t nodeSize){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);

  model->length    = 0;
  model->maxLength = maxLength;
  model->nodeSize  = nodeSize;
  model->isMutable = isMutable;

  model->vtable.destroy     = LAItemModelDestroy;
  model->vtable.appendEnd   = LAItemModelAppendEnd;
  model->vtable.appendStart = LAItemModelAppendStart;
  model->vtable.insert      = LAItemModelInsert;
  model->vtable.removeEnd   = LAItemModelRemoveEnd;
  model->vtable.removeStart = LAItemModelRemoveStart;
  model->vtable.remove      = LAItemModelRemove;
  model->vtable.iter        = LAItemModelIter;
    
  return LA_NO_ERROR;
}

LAErrorCode LAItemModelDestroy(LAItemModel *model){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  
  if(model->vtable.destroy == LAItemModelDestroy){
    code = LAItemModelErrorBase(model, "destroy");
  } else {
    code = model->vtable.destroy(model);
  }
  
  return code;
}

LAErrorCode LAItemModelAppendStart(LAItemModel *model, void *data){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  
  if(model->vtable.appendStart == LAItemModelAppendStart){
    code = LAItemModelErrorBase(model, "appendStart");
  } else {
    code = model->vtable.appendStart(model, data);
  }
  
  return code;
}

LAErrorCode LAItemModelAppendEnd(LAItemModel *model, void *data){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  
  if(model->vtable.appendEnd == LAItemModelAppendEnd){
    code = LAItemModelErrorBase(model, "appendEnd");
  } else {
    code = model->vtable.appendEnd(model, data); 
  }
  
  return code;
}

LAErrorCode LAItemModelInsert(LAItemModel *model, size_t index, void *data){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  
  if(model->vtable.insert == LAItemModelInsert){
    code = LAItemModelErrorBase(model, "insert");
  } else {
    code = model->vtable.insert(model, index, data);
  }
  
  return code;
}

LAErrorCode LAItemModelRemoveStart(LAItemModel *model){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  
  if(model->vtable.removeStart == LAItemModelRemoveStart){
    code = LAItemModelErrorBase(model, "removeStart");
  } else {
    code = model->vtable.removeStart(model);
  }
  
  return code;
}

LAErrorCode LAItemModelRemoveEnd(LAItemModel *model){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  
  if(model->vtable.removeEnd == LAItemModelRemoveEnd){
    code = LAItemModelErrorBase(model, "removeEnd");
  } else {
    code = model->vtable.removeEnd(model); 
  }
  
  return code;
}

LAErrorCode LAItemModelRemove(LAItemModel *model, size_t index){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  
  if(model->vtable.remove == LAItemModelRemove){
    code = LAItemModelErrorBase(model, "remove");
  } else {
    code = model->vtable.remove(model, index);
  }
  
  return code;
}

LAErrorCode LAItemModelIter(LAItemModel *model, LAItemModelIterCallback callback, ssize_t iterMax){
  LA_HANDLE_NULLPTR(model, LA_PROPAGATE_ERROR);

  LAErrorCode code = LA_NO_ERROR;
  
  if(model->vtable.iter == LAItemModelIter){
    code = LAItemModelErrorBase(model, "iter");
  } else {
    code = model->vtable.iter(model, callback, iterMax);
  }
  
  return code;
}
