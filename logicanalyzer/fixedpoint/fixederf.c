#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>


// -1.31636270795256
#define F32_ERF_CROSS_A_B 0xfffeaf03
#define F64_ERF_CROSS_A_B 0xfffffffeaf02da84
//  1.31636270795256
#define F32_ERF_CROSS_B_C 0x000150fd
#define F64_ERF_CROSS_B_C 0x0000000150fd257c

// int from -int to -1.316... e^(3.5 * (x + 5/6)) = 0.05268809345
#define F32_ERF_SECTOR_A_AREA 0x00000d7d
#define F64_ERF_SECTOR_A_AREA 0x000000000d7cf786
// int from 1.316... to inf  e^(-3.5 * (x - 5/6)) = 0.05268809345
#define F32_ERF_SECTOR_C_AREA 0x00000d7d
#define F64_ERF_SECTOR_C_AREA 0x000000000d7cf786

// int from -1.316... to 1.316.. sectorB = 0.8313825434 - (-0.8313825434)
// = 1.1662765087
#define F32_ERF_INTEGRAL_LOWER 0xffff2b2b
#define F64_ERF_INTEGRAL_LOWER 0xffffffff2b2a837e
#define F32_ERF_INTEGRAL_UPPER 0x0000d4d5
#define F64_ERF_INTEGRAL_UPPER 0x00000000d4d57c82

// 5/6 = 0.8333333333
#define F32_ERF_CONST_56 0X0000d555
#define F64_ERF_CONST_56 0X00000000d5555555

// 1/sqrt(pi) = 1.772453851
#define F32_ERF_RSQRT_PI 0x0000906f
#define F64_ERF_RSQRT_PI 0x00000000906eba82

fixed32_t fixedErf32(fixed32_t x){
	/*
		Sector A:	]-inf, -1.
			f(x) = e^(3.5 * (x + 5/6))
			F(x) = e^(3.5 * (x + 5/6)) / 
		Sector B: [-1.4, 1.4]
			f(x) = 1 - x^2   + x^4/2  - x^6/6  + x^8/24  - x^10/120  + x^12/720
			F(x) = x - x^3/3 + x^5/10 - x^7/42 + x^9/216 - x^11/1320 + x^13/9360

		Sector A:	]1.4, inf[
			f(x) =  e^(-3.5 * (x - 5/6))
			F(x) = -e^(-3.5 * (x - 5/6)) / 3.5
	*/

	fixed32_t y = 0;

	// 3.5 = 4 - 0.5
	const fixed32_t normal35 = (F32_ONE << 2) - (F32_ONE >> 1);

	// 1/3.5
	const fixed32_t recp35 = fixedRecp32(normal35);

	fixed32_t exponent;
	fixed32_t temp;
	fixed32_t x2 = fixedMul32(x, x);
	fixed32_t reciprocal;

	if(x < (int32_t) F32_ERF_CROSS_A_B){
		// x + 5/6
		exponent = x + F32_ERF_CONST_56;
		// e^(3.5 * (x + 5/6))
		temp = fixedExp32(fixedLShift32(exponent, 2) - fixedRShift32(exponent, 1));
		// temp / 3.5
		temp = fixedMul32(temp,recp35);
		y   += temp;
		goto normalization;
	} else {
		y += F32_ERF_SECTOR_A_AREA;
	}
	
	if(x < (int32_t) F32_ERF_CROSS_B_C){
		y -= F32_ERF_INTEGRAL_LOWER;

		// x
		exponent    = x;									// x
		y 		   += x;

		// x^3/3
		exponent    = fixedMul32(exponent, x2);				// x^3
		reciprocal  = (F32_ONE << 1) + F32_ONE;				// 3
		reciprocal  = fixedRecp32(reciprocal);				// 1/3
		y          -= fixedMul32(exponent, reciprocal);		

		// x^5/10
		exponent    = fixedMul32(exponent, x2);				// x^5
		reciprocal  = (F32_ONE << 3) + (F32_ONE << 1);		// 10
		reciprocal  = fixedRecp32(reciprocal);				// 1/10
		y          += fixedMul32(exponent, reciprocal);		

		// x^7/42
		exponent    = fixedMul32(exponent, x2);				// x^7
		reciprocal  = (F32_ONE << 5);
		reciprocal += (F32_ONE << 3) + (F32_ONE << 1);		// 42
		reciprocal  = fixedRecp32(reciprocal);				// 1/42
		y          -= fixedMul32(exponent, reciprocal);		

		// x^9/216
		exponent    = fixedMul32(exponent, x2);				// x^9
		reciprocal  = (F32_ONE << 7) + (F32_ONE << 6);
		reciprocal += (F32_ONE << 4) + (F32_ONE << 3);		// 216
		reciprocal  = fixedRecp32(reciprocal);				// 1/216
		y          += fixedMul32(exponent, reciprocal);		

		// x^11/1320
		exponent    = fixedMul32(exponent, x2);				// x^11
		reciprocal  = (F32_ONE << 10) + (F32_ONE << 8);
		reciprocal += (F32_ONE << 5)  + (F32_ONE << 3);		// 1320
		reciprocal  = fixedRecp32(reciprocal);				// 1/1320
		y          -= fixedMul32(exponent, reciprocal);		

		// x^13/9360
		exponent    = fixedMul32(exponent, x2);				// x^13
		reciprocal  = (F32_ONE << 13) + (F32_ONE << 10);
		reciprocal += (F32_ONE <<  7) + (F32_ONE <<  4);	// 9360
		reciprocal  = fixedRecp32(reciprocal);				// 1/9360
		y          += fixedMul32(exponent, reciprocal);		

		goto normalization;
	} else {
		y += (F32_ERF_INTEGRAL_UPPER - F32_ERF_INTEGRAL_LOWER);
	}

	y += F32_ERF_SECTOR_C_AREA;

	// x - 5/6
	exponent = x - F32_ERF_CONST_56;
	// -(x - 5/6)
	exponent = fixedNeg32(exponent);
	// e^(-3.5 * (x - 5/6))
	temp = fixedExp32(fixedLShift32(exponent, 2) - fixedRShift32(exponent, 1));
	// temp / 3.5
	temp = fixedMul32(temp,recp35);
	y   -= temp;
	goto normalization;

normalization:
	// y = y * 1/sqrt(pi)
	y = fixedMul32(y, F32_ERF_RSQRT_PI);

	// y = y * 2;
	y = y << 1;

	// y = y - 1
	y = y - F32_ONE;

	return y;
}

