#include "../liblogicanalyzer.h"
#include "shaders.h"
#include <stdint.h>
#include <stdlib.h>
#include "../glad/glad/glad.h"

const size_t LAShaderTypeLength[LA_SHADER_TYPE_AMOUNT] = {
	sizeof(uint8_t), 	sizeof(int8_t),
	sizeof(uint16_t), 	sizeof(int16_t),
	sizeof(uint32_t), 	sizeof(int32_t),
	sizeof(float),
	sizeof(LAVec2_t), 	sizeof(LAVec3_t),	sizeof(LAVec4_t),
	sizeof(LAMat2_t), 	sizeof(LAMat3_t),	sizeof(LAMat4_t),
};

LAErrorCode LAShaderValueInit(LAShaderValue **lsv, LAShaderType type, size_t amount, const char *name){
	LA_HANDLE_NULLPTR(lsv, LA_PROPAGATE_ERROR);

	if(amount == 0) return LA_ERROR_ZEROLENGTH;
	LAErrorCode code = LA_NO_ERROR;
	bool isStructAllocated = false;
	bool isStructValueAllocated = false;
	bool isNameAllocated = false;

	(*lsv) = (LAShaderValue *)malloc(sizeof(LAShaderValue) * 1);
	if((*lsv) == NULL) goto malloc_error;
	isStructAllocated = true;

	(*lsv)->type 	= type;
	(*lsv)->amount 	= amount;
	(*lsv)->next 	= NULL;
	(*lsv)->id		= 0x0;
	(*lsv)->dirty = false;

	(*lsv)->value 	= (void *)malloc(LAShaderTypeLength[type] * amount);
	if((*lsv)->value == NULL) goto malloc_error;
	isStructValueAllocated = true;

	if(name != NULL){
		size_t namelength = strlen(name);
		(*lsv)->uniformName = (char *)malloc(namelength + 1);
		if((*lsv)->uniformName == NULL) goto malloc_error;
		strncpy((*lsv)->uniformName, name, namelength);
		(*lsv)->uniformName[namelength] = '\0';
	}
	
	return LA_NO_ERROR;

malloc_error:
	code = LA_ERROR_MALLOC;
	goto handle_error;

handle_error:
	if(isNameAllocated){
		if((*lsv)->uniformName != NULL) free((*lsv)->uniformName);
	}

	if(isStructValueAllocated){
		if((*lsv)->value != NULL) free((*lsv)->value);
	}

	if(isStructAllocated){
		if((*lsv) != NULL) free((*lsv));
	}
	return code;
}

