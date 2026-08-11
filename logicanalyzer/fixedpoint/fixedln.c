#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "fixedconstants.h"
#include "utils/fixedlimits.h"

#define F64_LN_DISABLE_NEWTON 30

fixed32_t abstractLog232(ufixed32_t x){
	uint8_t rawShift = getBiggestOne32(x);
	int16_t integerPart = (F32_SIZE >> 1) - rawShift - 1;
	
	if(integerPart > 0){
		x = x >> integerPart;
	} else if (integerPart < 0){
		x = x << (-1 * integerPart);
	}
#ifdef DEBUG_MODE
	printf("x: %08x\n", x);
#endif

#ifdef DEBUG_MODE
	printf("shift: %i\tint: %i\n", rawShift, integerPart);
#endif
	fixed32_t result = convertIntToF32(integerPart, 0);
#ifdef DEBUG_MODE
	printf("result: %08x\n", result);
#endif

	x = fixedSub32(x, F32_ONE);
#ifdef DEBUG_MODE
	printf("x: %08x\n", x);
#endif
	if(x > (F32_SQRT_2 >> 1)){
#ifdef DEBUG_MODE
		printf("x decimal part is bigger than sqrt(2)/2\n");
#endif
		result = fixedAdd32(result, (F32_ONE >> 1));
		x 	   = fixedSub32(x, (F32_SQRT_2 >> 1));
#ifdef DEBUG_MODE
		printf("x: %08x\n", x);
#endif
	}

	ufixed32_t acc1 = x;
	ufixed32_t acc2 = 1;
	 fixed32_t acc3 = 0;

#ifdef DEBUG_MODE
	double n = convertF32ToDouble(x);
	printf("n: %f\n", n);
#endif

	for(uint8_t i = 0; i < RECIPROCAL_F32_COUNT; i++){
		acc2 = ufixedMul32(acc2, x);
		acc3 = ufixedMul32(acc2, reciprocalF32[i]);
		if(i & 0x1){
			acc1 = ufixedSub32(acc1, acc3);
		} else {
			acc1 = ufixedAdd32(acc1, acc3);
		}
	}

	fixed32_t decimalPart = ufixedMul32(acc1, F32_INV_LN_2);
	result = fixedAdd32(result, decimalPart);
	return result;
}

fixed64_t abstractLog264(ufixed64_t x){
	uint8_t rawShift = getBiggestOne64(x);
	int16_t integerPart = (F64_SIZE >> 1) - rawShift - 1;
	
	if(integerPart > 0){
		x = x >> integerPart;
	} else if (integerPart < 0){
		x = x << (-1 * integerPart);
	}
#ifdef DEBUG_MODE
	printf("x: %016lx\n", x);
#endif

#ifdef DEBUG_MODE
	printf("shift: %i\tint: %i\n", rawShift, integerPart);
#endif
	fixed64_t result = convertIntToF64(integerPart, 0);
#ifdef DEBUG_MODE
	printf("result: %016lx\n", result);
#endif

	x = fixedSub64(x, F64_ONE);
#ifdef DEBUG_MODE
	printf("x: %016lx\n", x);
#endif
	if(x > (F64_SQRT_2 >> 1)){
#ifdef DEBUG_MODE
		printf("x decimal part is bigger than sqrt(2)/2\n");
#endif
		result = fixedAdd64(result, (F64_ONE >> 1));
		x 	   = fixedSub64(x, (F64_SQRT_2 >> 1));
#ifdef DEBUG_MODE
		printf("x: %016lx\n", x);
#endif
	}

	ufixed64_t acc1 = x;
	ufixed64_t acc2 = 1;
	 fixed64_t acc3 = 0;

#ifdef DEBUG_MODE
	double n = convertF64ToDouble(x);
	printf("n: %f\n", n);
#endif

	for(uint8_t i = 0; i < RECIPROCAL_F64_COUNT; i++){
		acc2 = ufixedMul64(acc2, x);
		acc3 = ufixedMul64(acc2, reciprocalF64[i]);
		if(i & 0x1){
			acc1 = ufixedSub64(acc1, acc3);
		} else {
			acc1 = ufixedAdd64(acc1, acc3);
		}
	}

	fixed64_t decimalPart = ufixedMul64(acc1, F64_INV_LN_2);
	result = fixedAdd64(result, decimalPart);
	return result;
}

