#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "fixedpoint.h"
#include "utils/fixedlimits.h"

#define RSQRT_ITER 2 
#define RSQRT_EXP_NO_ITER_F32 14
#define RSQRT_EXP_NO_ITER_F64 30

/*
	"0x5F3759DF (like) magic number"

	In order to lower the relative error for the linealization of 1/sqrt(m),
	we introduce a "q" offset to the formula (g).

	Real formula:		f(x) 		= 1/sqrt(x)
	Linealized formula:	g_A(x, q) 	= 1         - (x-1)*(19/64)  - qA
						g_B(x, q) 	= sqrt(2)/2 - (x-2)*(13/128) - qB

	with q=0, the max relative error is 5e-2, but with the help of a graphic
	calculator, we saw that lowering "just a bit", reduces further down the
	error up to 3e-2

	But, how we calculate this value instead of just throw an approximation?

	Let us introduce the "squared relative error" (e_r2) formula:

	h(x,q) 	= ((f(x) - g(x, q)) / f(x))^2
			= (((1 / sqrt(x)) - (1 - (x-1)*(19/64) - qA)) / (1/sqrt(x)))^2
			= (((1 / sqrt(x)) - (sqrt(2)/2 - (x-2)*(13/128) - qB)) / (1/sqrt(x)))^2
			
	h_A(x,q)= (1 - sqrt(x) * (1 - (x-1)*(19/64) - qA))^2
	h_B(x,q)= (1 - sqrt(x) * (sqrt(2)/2 - (x-2)*(13/128) - qB))^2

	Now, let's use
			rA = 1 + 19/64 - qA		rB = sqrt(2)/2 + (2*13)/128 - qB

	h_A(x,r) 	= (1 - sqrt(x) * (r - 19x/64))^2
	h_B(x,r) 	= (1 - sqrt(x) * (r - 13x/128))^2

	then, we integrate h in x, between 1 and 2 for A, 2 to 4 for B to get the area under the curve
	(less area, less error). This gives us:
	(Tested by hand and Symbolab, feel free to test it with Wolfram or to correct any mistake)
	We ommited the terms without r:

	H_A(x,r) = -2(4sqrt(2)-2)r/3 + 3/2 * r^2 - 133r/96 + ...
	H_B(x,r) = -2(16 - 4sqrt(2))*r/3 + 6 r^2 - 91r/24 + ..

	We now differentiate it in r, to get the minimun value of r

	dH_A/dr = -2(4sqrt(2)-2)/3 + 3r - 133/96 + ...
	dH_B/dr = -2(16 - 4sqrt(2))/3 + 2*6r - 91/24 + ..
	
	r_A = 1.274439833
	r_B = 0.8905914306

	Now we transform back r to q
			rA = 1 + 19/64 - qA		rB = sqrt(2)/2 + (2*13)/128 - qB

	qA = 0.02243596878
	qB = 0.0196403506

	F32_RSQRT_CORRECTION and F64_RSQRT_CORRECTION are effectively qA and qB

	If we integrate h_A between 1 and 2 and add h_B between 2 and 4 with q,
	we get e_r2 = 6.255e-4
	Without correction, we get e_r2 = 3.34e-3
	so the squared relative error is "pretty low" and our correction lowers the relative error.
*/
#define F32_RSQRT_CORRECTION_A 0x000005be
#define F64_RSQRT_CORRECTION_A 0x0000000005be4fa3

#define F32_RSQRT_CORRECTION_B 0x00000507
#define F64_RSQRT_CORRECTION_B 0x0000000005072667

// legacy, to be removed -> 139/1440
#define F32_RSQRT_CORRECTION 0x000018b6
#define F64_RSQRT_CORRECTION 0x0000000018b60b60


