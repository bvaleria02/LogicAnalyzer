#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils/fixedlimits.h"

ufixed32_t abstractSqrt32(ufixed32_t x){
	ufixed32_t result = x;

	ufixed32_t low = 0;
	ufixed32_t high = x;
	if(x < F32_ONE){
		high = F32_ONE;
	}

	ufixed32_t acc = 0;
	int8_t cmp = 0;

	for(uint8_t i = 0; i < F32_SIZE; i++){
		// (low + high) / 2
		result = ufixedAdd32(low, high);
		result = result >> 1;
#ifdef DEBUG_MODE
		printf("low: %08x\thigh: %08x\tresult: %08x\n", low, high, result);
#endif

		// check if result is greater than sqrt(65536)
		cmp = ufixedCmp32(result, SQRT_U32_MAX);
		if(cmp == CMP_GREATER){
			high = result;
			continue;
		}

		// acc = result * result
		acc = ufixedMul32(result, result);
		cmp = ufixedCmp32(acc, x);
		if(cmp == CMP_GREATER){
			high = result;
		} else if (acc == x){
			return result;
		} else{
			low = result;
		};
	}
	return result;
}

ufixed64_t abstractSqrt64(ufixed64_t x){
	ufixed64_t result = x;

	ufixed64_t low = 0;
	ufixed64_t high = x;
	if(x < F64_ONE){
		high = F64_ONE;
	}

	ufixed64_t acc = 0;
	int8_t cmp = 0;

	for(uint8_t i = 0; i < F64_SIZE; i++){
		// (low + high) / 2
		result = ufixedAdd64(low, high);
		result = result >> 1;
#ifdef DEBUG_MODE
		printf("low: %016lx\thigh: %016lx\tresult: %016lx\n", low, high, result);
#endif

		// check if result is greater than sqrt(4294967296) 2**32
		cmp = ufixedCmp64(result, SQRT_U64_MAX);
		if(cmp == CMP_GREATER){
			high = result;
			continue;
		}

		// acc = result * result
		acc = ufixedMul64(result, result);
		cmp = ufixedCmp64(acc, x);
		if(cmp == CMP_GREATER){
			high = result;
		} else if (acc == x){
			return result;
		} else{
			low = result;
		};
	}
	return result;
}

#define SQRT_ITER 2
#define F32_SQRT_NO_ITER 11
#define F64_SQRT_NO_ITER 26

#define F32_SQRT_CORRECTION_A 0x0000030A
#define F32_SQRT_CORRECTION_B 0x00000441

#define F64_SQRT_CORRECTION_A 0x00000000030989BE
#define F64_SQRT_CORRECTION_B 0x000000000441062D

ufixed32_t abstractSqrt32_new(ufixed32_t x){
	if(x == F32_ZERO){
		fixedSetErrno(FP_ERROR_NEGATIVE);
		return 0;
	}

	int8_t exponent = getExponent32(x);
	ufixed32_t y = x;

	if(exponent > 0){
		y = x >> (exponent & ~(0x1));
	} else if (exponent < 0){
		y = x << ((abs(exponent)+1) & ~(0x1));
	}
	
	// y between [1, 4[
	//printf("In: %lf\texp: %i\tOut: %lf\n", convertUF32ToDouble(x), exponent, convertUF32ToDouble(y));
	if(y > (F32_ONE << 1)){
		// Sector B: y ∈ [2, 4[
		// y = sqrt(2) + (y-2)*(2 - sqrt(2))/2 + qB

		// y - 2
		y = y - (F32_ONE << 1);

		// (2 - sqrt(2))/2 = 0.2928932188135
		// 75/256 = 0.29296875
		y = (y << 6) + (y << 3) + (y << 1) + y;
		y = y >> 8;

		// y = sqrt(2) + (y-2)*75/256
		y = F32_SQRT_2 + y;

		// y = sqrt(2) + (y-2)*75/256 + qB
		y = y + F32_SQRT_CORRECTION_B;
	} else {
		// Sector A: y ∈ [1, 2[
		// y = 1 + (y-1)*(sqrt(2) - 1) + qA

		// y - 1
		y = y - F32_ONE;

		// (sqrt(2) - 1) = 0.4142135623731
		// 53/128 = 0.4140625
		y = (y << 5) + (y << 4) + (y << 2) + y;
		y = y >> 7;

		// y = 1 + (y-1)*53/128
		y = F32_ONE + y;

		// y = 1 + (y-1)*53/128 + qA
		y = y + F32_SQRT_CORRECTION_A;
	}

	if(exponent > 0){
		y = y << (exponent >> 1);
	} else if (exponent < 0){
		y = y >> ((abs(exponent)+1) >> 1);
	}

	/*
		f(y) 	= y^2 - x
		f'(y)	= 2y

		y_n+1	= y_n - (y_n^2 - x)/(2y)
		y_n+1	= y_n - y_n/2 + x/2y
		y_n+1	= (1/2)*y_n + x/2y
	*/

	ufixed32_t reciprocal;

	// Iterations are disabled for x > 2048.0 due to
	// loss of precision and overflow. 
	int8_t iterCount = SQRT_ITER;
	if(abs(exponent) > F32_SQRT_NO_ITER){
		iterCount = 0;
	}

	for(int8_t i = 0; i < iterCount; i++){
		reciprocal = ufixedRecp32(y);
		y = (y >> 1) + ufixedMul32(x, reciprocal >> 1);
	}

	return y;
}

