#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils/fixedlimits.h"

uint8_t fixedIsNeg32( fixed32_t x){
	if(x < F32_ZERO){
		return TRUE;
	}

	return FALSE;
}

uint8_t ufixedIsNeg32(ufixed32_t x){
	(void) x;
	return FALSE;
}

uint8_t  fixedIsNeg64( fixed64_t x){
	if(x < F64_ZERO){
		return TRUE;
	}

	return FALSE;
}

uint8_t ufixedIsNeg64(ufixed64_t x){
	(void) x;
	return FALSE;
}

uint8_t  fixedIsZero32( fixed32_t x){
	if(x == F32_ZERO){
		return TRUE;
	}

	return FALSE;
}

uint8_t ufixedIsZero32(ufixed32_t x){
	if(x == F32_ZERO){
		return TRUE;
	}

	return FALSE;
}

uint8_t  fixedIsZero64( fixed64_t x){
	if(x == F64_ZERO){
		return TRUE;
	}

	return FALSE;
}

uint8_t ufixedIsZero64(ufixed64_t x){
	if(x == F64_ZERO){
		return TRUE;
	}

	return FALSE;
}

uint8_t  fixedIsPos32( fixed32_t x){
	if(x > F32_ZERO){
		return TRUE;
	}

	return FALSE;
}

uint8_t ufixedIsPos32(ufixed32_t x){
	if(x > F32_ZERO){
		return TRUE;
	}

	return FALSE;
}

uint8_t  fixedIsPos64( fixed64_t x){
	if(x > F64_ZERO){
		return TRUE;
	}

	return FALSE;
}

uint8_t ufixedIsPos64(ufixed64_t x){
	if(x > F64_ZERO){
		return TRUE;
	}

	return FALSE;
}

uint8_t  fixedIsInt32( fixed32_t x){
	fixed32_t y = fixedAbs32(x);
	return ufixedIsInt32(y);
}

uint8_t ufixedIsInt32(ufixed32_t x){
	ufixed32_t y = x & F32_HMASK;

	if(y != x){
		return FALSE;
	}

	return TRUE;
}

uint8_t  fixedIsInt64( fixed64_t x){
	fixed32_t y = fixedAbs64(x);
	return ufixedIsInt64(y);
}

uint8_t ufixedIsInt64(ufixed64_t x){
	ufixed32_t y = x & F64_HMASK;

	if(y != x){
		return FALSE;
	}

	return TRUE;
}

FPErrorCode  fixedFrexp32(  fixed32_t x,  int8_t *exponent,  fixed32_t *mantissa){
	uint8_t sign = 0;
	if(fixedIsNeg32(x)){
		sign = 1;
		x = fixedAbs32(x);
	}

	FPErrorCode code = ufixedFrexp32(x, exponent, (ufixed32_t *)mantissa);
	if(code != FP_NO_ERROR){
		return code;
	}

	if(mantissa != NULL && sign){
		(*mantissa) = fixedNeg32(*mantissa);
	}

	return FP_NO_ERROR;
}

FPErrorCode ufixedFrexp32( ufixed32_t x, int8_t *exponent, ufixed32_t *mantissa){
	if(exponent == NULL && mantissa == NULL){
		fixedSetErrno(FP_ERROR_NULLPTR);
		return FP_ERROR_NULLPTR;
	}

	if(x == F32_ZERO){
		if(exponent != NULL) (*exponent) = 0;
		if(mantissa != NULL) (*mantissa) = 0;
		return FP_NO_ERROR;
	}

	int8_t exp = getExponent32(x);
	ufixed32_t m = x;

	if(exp < 0){
		m = x << abs(exp);
	} else if (exp > 0){
		m = x >> exp;
	}

	if(exponent != NULL){
		(*exponent) = exp;
	}

	if(mantissa != NULL){
		(*mantissa) = m;
	}

	return FP_NO_ERROR;
}

FPErrorCode  fixedFrexp64(  fixed64_t x,  int8_t *exponent,  fixed64_t *mantissa){
	uint8_t sign = 0;
	if(fixedIsNeg64(x)){
		sign = 1;
		x = fixedAbs64(x);
	}

	FPErrorCode code = ufixedFrexp64(x, exponent, (ufixed64_t *)mantissa);
	if(code != FP_NO_ERROR){
		return code;
	}

	if(mantissa != NULL && sign){
		(*mantissa) = fixedNeg64(*mantissa);
	}

	return FP_NO_ERROR;
}

FPErrorCode ufixedFrexp64( ufixed64_t x, int8_t *exponent, ufixed64_t *mantissa){
	if(exponent == NULL && mantissa == NULL){
		fixedSetErrno(FP_ERROR_NULLPTR);
		return FP_ERROR_NULLPTR;
	}

	if(x == F64_ZERO){
		if(exponent != NULL) (*exponent) = 0;
		if(mantissa != NULL) (*mantissa) = 0;
		return FP_NO_ERROR;
	}

	int8_t exp = getExponent64(x);
	ufixed64_t m = x;

	if(exp < 0){
		m = x << abs(exp);
	} else if (exp > 0){
		m = x >> exp;
	}

	if(exponent != NULL){
		(*exponent) = exp;
	}

	if(mantissa != NULL){
		(*mantissa) = m;
	}

	return FP_NO_ERROR;
}

