#include "../liblogicanalyzer.h"
#include "shaders.h"
#include <stdint.h>
#include <stdlib.h>
#include "../glad/glad/glad.h"

#define LA_SHADER_TEXTURE_MODES 3
#define LA_SHADER_TEXTURE_DATA_TYPES 2
#define LA_SHADER_TEXTURE_TEX_TYPES 3

size_t LAShaderTextureDataTypeLength[LA_SHADER_TEXTURE_DATA_TYPES] = {
	sizeof(uint8_t), sizeof(float)
};

size_t LAShaderTextureTypeLength[LA_SHADER_TEXTURE_TEX_TYPES] = {
	3, 4, 1
};

LAErrorCode LAShaderTextureInit(LAShaderTexture **lst, LAShaderTextureMode mode, LAShaderTextureDataType dType, LAShaderTextureType tType, size_t width, size_t height){
	LA_HANDLE_NULLPTR(lst, LA_PROPAGATE_ERROR);
	
	if(width == 0 || height == 0) 				return LA_ERROR_ZEROLENGTH;
	if(mode  >= LA_SHADER_TEXTURE_MODES) 		return LA_ERROR_INVALIDVALUE;
	if(dType >= LA_SHADER_TEXTURE_DATA_TYPES) 	return LA_ERROR_INVALIDVALUE;
	if(tType >= LA_SHADER_TEXTURE_TEX_TYPES) 	return LA_ERROR_INVALIDVALUE;

	LAErrorCode code = LA_NO_ERROR;
	bool isStructAllocated 		= false;
	bool isStructDataAllocated 	= false;
	bool isStructData2Allocated = false;
	bool isStructData3Allocated = false;
	size_t size = width * height;
	size_t dataLength = LAShaderTextureDataTypeLength[dType];
	size_t typeLength = LAShaderTextureTypeLength[tType];
	size_t totalLength = size * dataLength * typeLength;

	(*lst) = (LAShaderTexture *)malloc(sizeof(LAShaderTexture) * 1);
	if((*lst) == NULL) goto malloc_error;
	isStructAllocated = true;

	(*lst)->data 	= (void *)malloc(totalLength);
	if((*lst)->data == NULL) goto malloc_error;
	isStructDataAllocated = true;

	if(mode == LA_SHADER_TEXTURE_CUBEMAP){
		(*lst)->data2 	= (void *)malloc(totalLength);
		if((*lst)->data2 == NULL) goto malloc_error;
		isStructData2Allocated = true;
		(*lst)->data3 	= (void *)malloc(totalLength);
		if((*lst)->data3 == NULL) goto malloc_error;
		isStructData3Allocated = true;

		for(size_t i = 0; i < (width * height); i++){
			((char *)(*lst)->data2)[i] = 0x7f;
		}
	} else {
		(*lst)->data2 = NULL;
		(*lst)->data3 = NULL;
	}

	(*lst)->tMode 	= mode;
	(*lst)->dType 	= dType;
	(*lst)->tType 	= tType;
	(*lst)->id	   	= -1U;
	(*lst)->width 	= width;
	(*lst)->height	= height;
	(*lst)->dirty	= false;
	(*lst)->next	= NULL;

	return LA_NO_ERROR;

malloc_error:
	code = LA_ERROR_MALLOC;
	goto handle_error;

handle_error:

	if(isStructDataAllocated){
		if((*lst)->data != NULL) free((*lst)->data);
	}
	if(isStructData2Allocated){
		if((*lst)->data2 != NULL) free((*lst)->data2);
	}
	if(isStructData3Allocated){
		if((*lst)->data3 != NULL) free((*lst)->data3);
	}

	if(isStructAllocated){
		if((*lst) != NULL) free((*lst));
	}
	return code;
}

