#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "laBigInt.h"

LAErrorCode LABigIntCreateFlags(LABigInt_t *m, const size_t limbCount, const LABigIntFlags flags){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(limbCount == 0) return LA_ERROR_ZEROLENGTH;

	size_t capacity = (limbCount < 0x8) ? 0x8 : 0x8 + ((limbCount - 1) & ~(0x7));

	LABigIntLimb_t *limbs = (LABigIntLimb_t *)malloc(sizeof(LABigIntLimb_t) * capacity);
	if(limbs == NULL) return LA_ERROR_MALLOC;
	for(size_t n = 0; n < capacity; n++) limbs[n] = 0x0;

	m->limbCount 	= limbCount;
	m->capacity		= capacity;
	m->sign			= LA_BIG_INT_POSITIVE;
	m->flags		= flags | LA_BIG_INT_IS_SET;
	m->limbs 		= limbs;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntCreate(LABigInt_t *m, const size_t limbCount){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	return LABigIntCreateFlags(m, limbCount, LA_BIG_INT_READ | LA_BIG_INT_WRITE);
}

LAErrorCode LABigIntDestroy(LABigInt_t *m){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	
	if(m->limbs != NULL && !(m->flags & LA_BIG_INT_IS_STACK)) free(m->limbs);

	m->limbs 		= NULL;
	m->limbCount 	= 0x0;
	m->flags		= 0x0; 
	
	return LA_NO_ERROR;
}

LAErrorCode LABigIntSetLimbs(LABigInt_t *m, const size_t index, const LABigIntLimb_t *value, const size_t limbCount){
	LA_HANDLE_NULLPTR(m, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value, 	LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(m)))	return LA_ERROR_BIG_INT;
	
	for(size_t n = 0; n < limbCount; n++){
		size_t i = n + index;

		if(i >= m->limbCount) break;
		//printf("n: %lu\ti: %lu\n", n, i);

		m->limbs[i] = value[n];
	}

	return LA_NO_ERROR;
}

LAErrorCode LABigIntSetInt(LABigInt_t *m, const size_t index, const LABigIntLimb_t value){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	return LABigIntSetLimbs(m, index, &value, 1);
}

LAErrorCode LABigIntCreateFromHexString(LABigInt_t *m, const char *hexString){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	LAErrorCode code = LA_NO_ERROR;

	size_t digits = 0;
	size_t len    = 0;
	int8_t sign   = 1;
	for(size_t n = 0; hexString[n] != '\0'; n++){
		char c = hexString[n];

		if((c == '-') && (digits == 0))	sign = -1;
		if(c == '.') 	break;
		if(c == '\n') 	break;

		len++;

		if(c == '_') 	continue;
		if(c == ' ') 	continue;
		if(c == '-') 	continue;

		if((c >= '0') && (c <= '9')){
			digits++;
			continue;
		}

		c = c & 0xDF;

		if((c >= 'A') && (c <= 'F')){
			digits++;
			continue;
		}
	}

	size_t limbCount = (digits <= 0x8) ? 1 : 1 + ((digits-1) >> 3);
	// printf("digits: %lu\tlimbCount: %lu\n", digits, limbCount);
	size_t limbIndex = 0;
	LABigIntLimb_t acc = 0;
	size_t innerCounter = 0;
	int8_t tempValue = 0;

	code = LABigIntCreateFlags(m, limbCount, LA_BIG_INT_READ | LA_BIG_INT_WRITE);
	if(code) goto cleanup;
	code = LABigIntLimbSet(m, 0, m->limbCount, 0x0);
	if(code) goto cleanup;

	for(size_t i = 0; i < len; i++){
		size_t n = len - i - 1;
		char c = hexString[n];

		if((c >= 'a') && (c <= 'z')) c = c & 0xDF;

		if(c == '_') 	continue;
		if(c == ' ') 	continue;
		if(c == '-') 	continue;
		
		if((c >= '0') && (c <= '9')){
			tempValue = (c - '0');
		} else if (c >= 'A' && c <= 'Z'){
			tempValue = (c - 'A') + 0xA;
		}

		acc = acc | (tempValue << (innerCounter*4));
		innerCounter++;
		if(innerCounter >= 8){
			code = LABigIntSetInt(m, limbIndex, acc);
			if(code) goto cleanup;
			innerCounter = 0;
			limbIndex++;
			acc = 0x0;
		}
	}

	if(innerCounter != 0){
		code = LABigIntSetInt(m, limbIndex, acc);
		if(code) goto cleanup;
	}

	m->sign = (sign == 1) ? LA_BIG_INT_POSITIVE : LA_BIG_INT_NEGATIVE;

	return LA_NO_ERROR;	
cleanup:
	LABigIntDestroy(m);
	return code;
}