LAErrorCode LAShaderValueCopy(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, void *value){
	LA_HANDLE_NULLPTR(lsv, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(lsv->value, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value, 		LA_PROPAGATE_ERROR);

	if(length == 0) 						return LA_ERROR_ZEROLENGTH;
	if(lsv->type >= LA_SHADER_TYPE_AMOUNT) 	return LA_ERROR_INVALIDVALUE;
	
	size_t end = start + length;
	size_t typeLength = LAShaderTypeLength[lsv->type];

	if(start >= lsv->amount) 	return LA_ERROR_OUTOFBOUND;
	if(end   >  lsv->amount) 	return LA_ERROR_OUTOFBOUND;
	if(start >= end) 		 	return LA_ERROR_INVALIDVALUE;
	if(length > valueLength) 	return LA_ERROR_OUTOFBOUND;

	void *dest = ((char *)lsv->value) + (start * LAShaderTypeLength[lsv->type]);
	memcpy(dest, value, length * typeLength);

	lsv->dirty = true;

	return LA_NO_ERROR;
}

LAErrorCode LAShaderValueSetU8(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, uint8_t *value){
	LA_HANDLE_NULLPTR(lsv, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value,		LA_PROPAGATE_ERROR);
	if(lsv->type != LA_SHADER_TYPE_UINT8) return LA_ERROR_INCORRECTVALUE;
	return LAShaderValueCopy(lsv, start, length, valueLength, (void *)value);
}

LAErrorCode LAShaderValueSetI8(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, int8_t *value){
	LA_HANDLE_NULLPTR(lsv, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value,		LA_PROPAGATE_ERROR);
	if(lsv->type != LA_SHADER_TYPE_INT8) return LA_ERROR_INCORRECTVALUE;
	return LAShaderValueCopy(lsv, start, length, valueLength, (void *)value);
}

LAErrorCode LAShaderValueSetU16(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, uint16_t *value){
	LA_HANDLE_NULLPTR(lsv, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value,		LA_PROPAGATE_ERROR);
	if(lsv->type != LA_SHADER_TYPE_UINT16) return LA_ERROR_INCORRECTVALUE;
	return LAShaderValueCopy(lsv, start, length, valueLength, (void *)value);
}

LAErrorCode LAShaderValueSetI16(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, int16_t *value){
	LA_HANDLE_NULLPTR(lsv, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value,		LA_PROPAGATE_ERROR);
	if(lsv->type != LA_SHADER_TYPE_INT16) return LA_ERROR_INCORRECTVALUE;
	return LAShaderValueCopy(lsv, start, length, valueLength, (void *)value);
}

LAErrorCode LAShaderValueSetU32(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, uint32_t *value){
	LA_HANDLE_NULLPTR(lsv, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value,		LA_PROPAGATE_ERROR);
	if(lsv->type != LA_SHADER_TYPE_UINT32) return LA_ERROR_INCORRECTVALUE;
	return LAShaderValueCopy(lsv, start, length, valueLength, (void *)value);
}

LAErrorCode LAShaderValueSetI32(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, int32_t *value){
	LA_HANDLE_NULLPTR(lsv, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value,		LA_PROPAGATE_ERROR);
	if(lsv->type != LA_SHADER_TYPE_INT32) return LA_ERROR_INCORRECTVALUE;
	return LAShaderValueCopy(lsv, start, length, valueLength, (void *)value);
}

LAErrorCode LAShaderValueSetF(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, float *value){
	LA_HANDLE_NULLPTR(lsv, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value,		LA_PROPAGATE_ERROR);
	if(lsv->type != LA_SHADER_TYPE_FLOAT) return LA_ERROR_INCORRECTVALUE;
	return LAShaderValueCopy(lsv, start, length, valueLength, (void *)value);
}

LAErrorCode LAShaderValueSetVec2(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, LAVec2_t *value){
	LA_HANDLE_NULLPTR(lsv, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value,		LA_PROPAGATE_ERROR);
	if(lsv->type != LA_SHADER_TYPE_VEC2) return LA_ERROR_INCORRECTVALUE;
	return LAShaderValueCopy(lsv, start, length, valueLength, (void *)value);
}

LAErrorCode LAShaderValueSetVec3(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, LAVec3_t *value){
	LA_HANDLE_NULLPTR(lsv, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value,		LA_PROPAGATE_ERROR);
	if(lsv->type != LA_SHADER_TYPE_VEC3) return LA_ERROR_INCORRECTVALUE;
	return LAShaderValueCopy(lsv, start, length, valueLength, (void *)value);
}

LAErrorCode LAShaderValueSetVec4(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, LAVec4_t *value){
	LA_HANDLE_NULLPTR(lsv, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value,		LA_PROPAGATE_ERROR);
	if(lsv->type != LA_SHADER_TYPE_VEC4) return LA_ERROR_INCORRECTVALUE;
	return LAShaderValueCopy(lsv, start, length, valueLength, (void *)value);
}

LAErrorCode LAShaderValueSetMat2(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, LAMat2_t *value){
	LA_HANDLE_NULLPTR(lsv, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value,		LA_PROPAGATE_ERROR);
	if(lsv->type != LA_SHADER_TYPE_MAT2) return LA_ERROR_INCORRECTVALUE;
	return LAShaderValueCopy(lsv, start, length, valueLength, (void *)value);
}

LAErrorCode LAShaderValueSetMat3(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, LAMat3_t *value){
	LA_HANDLE_NULLPTR(lsv, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value,		LA_PROPAGATE_ERROR);
	if(lsv->type != LA_SHADER_TYPE_MAT3) return LA_ERROR_INCORRECTVALUE;
	return LAShaderValueCopy(lsv, start, length, valueLength, (void *)value);
}

LAErrorCode LAShaderValueSetMat4(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, LAMat4_t *value){
	LA_HANDLE_NULLPTR(lsv, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value,		LA_PROPAGATE_ERROR);
	if(lsv->type != LA_SHADER_TYPE_MAT4) return LA_ERROR_INCORRECTVALUE;
	return LAShaderValueCopy(lsv, start, length, valueLength, (void *)value);
}

LAErrorCode LAShaderValueAppend(LAShaderValue **lsv, LAShaderType type, size_t amount, LAShaderValue **value, const char *name){
	LA_HANDLE_NULLPTR(lsv,   LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value, LA_PROPAGATE_ERROR);
	
	LAShaderValue *now = (*lsv);
	LAShaderValue *next = NULL;

	if(now != NULL){
		printf("Now is not null (exists)\n");
		while(now->next != NULL){
			now = now->next;
		}
		LAErrorCode code = LAShaderValueInit(&next, type, amount, name);
		if(code) return code;
		now->next = next;
		(*value) = next;
	} else {
		printf("Now is null (first)\n");
		LAErrorCode code = LAShaderValueInit(&now, type, amount, name);
		if(code) return code;
		(*lsv) = now;
		(*value) = now;
	}
	
	return LA_NO_ERROR;
}

LAErrorCode LAShaderValueDestroy(LAShaderValue *lsv, bool destroyAll){
	LA_HANDLE_NULLPTR(lsv, LA_PROPAGATE_ERROR);

	LAShaderValue *now  = lsv;
	LAShaderValue *next = NULL;

	while(now != NULL){
		next = now->next;

		if(now->uniformName != NULL) free(now->uniformName);
		if(now->value       != NULL) free(now->value);
		if(now              != NULL) free(now);

		now = next;

		if(!destroyAll) break;
	}

	return LA_NO_ERROR;
}