ufixed32_t ufixedRSqrt32(ufixed32_t x){
	/*
		.This function calculates the 1/sqrt(x) (inverse square root, or reciprocal square root)
		of x for a q16.16 number in fixed point (ufixed32_t).

		This is useful to calculate the norm of vector for physics and lighting in 2d/3d spaces
		like simulators, videogames or in embedded machines like microcontrollers to control
		servos.
	*/

	// Special case, 0 or below. 1/sqrt(0) is not defined
	// neither 1/sqrt(x) where x is negative
	if(x <= F32_ZERO){
		fixedSetErrno(FP_ERROR_NEGATIVE);
		return F32_ZERO;
	 }

	fixed32_t x2 			= x >> 1;					// x2			= x/2
	fixed32_t y  			= F32_ZERO;
	fixed32_t threehalfs  	= F32_ONE + (F32_ONE >> 1);  // threehalfs	= 1.5 in fixed point

	/*
		"evil fixed point bit level hacking"

		Get the log_2 of x by taking the highest one with getBiggestOne32
		(that function is a CLZ for 32 bits)

		ex: 0b1... -> n =  0
			0b0..1 -> n = 31

		shift then converts the position from n to the exponent form of 2^shift
		shift = (32/2) - n - 1

		ex: 0b1... -> 32768.0 (in fixed point) = 2^15
					  n =  0
					  shift = 16 - 0 - 1 = 15

			0b...1 -> 1/65536 (in fixed point) = 2^-16
					  n = 31
					  shift = 16 - 31 - 1 = -16
	*/

	uint8_t n 		= getBiggestOne32(x);
	int8_t shift 	= (F32_SIZE >> 1) - n - 1;

#ifdef DEBUG_MODE
	printf("n: %i\ts: %i\n", n, shift);
	printf("x: %lf\n", convertF32ToDouble(x));
#endif

	/* 
		"what the fuck?"

		x is equivalent to 2^s * m, where:
			s is the exponent (given in "shift")
			m is a value between 1 and 2.

		Then, 1/sqrt(x) can be aproximated by 2^(-s/2)
		2^(-s/2) can be done by shifting the equivalent to 2^0 (1.0)
		And that it what "absshift" does, absshift is the half of the absolute
		value of s.

		Ex:
			8.0  -> shift = 3, absshift = 3/2 = 1
			72.0 -> shift = 7, absshift = 7/2 = 3

		To further approximate the initial value, we do a linear approximation of m
		in two sectors. We know that m is a number between 1 and 4 because if m is
		closer to 1, x is closer to the lower even power of two, but if m is closer to 4,
		x is closer to the higher even power of two.

		Notation:
			m: original m, that is x divided by the 2 to the lower even exponent
			m_l: linear approximation of 1/sqrt(m)

		Sector A: m e [1, 2[
			m_l = 1 - (m-1) * (1 - sqrt(2)/2) - qA

		Sector B: m e [2, 4[
			m_l = sqrt(2)/2 - (m-2) * (sqrt(2)/4 - 0.25) - qB

				qA = 0.02243596878		qB = 0.0196403506

		The "previous big power of two" is the previous (or equal) power of two with an even exponen, like 1 (2^0),
		4 (2^2), 16 (2^4) and so on, 64, 256, 1024, 4096...

		Lets do a step-by-step for x = 4.0, 32.0 and 1732.5

		x = 4.0, the previous big power of two is 4.
			m 	= 1
			m_l => Sector A: m_l = 1
			absshift = (2/1) = 1
			y = 1 / 2^1 = 0.5
			1/sqrt(4) = 0.5
			e_r = 0

		x = 32.0, the previous big power of two is 16.
			m 	= 2
			m_l => Sector B: m_l = 0.707106
			absshift = (5/2) = 2
			y = 0.707106 / 2^2 = 0.1767766953
			1/sqrt(32) = 0.1767766953
			e_r = 0

		x = 1732.5, the previous big power of two is 1024.
			m 	= 1.691894531
			m_l => Sector A: m_l = 0.7973487837
			absshift = (10/2) = 5
			y = 0.7973487837 / 2^5 = 0.02491714949
			1/sqrt(32) = 0.02402499901
			e_r = 3.71e-2

		This initial approximation helps the Newton method to converge with less iterations
		Instead of using 1 * 2^(-s/2) as our initial guess, we use a value between 0.5 to 1
		to reduce the relative error. Using a graphic calculator, we saw that the maximum error
		for this approximation is about 6e-4
	*/

	uint8_t absshift = 0;
	uint8_t normalizeShift = 0;


	if(shift > 0){
		absshift = shift >> 1;
		normalizeShift = shift & ~(0x1);
		// y is now between 1 and 4
		y = (x >> normalizeShift);
	} else {
		/*
			small "tweak" compared to the shift > 0 version, tested with gdb.
			This ensures that the linear approximation is also valid even for
			negative exponents
		*/
		absshift = abs(shift-1) >> 1;
		normalizeShift = (abs(shift) + 1) & ~(0x1);
		// y is now between 1 and 4
		y = (x << (normalizeShift));
	}

	// Compares if it should use the 2-4 segment or 1-2
	if(y >= (F32_ONE << 1)){
		// Sector B: y lies between 2 and 4

		// y - 2
		y = y - (F32_ONE << 1);

		// (y-2) * (sqrt(2)/4 - 0.25)
		// Note: (sqrt(2)/4 - 0.25) = 0.1035533905933
		// but we use 13/128 (0.1015625) as an approximation
		y = (y + (y << 2) + (y << 3)) >> 7;

		// sqrt(2)/2 - (y-2) * (sqrt(2)/4 - 0.25)
		y = (F32_SQRT_2 >> 1) - y;

		// Correction factor qA
		y = y - F32_RSQRT_CORRECTION_B;
	} else {
		// Sector A: y lies between 1 and 2

		// y - 1
		y = y - F32_ONE;

		// (y-1) * (1 - sqrt(2)/2)
		// Note: (1 - sqrt(2)/2) = 0.2928932188135
		// but we use 19/64 (0.296875) as an approximation
		y = (y + (y << 1) + (y << 4)) >> 6;

		// 1 - (y-1) * (1 - sqrt(2)/2)
		y = (F32_ONE) - y;
	
		// Correction factor qB
		y = y - F32_RSQRT_CORRECTION_A;
	}

	// Multiplication by 2^(-s/2)
	if(shift > 0){
		y = y >> absshift;
	} else {
		y = y << (absshift);
	}

#ifdef DEBUG_MODE
	printf("y: %lf\n", convertF32ToDouble(y));
#endif
	
	/*
		"1st - 2nd iteration, can't be ignored"

		Performs a Newton Rhapson iteration for y, the value obtained on the
		last block.

		This follows the next formula:
						f(y) = 1/y^2 - x

		where:
			y is the requiered value
			x the initial value

		Then, the derivativa:
					f'(y) = -2/y^3

		We create the Newton method:
			y_n+1 = y_n - (1/y_n^2 - x) / (-2/y_n^3)

		Then we simplify:
			y_n+1 = y_n + (y_n - x*y_n^3)/2
			y_n+1 = 3/2 * y_n - (x*y_n^3) / 2
			y_n+1 = 3/2 * y_n - ((x/2)*y_n^3)
			y_n+1 = y_n(3/2- ((x/2)*y_n^2))

			x/2 is the variable x2
			3/2 is threehalfs

		RSQRT_ITER = 2

		We iterate 2 times because uppon testing, relative error
		is about 1e-2 after two iterations IN THE WHOLE DOMAIN, in
		values with exponent between -10 and 10 (base 2), the error
		is like 1e-4 to 4e-3 (two iterations)
	
		1 iteration: e_r approx 1.89e-2 
		2 iteration: e_r approx 1.76e-3
		3 iteration: e_r approx 1.76e-2

		in 2-12 to 2 12 range

		1 iteration: e_r approx 4.9e-3 
		2 iteration: e_r approx 2,8e-3
		3 iteration: e_r approx 2.8e-3

	*/

	/*
		Now, we tested that, for x below 2^-14, the approximation gives lower
		relative error than even 1 iteration of Newton, this is because we are dealing
		with very small numbers for fixed point, and we lack precision.

		This also happens with values above 2^14, we tested that just using the approximation
		is far better in terms of relative error.

		We saw (using Octave and exporting the values as .csv), that for x below 2^-14, the
		result went way higher than just the approximation, and for above 2^14, it starts to oscillate
		very heavily and a small variation caused major numeric "leakage", this might be because of
		overflow in multiplication.

		Between 2^-16 to 2^-14, and 2^14 to 2^16 we disable Newton method
		14/-14 value is set on RSQRT_EXP_NO_ITER_F32.
	*/
	uint16_t iterCount = RSQRT_ITER;
	if(abs(shift) >= RSQRT_EXP_NO_ITER_F32){
		iterCount = 0;
	}

	for(int16_t i = 0; i < iterCount; i++){
		y = fixedMul32(y, threehalfs - fixedMul32(x2, fixedMul32(y, y)));
	}
	// We |y|, just to be sure it's positive because the Newton method cam
	// give the negative value (tested with a calculator and this function without abs)
	return abs(y);
}