LAErrorCode LAShaderTextureValueCopy(LAShaderTexture *lst, size_t index, size_t x0, size_t y0, size_t width, size_t height, size_t valueWidth, size_t valueHeight, void *value){
	LA_HANDLE_NULLPTR(lst, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(lst->data, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value, 		LA_PROPAGATE_ERROR);

	if(width  == 0) return LA_ERROR_ZEROLENGTH;
	if(height == 0) return LA_ERROR_ZEROLENGTH;

	size_t endX = x0 + width;
	size_t endY = y0 + height;
	size_t dataLength = LAShaderTextureDataTypeLength[lst->dType];
	size_t typeLength = LAShaderTextureTypeLength[lst->tType];

	if(endX > lst->width)		return LA_ERROR_OUTOFBOUND;
	if(endY > lst->height)		return LA_ERROR_OUTOFBOUND;
	if(x0   >= lst->width)		return LA_ERROR_OUTOFBOUND;
	if(y0   >= lst->height)		return LA_ERROR_OUTOFBOUND;
	if(width > valueWidth) 		return LA_ERROR_OUTOFBOUND;
	if(height > valueHeight) 	return LA_ERROR_OUTOFBOUND;
	if(x0   >= endX)			return LA_ERROR_INVALIDVALUE;
	if(y0   >= endY)			return LA_ERROR_INVALIDVALUE;

	switch(index){
		case 0:	memcpy(lst->data, value, (width * height) * dataLength * typeLength);
				break;
		case 1:	memcpy(lst->data2, value, (width * height) * dataLength * typeLength);
				break;
		case 2:	memcpy(lst->data3, value, (width * height) * dataLength * typeLength);
				break;
		default:	memcpy(lst->data, value, (width * height) * dataLength * typeLength);
	}

	/*
	for(size_t h = y0; h < endY; h++){
		memcpy(
			(char *)dest  + (h * lst->width + x0)   * dataLength,
			(char *)value + ((h - y0) * valueWidth) * dataLength,
			width * dataLength
		);
	}
*/
	lst->dirty = true;

	return LA_NO_ERROR;
}

LAErrorCode LAShaderTextureSet1d(LAShaderTexture *lst, size_t x0, size_t width, size_t valueWidth, void *value){
	LA_HANDLE_NULLPTR(lst,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value,	LA_PROPAGATE_ERROR);

	if(lst->tMode != LA_SHADER_TEXTURE_1D) return LA_ERROR_INVALIDVALUE;

	return LAShaderTextureValueCopy(lst, 0, x0, 0, width, 1, valueWidth, 1, value);
}

LAErrorCode LAShaderTextureSet2d(LAShaderTexture *lst, size_t x0, size_t y0, size_t width, size_t height, size_t valueWidth, size_t valueHeight, void *value){
	LA_HANDLE_NULLPTR(lst,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value,	LA_PROPAGATE_ERROR);

	if(lst->tMode != LA_SHADER_TEXTURE_2D) return LA_ERROR_INVALIDVALUE;

	return LAShaderTextureValueCopy(lst, 0, x0, y0, width, height, valueWidth, valueHeight, value);
}

LAErrorCode LAShaderTextureSetCubemap(LAShaderTexture *lst, size_t x0, size_t y0, size_t width, size_t height, size_t valueWidth, size_t valueHeight, void *value1, void *value2, void *value3){
	LA_HANDLE_NULLPTR(lst,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value1,	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value2,	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value3,	LA_PROPAGATE_ERROR);

	if(lst->tMode != LA_SHADER_TEXTURE_CUBEMAP) return LA_ERROR_INVALIDVALUE;

	LAErrorCode code = LA_NO_ERROR;

	code = LAShaderTextureValueCopy(lst, 0, x0, y0, width, height, valueWidth, valueHeight, value1);
	if(code) return code;
	code = LAShaderTextureValueCopy(lst, 1, x0, y0, width, height, valueWidth, valueHeight, value2);
	if(code) return code;
	code = LAShaderTextureValueCopy(lst, 2, x0, y0, width, height, valueWidth, valueHeight, value3);
	if(code) return code;

	return LA_NO_ERROR;
}

LAErrorCode LAShaderTextureAppend(LAShaderTexture **lst, LAShaderTextureMode tMode, LAShaderTextureDataType dType, LAShaderTextureType tType, size_t width, size_t height, LAShaderTexture **value){
	LA_HANDLE_NULLPTR(lst,   LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value, LA_PROPAGATE_ERROR);
	
	LAShaderTexture *now = (*lst);
	LAShaderTexture *next = NULL;

	if(now != NULL){
		printf("Now is not null (exists)\n");

		while(now->next != NULL){
			now = now->next;
		}

		LAErrorCode code = LAShaderTextureInit(&next, tMode, dType, tType, width, height);
		if(code) return code;

		now->next = next;
		(*value) = next;
	} else {
		printf("Now is null (first)\n");

		LAErrorCode code = LAShaderTextureInit(&now, tMode, dType, tType, width, height);
		if(code) return code;

		(*lst) = now;
		(*value) = now;
	}
	
	return LA_NO_ERROR;
}

LAErrorCode LAShaderTextureDestroy(LAShaderTexture *lst, bool destroyAll){
	LA_HANDLE_NULLPTR(lst, LA_PROPAGATE_ERROR);

	LAShaderTexture *now  = lst;
	LAShaderTexture *next = NULL;

	while(now != NULL){
		next = now->next;

		if(now->data3	!= NULL) free(now->data3);
		if(now->data2   != NULL) free(now->data2);
		if(now->data    != NULL) free(now->data);
		if(now          != NULL) free(now);

		now = next;
		if(!destroyAll) break;
	}

	return LA_NO_ERROR;
}