LAErrorCode LABigIntCreateFromIntStack(LABigInt_t *a, int64_t n, LABigIntLimb_t *limbs){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(limbs, LA_PROPAGATE_ERROR);

	a->limbs		= (LABigIntLimb_t *)limbs;
	a->limbs[0]		= labs(n) & 0xFFFFFFFF;
	a->limbs[1]		= labs(n) >> 32;
	a->sign  		= (n < 0) ? LA_BIG_INT_NEGATIVE : LA_BIG_INT_POSITIVE;
	a->capacity 	= 0x2;
	a->limbCount 	= 0x2;
	a->flags		= LA_BIG_INT_READ | LA_BIG_INT_WRITE | LA_BIG_INT_IS_SET | LA_BIG_INT_IS_STACK;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntCreateFromUIntStack(LABigInt_t *a, uint64_t n, LABigIntLimb_t *limbs){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(limbs, LA_PROPAGATE_ERROR);

	a->limbs		= (LABigIntLimb_t *)limbs;
	a->limbs[0]		= n & 0xFFFFFFFF;
	a->limbs[1]		= n >> 32;
	a->sign  		= LA_BIG_INT_POSITIVE;
	a->capacity 	= 0x2;
	a->limbCount 	= 0x2;
	a->flags		= LA_BIG_INT_READ | LA_BIG_INT_WRITE | LA_BIG_INT_IS_SET | LA_BIG_INT_IS_STACK;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntCreateFromInt(LABigInt_t *a, int64_t n){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	code = LABigIntCreateFlags(a, 0x2, LA_BIG_INT_READ | LA_BIG_INT_WRITE);
	if(code) goto cleanup;

	a->limbs[0]		= labs(n) & 0xFFFFFFFF;
	a->limbs[1]		= labs(n) >> 32;
	a->sign  		= (n < 0) ? LA_BIG_INT_NEGATIVE : LA_BIG_INT_POSITIVE;

	return LA_NO_ERROR;

cleanup:
	LABigIntDestroy(a);
	return code;
}

LAErrorCode LABigIntCreateFromUInt(LABigInt_t *a, uint64_t n){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	code = LABigIntCreateFlags(a, 0x2, LA_BIG_INT_READ | LA_BIG_INT_WRITE);
	if(code) goto cleanup;

	a->limbs[0]		= n & 0xFFFFFFFF;
	a->limbs[1]		= n >> 32;
	a->sign  		= LA_BIG_INT_POSITIVE;

	return LA_NO_ERROR;

cleanup:
	LABigIntDestroy(a);
	return code;
}

LAErrorCode LABigIntCreateFromDecString(LABigInt_t *a, const char *decString){
	LA_HANDLE_NULLPTR(a, 		 LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(decString, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	//Count valid characters
	size_t digits = 0;
	LABigIntSign_t sign = LA_BIG_INT_POSITIVE;
	for(size_t i = 0; decString[i] != '\0'; i++){
		const char c = decString[i];

		if(c == '.') break;
		if(c == '\n') break;
		if(c == '\t') break;

		if((c == '-') && (digits == 0)) sign = LA_BIG_INT_NEGATIVE;

		if((c >= '0') && (c <= '9'))	digits++;
	}

	// As limbs are uint32_t, they can hold from 0 to 4294967295, we can say,
	// each limb can hold 9 digits (and half of a 10th one)
	// Let's use 9 to be conservative, as the limbCount is padded to 8*N
	const size_t limbCount = 1 + (digits / 9);

	code = LABigIntCreateFlags(a, limbCount, LA_BIG_INT_READ | LA_BIG_INT_WRITE);
	if(code) goto cleanup;

	for(size_t i = 0; decString[i] != '\0'; i++){
		const char c = decString[i];

		if(c == '.') break;
		if(c == '\n') break;
		if(c == '\t') break;

		if((c >= '0') && (c <= '9')){
			uint64_t value = c - '0'; 

			code = LABigIntMulUIntSatInplace(a, 10);
			if(code) goto cleanup;
			code = LABigIntAddUIntSatInplace(a, value);
			if(code) goto cleanup;
		}
	}

	a->sign = sign;
	return LA_NO_ERROR;

cleanup:
	LABigIntDestroy(a);
	return code;
}

LAErrorCode LABigIntCreateFromOther(LABigInt_t *dest, LABigInt_t *src, const size_t limbCount){
	LA_HANDLE_NULLPTR(src, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(dest, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(src)))	return LA_ERROR_BIG_INT;

	LAErrorCode code = LA_NO_ERROR;

	code = LABigIntCreateFlags(
		dest,
		(limbCount == 0x0) ? src->limbCount : limbCount,
		LA_BIG_INT_READ | LA_BIG_INT_WRITE
	);
	if(code) return code;

	code = LABigIntCopyLimbs(src, dest, 0, dest->limbCount - 1);
	if(code) return code;

	return LA_NO_ERROR;
}
