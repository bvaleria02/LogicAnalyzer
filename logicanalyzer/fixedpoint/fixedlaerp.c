#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define F32_RECIPROCAL_6 0x00002aab
#define F64_RECIPROCAL_6 0x000000002aaaaaaa

/*
	Based on LAgrange intERPolation:

	A = x0, y0	-> x0 = -1
	B = x1, y1	-> x1 =  0 ----
	C = x2, y2	-> x2 =  1 ----
	D = x3, y3  -> x3 =  2

	f_A(x) = y0 * ((x-x_1)*(x-x_2)*(x-x_3)) / ((x0-x1)*(x0-x2)*(x0-x3))
	f_B(x) = y1 * ((x-x_0)*(x-x_2)*(x-x_3)) / ((x1-x0)*(x1-x2)*(x1-x3))
	f_C(x) = y2 * ((x-x_0)*(x-x_1)*(x-x_3)) / ((x2-x0)*(x2-x1)*(x2-x3))
	f_D(x) = y3 * ((x-x_0)*(x-x_1)*(x-x_2)) / ((x3-x0)*(x3-x1)*(x3-x2))

	Now we replace x0=-1, x1=0, x2=1 and x3=2

	f_A(x) = y0 * ((x)  *(x-1)*(x-2)) / ((-1)*(-2)*(-3))
	f_B(x) = y1 * ((x+1)*(x-1)*(x-2)) / ((1) *(-1)*(-2))
	f_C(x) = y2 * ((x+1)*(x)  *(x-2)) / ((2) *( 1)*(-1))
	f_D(x) = y3 * ((x+1)*(x)  *(x-1)) / ((3) *( 2)* (1))

	Now, simplify the denominators:

	f_A(x) = y0 * ((x)  *(x-1)*(x-2)) / (-6)
	f_B(x) = y1 * ((x+1)*(x-1)*(x-2)) / ( 2)
	f_C(x) = y2 * ((x+1)*(x)  *(x-2)) / (-2)
	f_D(x) = y3 * ((x+1)*(x)  *(x-1)) / ( 6)

	Simplify the denominator:
		
	f_A(x) = y0 * (x^3 - 3x^2 + 2x    ) / (-6)
	f_B(x) = y1 * (x^3 - 2x^2 -  x + 2) / ( 2)
	f_C(x) = y2 * (x^3 -  x^2 - 2x    ) / (-2)
	f_D(x) = y3 * (x^3        -  x    ) / ( 6)

	Now, merge the denominator with the numerator:

	f_A(x) = -(y0/6) * (x^3 - 3x^2 + 2x    )
	f_B(x) =  (y1/2) * (x^3 - 2x^2 -  x + 2) 
	f_C(x) = -(y2/2) * (x^3 -  x^2 - 2x    ) 
	f_D(x) =  (y3/6) * (x^3        -  x    )

	The final interpolation formula is f(x) = f_A(x) + f_B(x) + f_C(x) + f_D(x)
	We group by powers of x

	f(x) = x^3*(-y0/6 + y1/2 - y2/2 + y3/6) + x^2*(3y0/6 - 2y1/2 + y2/2)
	       + x*(-2y0/6 - y1/2 + 2y2/2 - y3/6) + 2y1/2

	We bring the division outside the parenthesis, and amplify up 6

	f(x) = x^3/6*(-y0 + 3y1 - 3y2 + y3) + x^2/6*(3y0 - 6y1 + 3y2)
		       + x/6*(-2y0 - 3y1 + 6y2 - y3) + y1

	Now, group everything and divide by 6

	f(x) = (x^3*(-y0 + 3y1 - 3y2 + y3) + x^2*(3y0 - 6y1 + 3y2)
		       + x*(-2y0 - 3y1 + 6y2 - y3) + 6*y1) / 6
		
	This is the final expresion for our interpolation. Now we're going to create
	intermediary 'p' steps, for better clarity:

	p3 = -y0 + 3y1 - 3y2 + y3
	p2 = 3y0 - 6y1 + 3y2
	p1 = -2y0 - 3y1 + 6y2 - y3
	p0 = 6*y1

	f(x) = (p0 + p1*x + p2*x^2 + p3*x^3)/6

	And finally, to avoid overflow, y0, y1, y2 and y3 are divided by 6, so:
	f(x) = p0' + p1'*x + p2'*x^2 + p3'*x^3
*/

