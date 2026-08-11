#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "fixedpoint.h"
#include "utils/fixedlimits.h"

#define F32_RECP_CORRECTION 0x00000000

#define F32_RECP_CORRECTION_A 0x0000047B
#define F32_RECP_CORRECTION_B 0x0000032B

#define F64_RECP_CORRECTION_A 0x00000000047B5223
#define F64_RECP_CORRECTION_B 0x00000000032B4741

#define RECP_ITER 2
#define RECP_NO_ITER_F32 8

ufixed32_t ufixedRecp32(ufixed32_t x){
	if(x == F32_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return UF32_MAX_VALUE;
	}

	/*
		getExponent32:
		0b1...		-> exponent = 15
		0b0...01 	-> exponent = -16
		1.0 = 00000000 00000001 00000000 00000000
			-> exponent = 0
	*/
	int8_t exponent 	= getExponent32(x);
	ufixed32_t y 		= F32_ZERO;
	ufixed32_t mantissa = F32_ZERO;

	if(exponent > 0){
		mantissa 	= x       >> exponent;
	} else {
		mantissa	= x       << abs(exponent);
	}

	if(mantissa > F32_SQRT_2){
		// Sector B: [sqrt(2), 2[
		// m = sqrt(2)/2 - (x - sqrt(2)) * (sqrt(2)/2 - 0.5)/(2 - sqrt(2)) - qB
		mantissa = mantissa - F32_SQRT_2;

		// (sqrt(2)/2 - 0.5)/(2 - sqrt(2)) = 0.3535533906
		// 45/128 = 0.3515625
		mantissa = (mantissa << 5) + (mantissa << 3) + (mantissa << 2) + mantissa;
		mantissa = mantissa >> 7;

		// sqrt(2)/2 - (x - sqrt(2)) * 45/128
		mantissa = (F32_SQRT_2 >> 1) - mantissa;

		// sqrt(2)/2 - (x - sqrt(2)) * 45/128 - qB
		mantissa = mantissa - F32_RECP_CORRECTION_B;
	} else {
		// Sector A: [1, sqrt(2)[
		// m = 1 - (x-1) * sqrt(2)/2 - qA
		mantissa = mantissa - F32_ONE;

		// sqrt(2)/2 = 0.7071067812
		// 45/64 = 0.703125
		mantissa = (mantissa << 5) + (mantissa << 3) + (mantissa << 2) + mantissa;
		mantissa = mantissa >> 6;

		// 1 - (x-1) * 45/65
		mantissa = F32_ONE - mantissa;

		// 1 - (x-1) * 45/65 - qA
		mantissa = mantissa - F32_RECP_CORRECTION_A;
	}

	if(exponent > 0){
		y = mantissa >> exponent;
	} else {
		y = mantissa << abs(exponent);
	}

	/*
		y_n+1 = y_n - (1/y_n - x) / (-1/y_n^2)
		y_n+1 = y_n + y_n^2(1/y_n - x)
		y_n+1 = y_n + y_n - x*y_n^2
		y_n+1 = 2*y_n - x *y_n^2

		Iterations disabled for |exp| > 8, because overflow, underflow
		and loss of precision that lead into erratic values (overshoot).
	*/

	int8_t iterCount = RECP_ITER;
	if(abs(exponent) >= RECP_NO_ITER_F32){
		iterCount = 0;
	}

	for(int8_t i = 0; i < iterCount; i++){
		y = (y << 1) - ufixedMul32(x, ufixedMul32(y, y));
	}

	return y;
}

fixed32_t fixedRecp32(fixed32_t x){
	if(x == F32_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return F32_MAX_VALUE;
	}

	uint8_t sign = 0;
	if(x < F32_ZERO){
		sign = 1;
		x = fixedNeg32(x);
	}

	ufixed32_t y = ufixedRecp32(x);
	if(sign){
		y = fixedNeg32(y);
	}

	return y;
}

ufixed64_t ufixedRecp64(ufixed64_t x){
	if(x == F64_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return UF64_MAX_VALUE;
	}

	/*
		getExponent32:
		0b1...		-> exponent = 15
		0b0...01 	-> exponent = -16
		1.0 = 00000000 00000001 00000000 00000000
			-> exponent = 0
	*/
	int8_t exponent 	= getExponent64(x);
	ufixed64_t y 		= F64_ZERO;
	ufixed64_t mantissa = F64_ZERO;

	if(exponent > 0){
		mantissa 	= x       >> exponent;
	} else {
		mantissa	= x       << abs(exponent);
	}

	if(mantissa > F64_SQRT_2){
		// Sector B: [sqrt(2), 2[
		// m = sqrt(2)/2 - (x - sqrt(2)) * (sqrt(2)/2 - 0.5)/(2 - sqrt(2)) - qB
		mantissa = mantissa - F64_SQRT_2;

		// (sqrt(2)/2 - 0.5)/(2 - sqrt(2)) = 0.3535533906
		// 45/128 = 0.3515625
		mantissa = (mantissa << 5) + (mantissa << 3) + (mantissa << 2) + mantissa;
		mantissa = mantissa >> 7;

		// sqrt(2)/2 - (x - sqrt(2)) * 45/128
		mantissa = (F64_SQRT_2 >> 1) - mantissa;

		// sqrt(2)/2 - (x - sqrt(2)) * 45/128 - qB
		mantissa = mantissa - F64_RECP_CORRECTION_B;
	} else {
		// Sector A: [1, sqrt(2)[
		// m = 1 - (x-1) * sqrt(2)/2 - qA
		mantissa = mantissa - F64_ONE;

		// sqrt(2)/2 = 0.7071067812
		// 45/64 = 0.703125
		mantissa = (mantissa << 5) + (mantissa << 3) + (mantissa << 2) + mantissa;
		mantissa = mantissa >> 6;

		// 1 - (x-1) * 45/65
		mantissa = F64_ONE - mantissa;

		// 1 - (x-1) * 45/65 - qA
		mantissa = mantissa - F64_RECP_CORRECTION_A;
	}

	if(exponent > 0){
		y = mantissa >> exponent;
	} else {
		y = mantissa << abs(exponent);
	}

	/*
		y_n+1 = y_n - (1/y_n - x) / (-1/y_n^2)
		y_n+1 = y_n + y_n^2(1/y_n - x)
		y_n+1 = y_n + y_n - x*y_n^2
		y_n+1 = 2*y_n - x *y_n^2

		Iterations disabled for |exp| > 8, because overflow, underflow
		and loss of precision that lead into erratic values (overshoot).
	*/

	int8_t iterCount = RECP_ITER;
	if(abs(exponent) >= RECP_NO_ITER_F32){
		iterCount = 0;
	}

	for(int8_t i = 0; i < iterCount; i++){
		y = (y << 1) - ufixedMul64(x, ufixedMul64(y, y));
	}

	return y;
}

fixed64_t fixedRecp64(fixed64_t x){
	if(x == F64_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return F64_MAX_VALUE;
	}

	uint8_t sign = 0;
	if(x < F64_ZERO){
		sign = 1;
		x = fixedNeg64(x);
	}

	ufixed64_t y = ufixedRecp64(x);
	if(sign){
		y = fixedNeg64(y);
	}

	return y;
}
