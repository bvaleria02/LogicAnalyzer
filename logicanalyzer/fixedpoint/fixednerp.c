#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*	
	NERP
	Based on Newton intERPolation

	A = x0, y0	-> x0 = -2
	B = x1, y1	-> x1 = -1
	C = x2, y2	-> x2 =  0  ----
	D = x3, y3  -> x3 =  1  ----
	C = x4, y4	-> x2 =  2
	D = x5, y5  -> x3 =  3

	f_0_1 = (y1- y0) / (x1 - x0)
	f_1_2 = (y2- y1) / (x2 - x1)
	f_2_3 = (y3- y2) / (x3 - x2)
	f_3_4 = (y4- y3) / (x4 - x3)
	f_4_5 = (y5- y4) / (x5 - x4)

	f_0_1_2 = (f_1_2 - f_0_1) / (x2 - x0)
	f_1_2_3 = (f_2_3 - f_1_2) / (x3 - x1)
	f_2_3_4 = (f_3_4 - f_2_3) / (x4 - x2)
	f_3_4_5 = (f_4_5 - f_3_4) / (x5 - x3)

	f_0_1_2_3 = (f_1_2_3 - f_0_1_2) / (x3 - x0)
	f_1_2_3_4 = (f_2_3_4 - f_1_2_3) / (x4 - x1)
	f_2_3_4_5 = (f_3_4_5 - f_2_3_4) / (x5 - x2)

	f_0_1_2_3_4 = (f_1_2_3_4 - f_0_1_2_3) / (x4 - x0)
	f_1_2_3_4_5 = (f_2_3_4_5 - f_1_2_3_4) / (x5 - x1)

	f_0_1_2_3_4_5 = (f_1_2_3_4_5 - f_0_1_2_3_4) / (x5 - x0)

	To avoid division (they are slow), lets precompute them:

	f_0_1 = (y1- y0) / (1)
	f_1_2 = (y2- y1) / (1)
	f_2_3 = (y3- y2) / (1)
	f_3_4 = (y4- y3) / (1)
	f_4_5 = (y5- y4) / (1)

	Let's also reduce the 3-term ones

	f_0_1_2 = ((y2-y1) - (y1-y0)) / (2)
	f_1_2_3 = ((y3-y2) - (y2-y1)) / (2)
	f_2_3_4 = ((y4-y3) - (y3-y2)) / (2)
	f_3_4_5 = ((y5-y4) - (y4-y3)) / (2)

	f_0_1_2 = (y0-2y1+y2) / (2)
	f_1_2_3 = (y1-2y2+y3) / (2)
	f_2_3_4 = (y2-2y3+y4) / (2)
	f_3_4_5 = (y3-2y4+y5) / (2)
	
	Now the 4-term ones

	f_0_1_2_3 = ((y1-2y2+y3)/2 - (y0-2y1+y2)/2) / (3)
	f_1_2_3_4 = ((y2-2y3+y4)/2 - (y1-2y2+y3)/2) / (3)
	f_2_3_4_5 = ((y3-2y4+y5)/2 - (y2-2y3+y4)/2) / (3)

	f_0_1_2_3 = (-y0+3y1-3y2+y3) / (6)
	f_1_2_3_4 = (-y1+3y2-3y3+y4) / (6)
	f_2_3_4_5 = (-y2+3y3-3y4+y5) / (6)

	Now the 5-term ones

	f_0_1_2_3_4 = ((y4-3y3+3y2-y1)/6 - (y3-3y2+3y1-y0)/6) / (4)
	f_1_2_3_4_5 = ((y5-3y4+3y3-y2)/6 - (y4-3y3+3y2-y1)/6) / (4)

	f_0_1_2_3_4 = (y4-4y3+6y2-4y1+y0) / (24)
	f_1_2_3_4_5 = (y5-4y4+6y3-4y2+y1) / (24)

	And the last one:

	f_0_1_2_3_4_5 = ((y5-4y4+6y3-4y2+y1)/24 - (y4-4y3+6y2-4y1+y0)/24) / (5)

	f_0_1_2_3_4_5 = (y5-5y4+10y3-10y2+5y1-y0) / (120)

	Our final expression uses the first term:

	f_0_1 			=                            y1 - y0
	f_0_1_2 		= (                    y2 - 2y1 + y0) / 2
	f_0_1_2_3 		= (             y3 -  3y2 + 3y1 - y0) / 6
	f_0_1_2_3_4 	= (      y4 -  4y3 +  6y2 - 4y1 + y0) / 24
	f_0_1_2_3_4_5 	= (y5 - 5y4 + 10y3 - 10y2 + 5y1 - y0) / 120

	Then:

	f(x) =    y0
	        + f_0_1         * (x - x0)
			+ f_0_1_2       * (x - x0)(x - x1)
			+ f_0_1_2_3     * (x - x0)(x - x1)(x - x2)
			+ f_0_1_2_3_4   * (x - x0)(x - x1)(x - x2)(x - x3)
			+ f_0_1_2_3_4_5 * (x - x0)(x - x1)(x - x2)(x - x3)(x - x4)
		 
	Now, let's replace the xX:

	f(x) =    y0
	        + f_0_1         * (x + 2)
			+ f_0_1_2       * (x + 2)(x + 1)
			+ f_0_1_2_3     * (x + 2)(x + 1)(x)
			+ f_0_1_2_3_4   * (x + 2)(x + 1)(x)(x - 1)
			+ f_0_1_2_3_4_5 * (x + 2)(x + 1)(x)(x - 1)(x - 2)

	f(x) = y0 + (x+2)*(f_0_1 + (x+1)*(f_0_1_2 +
				(x)*(f_0_1_2_3 + (x-1)*(f_0_1_2_3_4 +
				(x-2)*f_0_1_2_3_4_5))))

	f_0_1 			= p1 
	f_0_1_2 		= p2
	f_0_1_2_3 		= p3
	f_0_1_2_3_4 	= p4
	f_0_1_2_3_4_5 	= p5