FPErrorCode  fixedFrexp232( fixed32_t x, int8_t *exponent,  fixed32_t *mantissa){
	if(exponent == NULL && mantissa == NULL){
		fixedSetErrno(FP_ERROR_NULLPTR);
		return FP_ERROR_NULLPTR;
	}

	int8_t exponent_internal;
	fixed32_t mantissa_internal;

	FPErrorCode code = fixedFrexp32(x, &exponent_internal, &mantissa_internal);
	if(code != FP_NO_ERROR){
		return code;
	}

	if(abs(exponent_internal) & 0x1){
		exponent_internal -= 1;
		mantissa_internal = fixedLShift32(mantissa_internal, 1);
	}

	if(exponent != NULL){
		(*exponent) = exponent_internal;
	}

	if(mantissa != NULL){
		(*mantissa) = mantissa_internal;
	}

	return FP_NO_ERROR;
}


FPErrorCode ufixedFrexp232(ufixed32_t x, int8_t *exponent, ufixed32_t *mantissa){
	if(exponent == NULL && mantissa == NULL){
		fixedSetErrno(FP_ERROR_NULLPTR);
		return FP_ERROR_NULLPTR;
	}

	int8_t exponent_internal;
	ufixed32_t mantissa_internal;

	FPErrorCode code = ufixedFrexp32(x, &exponent_internal, &mantissa_internal);
	if(code != FP_NO_ERROR){
		return code;
	}

	if(abs(exponent_internal) & 0x1){
		exponent_internal -= 1;
		mantissa_internal = fixedLShift32(mantissa_internal, 1);
	}

	if(exponent != NULL){
		(*exponent) = exponent_internal;
	}

	if(mantissa != NULL){
		(*mantissa) = mantissa_internal;
	}

	return FP_NO_ERROR;
}

FPErrorCode  fixedFrexp264( fixed64_t x, int8_t *exponent,  fixed64_t *mantissa){
	if(exponent == NULL && mantissa == NULL){
		fixedSetErrno(FP_ERROR_NULLPTR);
		return FP_ERROR_NULLPTR;
	}

	int8_t exponent_internal;
	fixed64_t mantissa_internal;

	FPErrorCode code = fixedFrexp64(x, &exponent_internal, &mantissa_internal);
	if(code != FP_NO_ERROR){
		return code;
	}

	if(abs(exponent_internal) & 0x1){
		exponent_internal -= 1;
		mantissa_internal = fixedLShift64(mantissa_internal, 1);
	}

	if(exponent != NULL){
		(*exponent) = exponent_internal;
	}

	if(mantissa != NULL){
		(*mantissa) = mantissa_internal;
	}

	return FP_NO_ERROR;
}

FPErrorCode ufixedFrexp264(ufixed64_t x, int8_t *exponent, ufixed64_t *mantissa){
	if(exponent == NULL && mantissa == NULL){
		fixedSetErrno(FP_ERROR_NULLPTR);
		return FP_ERROR_NULLPTR;
	}

	int8_t exponent_internal;
	ufixed64_t mantissa_internal;

	FPErrorCode code = ufixedFrexp64(x, &exponent_internal, &mantissa_internal);
	if(code != FP_NO_ERROR){
		return code;
	}

	if(abs(exponent_internal) & 0x1){
		exponent_internal -= 1;
		mantissa_internal = fixedLShift64(mantissa_internal, 1);
	}

	if(exponent != NULL){
		(*exponent) = exponent_internal;
	}

	if(mantissa != NULL){
		(*mantissa) = mantissa_internal;
	}

	return FP_NO_ERROR;
}

FPErrorCode  fixedModf32(   fixed32_t x,  fixed32_t *integer,   fixed32_t *decimal){
	uint8_t sign = 0;
	if(fixedIsNeg32(x)){
		sign = 1;
		x = fixedAbs32(x);
	}

	FPErrorCode code = ufixedModf32(x, (ufixed32_t *)integer, (ufixed32_t *)decimal);
	if(code != FP_NO_ERROR){
		return code;
	}

	if(integer != NULL && sign){
		(*integer) = fixedNeg32(*integer);
	}

	return FP_NO_ERROR;
}

FPErrorCode ufixedModf32(  ufixed32_t x, ufixed32_t *integer,  ufixed32_t *decimal){
	if(integer == NULL && decimal == NULL){
		fixedSetErrno(FP_ERROR_NULLPTR);
		return FP_ERROR_NULLPTR;
	}

	if(integer != NULL){
		(*integer) = x & F32_HMASK;
	}

	if(decimal != NULL){
		(*decimal) = x & F32_LMASK;
	}

	return FP_NO_ERROR;
}

FPErrorCode  fixedModf64(   fixed64_t x,  fixed64_t *integer,   fixed64_t *decimal){
	uint8_t sign = 0;
	if(fixedIsNeg64(x)){
		sign = 1;
		x = fixedAbs64(x);
	}

	FPErrorCode code = ufixedModf64(x, (ufixed64_t *)integer, (ufixed64_t *)decimal);
	if(code != FP_NO_ERROR){
		return code;
	}

	if(integer != NULL && sign){
		(*integer) = fixedNeg64(*integer);
	}

	return FP_NO_ERROR;
}

FPErrorCode ufixedModf64(  ufixed64_t x, ufixed64_t *integer,  ufixed64_t *decimal){
	if(integer == NULL && decimal == NULL){
		fixedSetErrno(FP_ERROR_NULLPTR);
		return FP_ERROR_NULLPTR;
	}

	if(integer != NULL){
		(*integer) = x & F64_HMASK;
	}

	if(decimal != NULL){
		(*decimal) = x & F64_LMASK;
	}

	return FP_NO_ERROR;
}
