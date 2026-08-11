#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

fixed32_t abstractLerp32(fixed32_t y0, fixed32_t y1, ufixed32_t x){
	/*
		Lerp: Linear Interpolation for q16.16
		x must be between 0.0 and 1.0

		y = y0 + (x - x0) * (y1 - y0) / (x1 - x0)
		x0 = 0,		x1 = 1

		y = y0 + x * (y1 - y0)
	*/

	if(x > F32_ONE || (int32_t) x < F32_ZERO){
		fixedSetErrno(FP_ERROR_OUTOFRANGE);
	}

	fixed32_t numerator;
	fixed32_t y;

	numerator = y1 - y0;
	y = fixedMul32(numerator, x);
	y = y0 + y;
	return y;
}

fixed64_t abstractLerp64(fixed64_t y0, fixed64_t y1, ufixed64_t x){
	/*
		Lerp: Linear Interpolation for q32.32
		x must be between 0.0 and 1.0

		y = y0 + (x - x0) * (y1 - y0) / (x1 - x0)
		x0 = 0,		x1 = 1

		y = y0 + x * (y1 - y0)
	*/

	if(x > F64_ONE || (int64_t) x < F64_ZERO){
		fixedSetErrno(FP_ERROR_OUTOFRANGE);
	}

	fixed64_t numerator;
	fixed64_t y;

	numerator = y1 - y0;
	y = ufixedMul64(numerator, x);
	y = y0 + y;
	return y;
}

fixed32_t fixedLerp32(fixed32_t y0, fixed32_t y1, ufixed32_t x){
	return abstractLerp32(y0, y1, x);
}

ufixed32_t ufixedLerp32(ufixed32_t y0, ufixed32_t y1, ufixed32_t x){
	return abstractLerp32(y0, y1, x);
}

fixed64_t fixedLerp64(fixed64_t y0, fixed64_t y1, ufixed64_t x){
	return abstractLerp64(y0, y1, x);
}

ufixed64_t ufixedLerp64(ufixed64_t y0, ufixed64_t y1, ufixed64_t x){
	return abstractLerp64(y0, y1, x);
}