*/

#define F32_RECIPROCAL_6 0x00002aaa
#define F64_RECIPROCAL_6 0x000000002aaaaaaa

#define F32_RECIPROCAL_120 0x00000222
#define F64_RECIPROCAL_120 0x0000000002222222

fixed32_t fixedNerp32(fixed32_t y0, fixed32_t y1, fixed32_t y2, fixed32_t y3, fixed32_t y4, fixed32_t y5, fixed32_t x){
	if(fixedAbs32(x) > (F32_ONE << 1)){
		fixedSetErrno(FP_ERROR_OUTOFRANGE);
	}

	fixed32_t p1 = y1 - y0;

	fixed32_t p2 = y2 - (y1+y1) + y0;

	fixed32_t p3 = (-y2+y1);
	// y3 + 3(-y2-y1) - y0
	p3 = y3 + (p3 + p3 + p3) -y0;

	// -y3 + (y2*1.5) - y1
	fixed32_t p4 = -y3 - y1 + (y2 + fixedRShift32(y2, 1));
	// y4 + 4*(-y3 + 1.5y2 - y1) + y0
	p4 = y4 + fixedLShift32(p4, 2) + y0;

	// -y4 + 2*(y3-y2) + y1
	fixed32_t p5 = -y4 + fixedLShift32(y3-y2, 2) + y1;
	// y5 + (1+4)*(-y4 + 2(y3-y2) + y1) - y0
	p5 = y5 + p5 + fixedLShift32(p5, 2) - y0;

	p1 = p1;									// p1/1
	p2 = fixedRShift32(p2, 1);					// p2/2
	p3 = fixedMul32(p3, F32_RECIPROCAL_6);		// p3/6
	p4 = fixedRShift32(p4, 2);					// p4/4
	p4 = fixedMul32(p4, F32_RECIPROCAL_6);		// (p4/4)/6 = p4/4
	p5 = fixedMul32(p5, F32_RECIPROCAL_120);	// p5/120

	// Result
	fixed32_t y;
	// y = (x-2)*p5
	y  = fixedMul32(p5,     x - (F32_ONE << 1));
	// y = (x-1)*(p4 + y)
	y  = fixedMul32(y + p4, x - F32_ONE       );
	// y = (x)*(p3 + y)
	y  = fixedMul32(y + p3, x 		          );
	// y = (x+1)*(p2 + y)
	y  = fixedMul32(y + p2, x + F32_ONE       );
	// y = (x+2)*(p1 + y)
	y  = fixedMul32(y + p1, x + (F32_ONE << 1));
	// y = y0 + y
	y += y0;

	return y;
}

ufixed32_t ufixedNerp32(ufixed32_t y0, ufixed32_t y1, ufixed32_t y2, ufixed32_t y3, ufixed32_t y4, ufixed32_t y5, ufixed32_t x){
	return fixedNerp32(y0, y1, y2, y3, y4, y5, x);
}

fixed64_t fixedNerp64(fixed64_t y0, fixed64_t y1, fixed64_t y2, fixed64_t y3, fixed64_t y4, fixed64_t y5, fixed64_t x){
	if(fixedAbs64(x) > (F64_ONE << 1)){
		fixedSetErrno(FP_ERROR_OUTOFRANGE);
	}

	fixed64_t p1 = y1 - y0;

	fixed64_t p2 = y2 - (y1+y1) + y0;

	fixed64_t p3 = (-y2+y1);
	// y3 + 3(-y2-y1) - y0
	p3 = y3 + (p3 + p3 + p3) -y0;

	// -y3 + (y2*1.5) - y1
	fixed64_t p4 = -y3 - y1 + (y2 + fixedRShift64(y2, 1));
	// y4 + 4*(-y3 + 1.5y2 - y1) + y0
	p4 = y4 + fixedLShift64(p4, 2) + y0;

	// -y4 + 2*(y3-y2) + y1
	fixed64_t p5 = -y4 + fixedLShift64(y3-y2, 2) + y1;
	// y5 + (1+4)*(-y4 + 2(y3-y2) + y1) - y0
	p5 = y5 + p5 + fixedLShift64(p5, 2) - y0;

	p1 = p1;									// p1/1
	p2 = fixedRShift64(p2, 1);					// p2/2
	p3 = fixedMul64(p3, F64_RECIPROCAL_6);		// p3/6
	p4 = fixedRShift64(p4, 2);					// p4/4
	p4 = fixedMul64(p4, F64_RECIPROCAL_6);		// (p4/4)/6 = p4/4
	p5 = fixedMul64(p5, F64_RECIPROCAL_120);	// p5/120

	// Result
	fixed64_t y;
	// y = (x-2)*p5
	y  = fixedMul64(p5,     x - (F64_ONE << 1));
	// y = (x-1)*(p4 + y)
	y  = fixedMul64(y + p4, x - F64_ONE       );
	// y = (x)*(p3 + y)
	y  = fixedMul64(y + p3, x 		          );
	// y = (x+1)*(p2 + y)
	y  = fixedMul64(y + p2, x + F64_ONE       );
	// y = (x+2)*(p1 + y)
	y  = fixedMul64(y + p1, x + (F64_ONE << 1));
	// y = y0 + y
	y += y0;

	return y;
}

ufixed64_t ufixedNerp64(ufixed64_t y0, ufixed64_t y1, ufixed64_t y2, ufixed64_t y3, ufixed64_t y4, ufixed64_t y5, ufixed64_t x){
	return fixedNerp64(y0, y1, y2, y3, y4, y5, x);
}