fixed32_t newtonIterLog232(ufixed32_t x, fixed32_t y){
	/*
		y_n+1 = y_n - (1 / ln(2)) * (1 - x * 2^-y)

		y_n+1 = z;
		y_n   = y
		2^-x  = exp(-x * ln(2))
	*/

	fixed32_t z;

	// -y
	z = fixedNeg32(y);
	// -y * ln(2)
	z = fixedMul32(z, F32_LN_2);
	// exp^(-y * ln(2))
	z = fixedExp32(z);
	// x * exp...
	z = fixedMul32(z, x);
	// 1 - x * exp...
	z = F32_ONE - z;
	// (1/ln(2)) * (1 - x * exp...)
	z = fixedMul32(z, F32_INV_LN_2);
	// y - (1/ln(2)) * (1 - x * exp...)
	z = y - z;

	return z;
}

fixed32_t abstractLog232_new(ufixed32_t x){
	if(x == F32_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return F32_ZERO;
	}

	int8_t exponent;
	ufixed32_t mantissa;
	ufixedFrexp32(x, &exponent, &mantissa);

	fixed32_t y = mantissa;

	if(mantissa < F32_SQRT_2){
		// Sector A
		// y = (x - 1) * (0.5)/(sqrt(2) - 1)

		// x - 1
		y = y - F32_ONE;

		// 0.5/(sqrt(2) - 1) = 1.2071067811865
		// 309/256 = 1.20703125
		y = (y << 8) + (y << 5) + (y << 4) + (y << 2) + y;
		y = y >> 8;
	} else {
		// Sector B
		// y = 0.5 + (x - sqrt(2)) * (1 - 0.5)/(2 - sqrt(2))
		
		// x - sqrt(2)
		y = y - F32_SQRT_2;

		// (1 - 0.5)/(2 - sqrt(2)) = 0.8535533906
		// 437/512 = 0.53515625
		y = (y << 8) + (y << 7) + (y << 5) + (y << 4) + (y << 2) + y;
		y = y >> 9;

		// 0.5 + ...
		y = y + (F32_ONE >> 1);
	}
	
	y = y + convertIntToF32(exponent, 0);

	y = newtonIterLog232(x, y);

	return y;
}

fixed64_t newtonIterLog264(ufixed64_t x, fixed64_t y){
	/*
		y_n+1 = y_n - (1 / ln(2)) * (1 - x * 2^-y)

		y_n+1 = z;
		y_n   = y
		2^-x  = exp(-x * ln(2))
	*/

	fixed64_t z;

	// -y
	z = fixedNeg64(y);
	// -y * ln(2)
	z = fixedMul64(z, F64_LN_2);
	// exp^(-y * ln(2))
	z = fixedExp64(z);
	// x * exp...
	z = fixedMul64(z, x);
	// 1 - x * exp...
	z = F64_ONE - z;
	// (1/ln(2)) * (1 - x * exp...)
	z = fixedMul64(z, F64_INV_LN_2);
	// y - (1/ln(2)) * (1 - x * exp...)
	z = y - z;

	return z;
}

