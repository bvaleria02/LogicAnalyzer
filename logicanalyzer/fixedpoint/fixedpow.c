#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>
#include "fixedconstants.h"

fixed32_t fixedPow32(fixed32_t x, uint16_t n){
	if(x == F32_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return F32_ONE;
	}

	uint16_t iterCount = n >> 1;
	fixed32_t acc = F32_ONE;

	for(uint16_t i = 0; i < iterCount; i++){
		acc = fixedMul32(acc, x);
	}
	acc = fixedMul32(acc, acc);

	if(n & 0x1){
		acc = fixedMul32(acc, x);
	}
	return acc;
}

ufixed32_t ufixedPow32(ufixed32_t x, uint16_t n){
	if(x == F32_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return F32_ONE;
	}

	uint16_t iterCount = n >> 1;
	ufixed32_t acc = F32_ONE;

	for(uint16_t i = 0; i < iterCount; i++){
		acc = ufixedMul32(acc, x);
	}
	acc = ufixedMul32(acc, acc);

	if(n & 0x1){
		acc = ufixedMul32(acc, x);
	}
	return acc;
}

fixed64_t fixedPow64(fixed64_t x, uint16_t n){
	if(x == F64_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return F64_ONE;
	}

	uint16_t iterCount = n >> 1;
	fixed64_t acc = F64_ONE;

	for(uint16_t i = 0; i < iterCount; i++){
		acc = fixedMul64(acc, x);
	}
	acc = fixedMul64(acc, acc);

	if(n & 0x1){
		acc = fixedMul64(acc, x);
	}
	return acc;
}

ufixed64_t ufixedPow64(ufixed64_t x, uint16_t n){
	if(x == F64_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return F64_ONE;
	}

	uint16_t iterCount = n >> 1;
	ufixed64_t acc = F64_ONE;

	for(uint16_t i = 0; i < iterCount; i++){
		acc = ufixedMul64(acc, x);
	}
	acc = ufixedMul64(acc, acc);

	if(n & 0x1){
		acc = ufixedMul64(acc, x);
	}
	return acc;
}

fixed32_t  fixedPowf32( fixed32_t x,  fixed32_t n){
	if(x == F32_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return F32_ONE;
	}

	fixed32_t exponent;

	exponent = fixedLn32(x);
	exponent = fixedMul32(exponent, n);

	fixed32_t y = fixedExp32(exponent);

	return y;
}

ufixed32_t ufixedPowf32(ufixed32_t x, ufixed32_t n){
	if(x == F32_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return F32_ONE;
	}

	ufixed32_t exponent;

	exponent = ufixedLn32(x);
	exponent = ufixedMul32(exponent, n);

	ufixed32_t y = ufixedExp32(exponent);

	return y;
}

 fixed64_t  fixedPowf64( fixed64_t x,  fixed64_t n){
	if(x == F64_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return F64_ONE;
	}

	fixed64_t exponent;

	exponent = fixedLn64(x);
	exponent = fixedMul64(exponent, n);

	fixed64_t y = fixedExp64(exponent);

	return y;
}

ufixed64_t ufixedPowf64(ufixed64_t x, ufixed64_t n){
	if(x == F64_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return F64_ONE;
	}

	ufixed64_t exponent;

	exponent = ufixedLn64(x);
	exponent = ufixedMul64(exponent, n);

	ufixed64_t y = ufixedExp64(exponent);

	return y;
}