ufixed32_t ufixedErf32(ufixed32_t x){
	return fixedErf32(x);
}

fixed64_t fixedErf64(fixed64_t x){
	/*
		Sector A:	]-inf, -1.
			f(x) = e^(3.5 * (x + 5/6))
			F(x) = e^(3.5 * (x + 5/6)) / 
		Sector B: [-1.4, 1.4]
			f(x) = 1 - x^2   + x^4/2  - x^6/6  + x^8/24  - x^10/120  + x^12/720
			F(x) = x - x^3/3 + x^5/10 - x^7/42 + x^9/216 - x^11/1320 + x^13/9360

		Sector A:	]1.4, inf[
			f(x) =  e^(-3.5 * (x - 5/6))
			F(x) = -e^(-3.5 * (x - 5/6)) / 3.5
	*/

	fixed64_t y = 0;

	// 3.5 = 4 - 0.5
	const fixed64_t normal35 = (F64_ONE << 2) - (F64_ONE >> 1);

	// 1/3.5
	const fixed64_t recp35 = fixedRecp64(normal35);

	fixed64_t exponent;
	fixed64_t temp;
	fixed64_t x2 = fixedMul64(x, x);
	fixed64_t reciprocal;

	if(x < (int64_t) F64_ERF_CROSS_A_B){
		// x + 5/6
		exponent = x + F64_ERF_CONST_56;
		// e^(3.5 * (x + 5/6))
		temp = fixedExp64(fixedLShift64(exponent, 2) - fixedRShift64(exponent, 1));
		// temp / 3.5
		temp = fixedMul64(temp,recp35);
		y   += temp;
		goto normalization;
	} else {
		y += F64_ERF_SECTOR_A_AREA;
	}
	
	if(x < (int64_t) F64_ERF_CROSS_B_C){
		y -= F64_ERF_INTEGRAL_LOWER;

		// x
		exponent    = x;									// x
		y 		   += x;

		// x^3/3
		exponent    = fixedMul64(exponent, x2);				// x^3
		reciprocal  = (F64_ONE << 1) + F64_ONE;				// 3
		reciprocal  = fixedRecp64(reciprocal);				// 1/3
		y          -= fixedMul64(exponent, reciprocal);		

		// x^5/10
		exponent    = fixedMul64(exponent, x2);				// x^5
		reciprocal  = (F64_ONE << 3) + (F64_ONE << 1);		// 10
		reciprocal  = fixedRecp64(reciprocal);				// 1/10
		y          += fixedMul64(exponent, reciprocal);		

		// x^7/42
		exponent    = fixedMul64(exponent, x2);				// x^7
		reciprocal  = (F64_ONE << 5);
		reciprocal += (F64_ONE << 3) + (F64_ONE << 1);		// 42
		reciprocal  = fixedRecp64(reciprocal);				// 1/42
		y          -= fixedMul64(exponent, reciprocal);		

		// x^9/216
		exponent    = fixedMul64(exponent, x2);				// x^9
		reciprocal  = (F64_ONE << 7) + (F64_ONE << 6);
		reciprocal += (F64_ONE << 4) + (F64_ONE << 3);		// 216
		reciprocal  = fixedRecp64(reciprocal);				// 1/216
		y          += fixedMul64(exponent, reciprocal);		

		// x^11/1320
		exponent    = fixedMul64(exponent, x2);				// x^11
		reciprocal  = (F64_ONE << 10) + (F64_ONE << 8);
		reciprocal += (F64_ONE << 5)  + (F64_ONE << 3);		// 1320
		reciprocal  = fixedRecp64(reciprocal);				// 1/1320
		y          -= fixedMul64(exponent, reciprocal);		

		// x^13/9360
		exponent    = fixedMul64(exponent, x2);				// x^13
		reciprocal  = (F64_ONE << 13) + (F64_ONE << 10);
		reciprocal += (F64_ONE <<  7) + (F64_ONE <<  4);	// 9360
		reciprocal  = fixedRecp64(reciprocal);				// 1/9360
		y          += fixedMul64(exponent, reciprocal);		

		goto normalization;
	} else {
		y += (F64_ERF_INTEGRAL_UPPER - F64_ERF_INTEGRAL_LOWER);
	}

	y += F64_ERF_SECTOR_C_AREA;

	// x - 5/6
	exponent = x - F64_ERF_CONST_56;
	// -(x - 5/6)
	exponent = fixedNeg64(exponent);
	// e^(-3.5 * (x - 5/6))
	temp = fixedExp64(fixedLShift64(exponent, 2) - fixedRShift64(exponent, 1));
	// temp / 3.5
	temp = fixedMul64(temp,recp35);
	y   -= temp;
	goto normalization;

normalization:
	// y = y * 1/sqrt(pi)
	y = fixedMul64(y, F64_ERF_RSQRT_PI);

	// y = y * 2;
	y = y << 1;

	// y = y - 1
	y = y - F64_ONE;

	return y;
}