fixed32_t fixedRSqrt32(fixed32_t x){
	if(x <= F32_ZERO){
		fixedSetErrno(FP_ERROR_NEGATIVE);
		return F32_ZERO;
	}

	return ufixedRSqrt32(x);
}

ufixed64_t ufixedRSqrt64(ufixed64_t x){
	/*
		.This function calculates the 1/sqrt(x) (inverse square root, or reciprocal square root)
		of x for a q32.32 number in fixed point (ufixed64_t).

		This is useful to calculate the norm of vector for physics and lighting in 2d/3d spaces
		like simulators, videogames or in embedded machines like microcontrollers to control
		servos.
	*/

	// Special case, 0 or below. 1/sqrt(0) is not defined
	// neither 1/sqrt(x) where x is negative
	if(x <= F64_ZERO){
		fixedSetErrno(FP_ERROR_NEGATIVE);
		return F64_ZERO;
	 }

	fixed64_t x2 			= x >> 1;					// x2			= x/2
	fixed64_t y  			= F64_ZERO;
	fixed64_t threehalfs  	= F64_ONE + (F64_ONE >> 1);	// threehalfs 	= 1.5 in fixed point

	/*
		"evil fixed point bit level hacking"

		Get the log_2 of x by taking the highest one with getBiggestOne64
		(that function is a CLZ for 64 bits)

		ex: 0b1... -> n =  0
			0b0..1 -> n = 64

		shift then converts the position from n to the exponent form of 2^shift
		shift = (64/2) - n - 1

		ex: 0b1... -> 2147483648.0 (in fixed point) = 2^31
					  n =  0
					  shift = 32 - 0 - 1 = 31

			0b...1 -> 1/4294967296 (in fixed point) = 2^-32
					  n =  63
					  shift = 32 - 63 - 1 = -32
	*/

	uint8_t n 		= getBiggestOne64(x);
	int8_t shift 	= (F64_SIZE >> 1) - n - 1;

#ifdef DEBUG_MODE
	printf("n: %i\ts: %i\n", n, shift);
	printf("x: %lf\n", convertF64ToDouble(x));
#endif

	/* 
		"what the fuck?"

		x is equivalent to 2^s * m, where:
			s is the exponent (given in "shift")
			m is a value between 1 and 2.

		Then, 1/sqrt(x) can be aproximated by 2^(-s/2)
		2^(-s/2) can be done by shifting the equivalent to 2^0 (1.0)
		And that it what "absshift" does, absshift is the half of the absolute
		value of s.

		Ex:
			8.0   -> shift = 3, absshift = 3/2 = 1
			72.02 -> shift = 7, absshift = 7/2 = 3

		To further approximate the initial value, we do a linear approximation of m
		in two sectors. We know that m is a number between 1 and 4 because if m is
		closer to 1, x is closer to the lower even power of two, but if m is closer to 4,
		x is closer to the higher even power of two.

		Notation:
			m: original m, that is x divided by the 2 to the lower even exponent
			m_l: linear approximation of 1/sqrt(m)

		Sector A: m e [1, 2[
			m_l = 1 - (m-1) * (1 - sqrt(2)/2) - qA

		Sector B: m e [2, 4[
			m_l = sqrt(2)/2 - (m-2) * (sqrt(2)/4 - 0.25) - qB

				qA = 0.02243596878		qB = 0.0196403506

		The "previous big power of two" is the previous (or equal) power of two with an even exponen, like 1 (2^0),
		4 (2^2), 16 (2^4) and so on, 64, 256, 1024, 4096...

		Lets do a step-by-step for x = 4.0, 32.0 and 1732.5

		x = 4.0, the previous big power of two is 4.
			m 	= 1
			m_l => Sector A: m_l = 1
			absshift = (2/1) = 1
			y = 1 / 2^1 = 0.5
			1/sqrt(4) = 0.5
			e_r = 0

		x = 32.0, the previous big power of two is 16.
			m 	= 2
			m_l => Sector B: m_l = 0.707106
			absshift = (5/2) = 2
			y = 0.707106 / 2^2 = 0.1767766953
			1/sqrt(32) = 0.1767766953
			e_r = 0

		x = 1732.5, the previous big power of two is 1024.
			m 	= 1.691894531
			m_l => Sector A: m_l = 0.7973487837
			absshift = (10/2) = 5
			y = 0.7973487837 / 2^5 = 0.02491714949
			1/sqrt(32) = 0.02402499901
			e_r = 3.71e-2

		This initial approximation helps the Newton method to converge with less iterations
		Instead of using 1 * 2^(-s/2) as our initial guess, we use a value between 0.5 to 1
		to reduce the relative error. Using a graphic calculator, we saw that the maximum error
		for this approximation is about 6e-4
	*/

	uint8_t absshift = 0;
	uint8_t normalizeShift = 0;

	if(shift > 0){
		absshift = shift >> 1;
		normalizeShift = shift & ~(0x1);
		// y is now between 1 and 4
		y = (x >> normalizeShift);
	} else {
		/*
			small "tweak" compared to the shift > 0 version, tested with gdb.
			This ensures that the linear approximation is also valid even for
			negative exponents
		*/
		absshift = abs(shift-1) >> 1;
		normalizeShift = (abs(shift) + 1) & ~(0x1);
		// y is now between 1 and 4
		y = (x << (normalizeShift));
	}

	// Compares if it should use the 2-4 segment or 1-2
	if(y >= (F64_ONE << 1)){
		// Sector B: y lies between 2 and 4

		// y - 2
		y = y - (F64_ONE << 1);

		// (y-2) * (sqrt(2)/4 - 0.25)
		// Note: (sqrt(2)/4 - 0.25) = 0.1035533905933
		// but we use 13/128 (0.1015625) as an approximation
		y = (y + (y << 2) + (y << 3)) >> 7;

		// sqrt(2)/2 - (y-2) * (sqrt(2)/4 - 0.25)
		y = (F64_SQRT_2 >> 1) - y;

		// Correction factor qA
		y = y - F64_RSQRT_CORRECTION_B;
	} else {
		// Sector A: y lies between 1 and 2

		// y - 1
		y = y - F64_ONE;

		// (y-1) * (1 - sqrt(2)/2)
		// Note: (1 - sqrt(2)/2) = 0.2928932188135
		// but we use 19/64 (0.296875) as an approximation
		y = (y + (y << 1) + (y << 4)) >> 6;

		// 1 - (y-1) * (1 - sqrt(2)/2)
		y = (F64_ONE) - y;
	
		// Correction factor qB
		y = y - F64_RSQRT_CORRECTION_A;
	}

	// Multiplication by 2^(-s/2)
	if(shift > 0){
		y = y >> absshift;
	} else {
		y = y << (absshift);
	}

#ifdef DEBUG_MODE
	printf("y: %lf\n", convertF64ToDouble(y));
#endif
	
	/*
		"1st - 2nd iteration, can't be ignored"

		Performs a Newton Rhapson iteration for y, the value obtained on the
		last block.

		This follows the next formula:
						f(y) = 1/y^2 - x

		where:
			y is the requiered value
			x the initial value

		Then, the derivativa:
					f'(y) = -2/y^3

		We create the Newton method:
			y_n+1 = y_n - (1/y_n^2 - x) / (-2/y_n^3)

		Then we simplify:
			y_n+1 = y_n + (y_n - x*y_n^3)/2
			y_n+1 = 3/2 * y_n - (x*y_n^3) / 2
			y_n+1 = 3/2 * y_n - ((x/2)*y_n^3)
			y_n+1 = y_n(3/2- ((x/2)*y_n^2))

			x/2 is the variable x2
			3/2 is threehalfs

		RSQRT_ITER = 2

		We iterate 2 times because uppon testing, relative error
		is about 1e-2 after two iterations IN THE WHOLE DOMAIN, in
		values with exponent between -10 and 10 (base 2), the error
		is like 1e-4 to 4e-3 (two iterations)
	
		1 iteration: e_r approx 1.89e-2 
		2 iteration: e_r approx 1.76e-3
		3 iteration: e_r approx 1.76e-2

		in 2-12 to 2 12 range

		1 iteration: e_r approx 4.9e-3 
		2 iteration: e_r approx 2,8e-3
		3 iteration: e_r approx 2.8e-3

	*/

	/*
		Now, we tested that, for x below 2^-14, the approximation gives lower
		relative error than even 1 iteration of Newton, this is because we are dealing
		with very small numbers for fixed point, and we lack precision.

		This also happens with values above 2^14, we tested that just using the approximation
		is far better in terms of relative error.

		We saw (using Octave and exporting the values as .csv), that for x below 2^-14, the
		result went way higher than just the approximation, and for above 2^14, it starts to oscillate
		very heavily and a small variation caused major numeric "leakage", this might be because of
		overflow in multiplication.

		Between 2^-16 to 2^-14, and 2^14 to 2^16 we disable Newton method
		14/-14 value is set on RSQRT_EXP_NO_ITER_F32.
	*/
	uint16_t iterCount = RSQRT_ITER;
	if(abs(shift) >= RSQRT_EXP_NO_ITER_F64){
		iterCount = 0;
	}

	for(int16_t i = 0; i < iterCount; i++){
		y = ufixedMul64(y, threehalfs - ufixedMul64(x2, ufixedMul64(y, y)));
	}
	// We |y|, just to be sure it's positive because the Newton method cam
	// give the negative value (tested with a calculator and this function without abs)
	if(y < F32_ZERO){
		y = fixedNeg64(y);
	}

	return y;
}

fixed64_t fixedRSqrt64(fixed64_t x){
	if(x <= F64_ZERO){
		fixedSetErrno(FP_ERROR_NEGATIVE);
		return F64_ZERO;
	}

	return ufixedRSqrt64(x);
}