fixed32_t fixedLaerp32(fixed32_t y0, fixed32_t y1, fixed32_t y2, fixed32_t y3, ufixed32_t x){
	if(fixedAbs32(x) > F32_ONE){
		fixedSetErrno(FP_ERROR_OUTOFRANGE);
	}

	y0 = fixedMul32(y0, F32_RECIPROCAL_6);
	y1 = fixedMul32(y1, F32_RECIPROCAL_6);
	y2 = fixedMul32(y2, F32_RECIPROCAL_6);
	y3 = fixedMul32(y3, F32_RECIPROCAL_6);

	// p0 = y1*4 + y1*2
	fixed32_t p0 = fixedLShift32(y1, 2) + fixedLShift32(y1, 1);

	// p1 = -2y0 - (3y1) + 2 * 3y2 - y3
	fixed32_t p1 = -y0 -y0 - (y1 + y1 + y1) + fixedLShift32(y2 + y2 + y2, 1) - y3;

	// p2 = y0 - 2y1 + y2
	fixed32_t p2 = y0 - y1 - y1 + y2;
	// p2 = p2*2 + p2
	p2 = fixedLShift32(p2, 1) + p2;

	// p3 = y1 - y2
	fixed32_t p3 = y1 - y2;
	// p3 = -y0 + (2*p3 + p3) + y3
	p3 = -y0  + (fixedLShift32(p3, 1) + p3) + y3;
	
	ufixed32_t x2 = ufixedMul32(x, x);
	ufixed32_t x3 = ufixedMul32(x, x2);

	fixed32_t y;
	y = p0 + fixedMul32(p1, x) + fixedMul32(p2, x2) + fixedMul32(p3, x3);

	return y;
}

ufixed32_t ufixedLaerp32(ufixed32_t y0, ufixed32_t y1, ufixed32_t y2, ufixed32_t y3, ufixed32_t x){
	return fixedLaerp32(y0, y1, y2, y3, x);
}

fixed64_t fixedLaerp64(fixed64_t y0, fixed64_t y1, fixed64_t y2, fixed64_t y3, ufixed64_t x){
	if(fixedAbs64(x) > F64_ONE){
		fixedSetErrno(FP_ERROR_OUTOFRANGE);
	}

	y0 = fixedMul64(y0, F64_RECIPROCAL_6);
	y1 = fixedMul64(y1, F64_RECIPROCAL_6);
	y2 = fixedMul64(y2, F64_RECIPROCAL_6);
	y3 = fixedMul64(y3, F64_RECIPROCAL_6);

	// p0 = y1*4 + y1*2
	fixed64_t p0 = fixedLShift64(y1, 2) + fixedLShift64(y1, 1);

	// p1 = -2y0 - (3y1) + 2 * 3y2 - y3
	fixed64_t p1 = -y0 -y0 - (y1 + y1 + y1) + fixedLShift64(y2 + y2 + y2, 1) - y3;

	// p2 = y0 - 2y1 + y2
	fixed64_t p2 = y0 - y1 - y1 + y2;
	// p2 = p2*2 + p2
	p2 = fixedLShift64(p2, 1) + p2;

	// p3 = y1 - y2
	fixed64_t p3 = y1 - y2;
	// p3 = -y0 + (2*p3 + p3) + y3
	p3 = -y0  + (fixedLShift64(p3, 1) + p3) + y3;
	
	ufixed64_t x2 = ufixedMul64(x, x);
	ufixed64_t x3 = ufixedMul64(x, x2);

	fixed64_t y;
	y = p0 + fixedMul64(p1, x) + fixedMul64(p2, x2) + fixedMul64(p3, x3);

	return y;
}

ufixed64_t ufixedLaerp64(ufixed64_t y0, ufixed64_t y1, ufixed64_t y2, ufixed64_t y3, ufixed64_t x){
	return fixedLaerp64(y0, y1, y2, y3, x);
}