// Average: e_r = 1.63e-3
fixed64_t abstractLog264_new(ufixed64_t x){
	if(x == F64_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return F64_ZERO;
	}

	int8_t exponent;
	ufixed64_t mantissa;
	ufixedFrexp64(x, &exponent, &mantissa);

	fixed64_t y = mantissa;

	if(mantissa < F64_SQRT_2){
		// Sector A
		// y = (x - 1) * (0.5)/(sqrt(2) - 1)

		// x - 1
		y = y - F64_ONE;

		// 0.5/(sqrt(2) - 1) = 1.2071067811865
		// 309/256 = 1.20703125
		y = (y << 8) + (y << 5) + (y << 4) + (y << 2) + y;
		y = y >> 8;
	} else {
		// Sector B
		// y = 0.5 + (x - sqrt(2)) * (1 - 0.5)/(2 - sqrt(2))
		
		// x - sqrt(2)
		y = y - F64_SQRT_2;

		// (1 - 0.5)/(2 - sqrt(2)) = 0.8535533906
		// 437/512 = 0.53515625
		y = (y << 8) + (y << 7) + (y << 5) + (y << 4) + (y << 2) + y;
		y = y >> 9;

		// 0.5 + ...
		y = y + (F64_ONE >> 1);
	}
	
	y = y + convertIntToF64(exponent, 0);

	// Newton iter disabled for exponents lower than -30 and higher than 30 due to
	// loss of precision, overflow and underflow
	if(abs(exponent) < F64_LN_DISABLE_NEWTON){
		y = newtonIterLog264(x, y);
	}

	return y;
}

fixed32_t abstractLn32(ufixed32_t x){
	fixed32_t y = abstractLog232_new(x);
	y = fixedMul32(y, F32_LN_2);
	return y;
}

fixed32_t abstractLog1032(ufixed32_t x){
	fixed32_t y = abstractLog232_new(x);
	y = fixedMul32(y, F32_LOG_10_2);
	return y;
}

fixed64_t abstractLn64(ufixed64_t x){
	fixed64_t y = abstractLog264_new(x);
	y = fixedMul64(y, F64_LN_2);
	return y;
}

fixed64_t abstractLog1064(ufixed64_t x){
	fixed64_t y = abstractLog264_new(x);
	y = fixedMul64(y, F64_LOG_10_2);
	return y;
}

fixed32_t fixedLn32(fixed32_t x){
	if(x <= 0){
		fixedSetErrno(FP_ERROR_NEGATIVE);
		return 0;
	}
	
	return abstractLn32(x);
}

ufixed32_t ufixedLn32(ufixed32_t x){
	return abstractLn32(x);
}

fixed64_t fixedLn64(fixed64_t x){
	if(x <= 0){
		fixedSetErrno(FP_ERROR_NEGATIVE);
		return 0;
	}
	
	return abstractLn64(x);
}

ufixed64_t ufixedLn64(ufixed64_t x){
	return abstractLn64(x);
}

fixed32_t fixedLog232(fixed32_t x){
	if(x <= 0){
		fixedSetErrno(FP_ERROR_NEGATIVE);
		return 0;
	}
	
	return abstractLog232_new(x);
}

ufixed32_t ufixedLog232(ufixed32_t x){
	return abstractLog232_new(x);
}

fixed64_t fixedLog264(fixed64_t x){
	if(x <= 0){
		fixedSetErrno(FP_ERROR_NEGATIVE);
		return 0;
	}
	
	return abstractLog264_new(x);
}

ufixed64_t ufixedLog264(ufixed64_t x){
	return abstractLog264_new(x);
}

fixed32_t fixedLog1032(fixed32_t x){
	if(x <= 0){
		fixedSetErrno(FP_ERROR_NEGATIVE);
		return 0;
	}
	
	return abstractLog1032(x);
}

ufixed32_t ufixedLog1032(ufixed32_t x){
	return abstractLog1032(x);
}

fixed64_t fixedLog1064(fixed64_t x){
	if(x <= 0){
		fixedSetErrno(FP_ERROR_NEGATIVE);
		return 0;
	}
	
	return abstractLog1064(x);
}

ufixed64_t ufixedLog1064(ufixed64_t x){
	return abstractLog1064(x);
}
