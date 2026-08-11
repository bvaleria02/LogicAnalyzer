#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

fixed32_t abstractCubicSplineInterpolation32(fixed32_t y0, fixed32_t y1, fixed32_t y2, fixed32_t y3, ufixed32_t x){
	/*
		Catmull-Rom based cubic spline interpolation

		A = (n-1, y0)
		B = (n,   y1)
		C = (n+1, y2)
		D = (n+2, y3)

		x ∈ [0, 1]

		Formula:
		y = 0.5 * (2y1 + (-y0 + y2)*x + (2y0 - 5y1 + 4y2 - y3)*x^2 + (-y0 + 3y1 - 3y2 +y2)*x^3)
	*/

	if(fixedAbs32(x) > F32_ONE){
		fixedSetErrno(FP_ERROR_OUTOFRANGE);
	}

	fixed32_t x2 = fixedMul32(x,  x);
	fixed32_t x3 = fixedMul32(x2, x);
	
	// 2*y1
	fixed32_t p0 = y1 << 1;			// 2*y1

	// (-y0 + y2)
	fixed32_t p1 = y2 -y0;			// -y0 + y2

	// (2*y0 - 5*y1 + 4*y2 - y3)
	fixed32_t p2 = (y0 << 1);		// 2*y0
	p2 -= ((y1 << 2) + y1);			// 5*y1
	p2 += (y2 << 2);				// 4*y2
	p2 -=  y3;						// y3

	// -y0 + 3*y1 - 3*y2 + y3
	fixed32_t p3 = fixedNeg32(y0);	// -y0
	p3 += (y1 << 1) + y1;			// 3*y1
	p3 -= (y2 << 1) + y2;			// 3*y2
	p3 += y3;						// y3

	fixed32_t result = p0;
	result += fixedMul32(p1, x );
	result += fixedMul32(p2, x2);
	result += fixedMul32(p3, x3);


	// Absolute value
	uint8_t sign = 0;
	if(result < F32_ZERO){
		sign = 1;
		result = fixedNeg32(result);
	}

	// result * 0.5
	result = result >> 1;

	// Sign correction
	if(sign){
		result = fixedNeg32(result);
	}

	return result;
}

fixed64_t abstractCubicSplineInterpolation64(fixed64_t y0, fixed64_t y1, fixed64_t y2, fixed64_t y3, ufixed64_t x){
	/*
		Catmull-Rom based cubic spline interpolation

		A = (n-1, y0)
		B = (n,   y1)
		C = (n+1, y2)
		D = (n+2, y3)

		x ∈ [0, 1]

		Formula:
		y = 0.5 * (2y1 + (-y0 + y2)*x + (2y0 - 5y1 + 4y2 - y3)*x^2 + (-y0 + 3y1 - 3y2 +y2)*x^3)
	*/

	fixed64_t x2 = ufixedMul64(x,  x);
	fixed64_t x3 = ufixedMul64(x2, x);
	
	// 2*y1
	fixed64_t p0 = y1 << 1;			// 2*y1

	// (-y0 + y2)
	fixed64_t p1 = y2 -y0;			// -y0 + y2

	// (2*y0 - 5*y1 + 4*y2 - y3)
	fixed64_t p2 = (y0 << 1);		// 2*y0
	p2 -= ((y1 << 2) + y1);			// 5*y1
	p2 += (y2 << 2);				// 4*y2
	p2 -=  y3;						// y3

	// -y0 + 3*y1 - 3*y2 + y3
	fixed64_t p3 = fixedNeg32(y0);	// -y0
	p3 += (y1 << 1) + y1;			// 3*y1
	p3 -= (y2 << 1) + y2;			// 3*y2
	p3 += y3;						// y3

	fixed64_t result = p0;
	result += ufixedMul32(p1, x );
	result += ufixedMul32(p2, x2);
	result += ufixedMul32(p3, x3);


	// Absolute value
	uint8_t sign = 0;
	if(result < F64_ZERO){
		sign = 1;
		result = fixedNeg64(result);
	}

	// result * 0.5
	result = result >> 1;

	// Sign correction
	if(sign){
		result = fixedNeg64(result);
	}

	return result;
}

fixed32_t fixedCSerp32(fixed32_t y0, fixed32_t y1, fixed32_t y2, fixed32_t y3, ufixed32_t x){
	return abstractCubicSplineInterpolation32(y0, y1,y2, y3, x);
}

ufixed32_t ufixedCSerp32(ufixed32_t y0, ufixed32_t y1, ufixed32_t y2, ufixed32_t y3, ufixed32_t x){
	return abstractCubicSplineInterpolation32(y0, y1,y2, y3, x);
}

fixed64_t fixedCSerp64(fixed64_t y0, fixed64_t y1, fixed64_t y2, fixed64_t y3, ufixed64_t x){
	return abstractCubicSplineInterpolation64(y0, y1,y2, y3, x);
}

ufixed64_t ufixedCSerp64(ufixed64_t y0, ufixed64_t y1, ufixed64_t y2, ufixed64_t y3, ufixed64_t x){
	return abstractCubicSplineInterpolation64(y0, y1,y2, y3, x);
}