ufixed64_t ufixedErf64(ufixed64_t x){
	return fixedErf64(x);
}

// Error Function Complementary

fixed32_t fixedErfc32(fixed32_t x){
	return F32_ONE - fixedErf32(x);
}

ufixed32_t ufixedErfc32(ufixed32_t x){
	return F32_ONE - ufixedErf32(x);
}

fixed64_t fixedErfc64(fixed64_t x){
	return F64_ONE - fixedErf64(x);
}

ufixed64_t ufixedErfc64(ufixed64_t x){
	return F64_ONE - ufixedErf64(x);
}

// Q function

fixed32_t fixedQFunc32(fixed32_t x){
	// x / sqrt(2)
	x = fixedMul32(x, F32_SQRT_2 >> 1);

	fixed32_t y = fixedErfc32(x);

	// 1/2 * erfc(x / sqrt(2))
	return fixedRShift32(y, 1);
}

ufixed32_t ufixedQFunc32(ufixed32_t x){
	return fixedQFunc32(x);
}

fixed64_t fixedQFunc64(fixed64_t x){
	// x / sqrt(2)
	x = fixedMul64(x, F64_SQRT_2 >> 1);

	fixed64_t y = fixedErfc64(x);

	// 1/2 * erfc(x / sqrt(2))
	return fixedRShift64(y, 1);
}

ufixed64_t ufixedQFunc64(ufixed64_t x){
	return fixedQFunc64(x);
}
