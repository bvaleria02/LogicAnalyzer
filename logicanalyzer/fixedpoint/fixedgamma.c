#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>

#define F32_GAMMA_COUNT 17
#define F32_GAMMA_SHIFT 12
#define F32_GAMMA_MASK  0x00000FFF
const ufixed32_t gammaF32[F32_GAMMA_COUNT] = {
	0x00010000,
	0x0000F7B3,
	0x0000F116,
	0x0000EBED,
	0x0000E80A,
	0x0000E54A,
	0x0000E390,
	0x0000E2C7,
	0x0000E2E0,
	0x0000E3CD,
	0x0000E586,
	0x0000E805,
	0x0000EB48,
	0x0000EF4D,
	0x0000F415,
	0x0000F9A4,
	0x00010000
};

#define F64_GAMMA_COUNT 17
#define F64_GAMMA_SHIFT 28
#define F64_GAMMA_MASK  0x000000000FFFFFFF
const ufixed64_t gammaF64[F64_GAMMA_COUNT] = {
	0x0000000100000000,
	0x00000000F7B353CB,
	0x00000000F1160CB1,
	0x00000000EBECEA14,
	0x00000000E809FE24,
	0x00000000E5498F15,
	0x00000000E38FD6F4,
	0x00000000E2C75E50,
	0x00000000E2DFC48E,
	0x00000000E3CCD767,
	0x00000000E585E45B,
	0x00000000E805356B,
	0x00000000EB47AE88,
	0x00000000EF4C8448,
	0x00000000F4150658,
	0x00000000F9A479BF,
	0x0000000100000000,
};

ufixed32_t ufixedGamma32(ufixed32_t x){
	// Special case: Γ(0) is undefined, returning max value (I32_MANTISSA = 0x7FFFFFFF)
	if(x == F32_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return F32_MAX_VALUE;
	}
	
	ufixed32_t decimal = x & F32_LMASK;
	uint16_t integer   = x >> F32_RSHIFT;
	ufixed32_t temp;

	// LUT is defined for Γ(1) to Γ(2)
	uint8_t index0;
	uint8_t index1 = decimal >> F32_GAMMA_SHIFT;
	uint8_t index2 = index1 + 1;
	uint8_t index3 = index1 + 2;
	
	if(index1 == 0)	index0 = 0;
	else 			index0 = index1 - 1;

	if(index0 >= F32_GAMMA_COUNT) index0 = F32_GAMMA_COUNT - 1;
	if(index1 >= F32_GAMMA_COUNT) index1 = F32_GAMMA_COUNT - 1;
	if(index2 >= F32_GAMMA_COUNT) index2 = F32_GAMMA_COUNT - 1;
	if(index3 >= F32_GAMMA_COUNT) index3 = F32_GAMMA_COUNT - 1;

	ufixed32_t y0 = gammaF32[index0];
	ufixed32_t y1 = gammaF32[index1];
	ufixed32_t y2 = gammaF32[index2];
	ufixed32_t y3 = gammaF32[index3];

	// Cubic interpolation
	ufixed32_t iX = decimal & F32_GAMMA_MASK;
	ufixed32_t y  = ufixedCSerp32(y0, y1, y2, y3, iX);

	if(x >= (F32_ONE << 1)){
		// Γ(n + 1) = n cdot Γ(n)
		// n = (the result of cubic interpolation) + i
		for(uint16_t i = 1; i < (integer); i++){
			temp = decimal + convertIntToUF32(i, 0);
			y = ufixedMul32(y, temp);
		}
	} else if(x < F32_ONE){
		// Γ(n) = Γ(n + 1) / n
		// Γ(n + 1) = the result of the Cubic Interpolation
		// n = decimal
		temp = ufixedRecp32(x);
		y = ufixedMul32(y, temp);
	}

	return y;
}

fixed32_t fixedGamma32(fixed32_t x){
	if(x >= F32_ZERO){
		return ufixedGamma32(x);
	}
		
	ufixed32_t absolute;
	ufixed32_t decimal;
	ufixed32_t complementary;
	ufixed32_t integer;
	ufixed32_t temp;

	// -3.95 -> 3.95
	absolute = fixedNeg32(x);

	// 3.95 -> 0.95
	decimal = absolute & F32_LMASK;
	// 3.95 -> 3.0
	integer = absolute >> F32_RSHIFT;

	// 0.95 -> 0.05
	complementary = F32_ONE - decimal;

	// 0x05 -> Γ(0.05) = 19.470085
	ufixed32_t y = ufixedGamma32(complementary);

	for(uint16_t i = 0; i <= integer; i++){
		temp = decimal + convertIntToUF32(i, 0);
		temp = ufixedRecp32(temp);
		y = ufixedMul32(y,  temp);
	}

	fixed32_t ySigned;
	if(integer & 0x1){
		ySigned = y;
	} else {
		ySigned = fixedNeg32(y);
	}

	return ySigned;
}