ufixed64_t abstractSqrt64_new(ufixed64_t x){
	if(x == F64_ZERO){
		fixedSetErrno(FP_ERROR_NEGATIVE);
		return 0;
	}

	int8_t exponent = getExponent64(x);
	ufixed64_t y = x;

	if(exponent > 0){
		y = x >> (exponent & ~(0x1));
	} else if (exponent < 0){
		y = x << ((abs(exponent)+1) & ~(0x1));
	}
	
	// y between [1, 4[
	//printf("In: %lf\texp: %i\tOut: %lf\n", convertUF32ToDouble(x), exponent, convertUF32ToDouble(y));
	if(y > (F64_ONE << 1)){
		// Sector B: y ∈ [2, 4[
		// y = sqrt(2) + (y-2)*(2 - sqrt(2))/2 + qB

		// y - 2
		y = y - (F64_ONE << 1);

		// (2 - sqrt(2))/2 = 0.2928932188135
		// 75/256 = 0.29296875
		y = (y << 6) + (y << 3) + (y << 1) + y;
		y = y >> 8;

		// y = sqrt(2) + (y-2)*75/256
		y = F64_SQRT_2 + y;

		// y = sqrt(2) + (y-2)*75/256 + qB
		y = y + F64_SQRT_CORRECTION_B;
	} else {
		// Sector A: y ∈ [1, 2[
		// y = 1 + (y-1)*(sqrt(2) - 1) + qA

		// y - 1
		y = y - F64_ONE;

		// (sqrt(2) - 1) = 0.4142135623731
		// 53/128 = 0.4140625
		y = (y << 5) + (y << 4) + (y << 2) + y;
		y = y >> 7;

		// y = 1 + (y-1)*53/128
		y = F64_ONE + y;

		// y = 1 + (y-1)*53/128 + qA
		y = y + F64_SQRT_CORRECTION_A;
	}

	if(exponent > 0){
		y = y << (exponent >> 1);
	} else if (exponent < 0){
		y = y >> ((abs(exponent)+1) >> 1);
	}

	/*
		f(y) 	= y^2 - x
		f'(y)	= 2y

		y_n+1	= y_n - (y_n^2 - x)/(2y)
		y_n+1	= y_n - y_n/2 + x/2y
		y_n+1	= (1/2)*y_n + x/2y
	*/

	ufixed64_t reciprocal;

	// Iterations are disabled for x > 2048.0 due to
	// loss of precision and overflow. 
	int8_t iterCount = SQRT_ITER;
	if(abs(exponent) > F64_SQRT_NO_ITER){
		iterCount = 0;
	}

	for(int8_t i = 0; i < iterCount; i++){
		reciprocal = ufixedRecp64(y);
		y = (y >> 1) + ufixedMul64(x, reciprocal >> 1);
	}

	return y;
}

fixed32_t fixedSqrt32(fixed32_t x){
	if(x < 0){
		fixedSetErrno(FP_ERROR_NEGATIVE);
		return 0;
	}
	
	return abstractSqrt32_new(x);
}

ufixed32_t ufixedSqrt32(ufixed32_t x){
	return abstractSqrt32_new(x);
}

fixed64_t fixedSqrt64(fixed64_t x){
	if(x < 0){
		fixedSetErrno(FP_ERROR_NEGATIVE);
		return 0;
	}
	
	return abstractSqrt64_new(x);
}

ufixed64_t ufixedSqrt64(ufixed64_t x){
	return abstractSqrt64_new(x);
}