ufixed64_t ufixedGamma64(ufixed64_t x){
	// Special case: Γ(0) is undefined, returning max value (I64_MANTISSA = 0x7FFFFFFFFFFFFFFF)
	if(x == F64_ZERO){
		return I64_MANTISSA;
	}
	
	ufixed64_t decimal = x & F64_LMASK;
	uint16_t integer   = x >> F64_RSHIFT;
	ufixed64_t temp;

	// LUT is defined for Γ(1) to Γ(2)
	uint8_t index0;
	uint8_t index1 = decimal >> F64_GAMMA_SHIFT;
	uint8_t index2 = index1 + 1;
	uint8_t index3 = index1 + 2;
	
	if(index1 == 0)	index0 = 0;
	else 			index0 = index1 - 1;

	if(index0 >= F64_GAMMA_COUNT) index0 = F64_GAMMA_COUNT - 1;
	if(index1 >= F64_GAMMA_COUNT) index1 = F64_GAMMA_COUNT - 1;
	if(index2 >= F64_GAMMA_COUNT) index2 = F64_GAMMA_COUNT - 1;
	if(index3 >= F64_GAMMA_COUNT) index3 = F64_GAMMA_COUNT - 1;

	ufixed64_t y0 = gammaF64[index0];
	ufixed64_t y1 = gammaF64[index1];
	ufixed64_t y2 = gammaF64[index2];
	ufixed64_t y3 = gammaF64[index3];

	// Cubic interpolation
	ufixed64_t iX = decimal & F64_GAMMA_MASK;
	ufixed64_t y  = ufixedCSerp64(y0, y1, y2, y3, iX);

	if(x >= (F64_ONE << 1)){
		// Γ(n + 1) = n cdot Γ(n)
		// n = (the result of cubic interpolation) + i
		for(uint16_t i = 1; i < (integer); i++){
			temp = decimal + convertIntToUF64(i, 0);
			y = ufixedMul64(y, temp);
		}
	} else if(x < F64_ONE){
		// Γ(n) = Γ(n + 1) / n
		// Γ(n + 1) = the result of the Cubic Interpolation
		// n = decimal
		temp = ufixedRecp64(x);
		y = ufixedMul64(y, temp);
	}

	return y;
}

fixed64_t fixedGamma64(fixed64_t x){
	if(x >= F64_ZERO){
		return ufixedGamma64(x);
	}
		
	ufixed64_t absolute;
	ufixed64_t decimal;
	ufixed64_t complementary;
	ufixed64_t integer;
	ufixed64_t temp;

	// -3.95 -> 3.95
	absolute = fixedNeg64(x);

	// 3.95 -> 0.95
	decimal = absolute & F64_LMASK;
	// 3.95 -> 3.0
	integer = absolute >> F64_RSHIFT;

	// 0.95 -> 0.05
	complementary = F64_ONE - decimal;

	// 0x05 -> Γ(0.05) = 19.470085
	ufixed64_t y = ufixedGamma64(complementary);

	for(uint16_t i = 0; i <= integer; i++){
		temp = decimal + convertIntToUF64(i, 0);
		temp = ufixedRecp64(temp);
		y = ufixedMul64(y,  temp);
	}

	fixed64_t ySigned;
	if(integer & 0x1){
		ySigned = y;
	} else {
		ySigned = fixedNeg64(y);
	}

	return ySigned;
}

// Reciprocal gamma

fixed32_t fixedGammaRecp32(fixed32_t x){
	fixed32_t y = fixedGamma32(x);
	y = fixedRecp32(y);
	return y;
}

ufixed32_t ufixedGammaRecp32(ufixed32_t x){
	ufixed32_t y = ufixedGamma32(x);
	y = ufixedRecp32(y);
	return y;
}

fixed64_t fixedGammaRecp64(fixed64_t x){
	fixed64_t y = fixedGamma64(x);
	y = fixedRecp64(y);
	return y;
}

ufixed64_t ufixedGammaRecp64(ufixed64_t x){
	ufixed64_t y = ufixedGamma64(x);
	y = ufixedRecp64(y);
	return y;
}

