#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>
#include "fixedconstants.h"
#include "fixedtrig/fixedsinehyper.h"

fixed32_t  fixedExp32( fixed32_t x){
	uint8_t sign = 0;
	if(x < F32_ZERO){
		x = fixedNeg32(x);
		sign = 1;
	}

	fixed32_t y = ufixedExp32(x);
	
	if(sign){
		y = fixedRecp32(y);
	}

	return y;
}

ufixed32_t ufixedExp32(ufixed32_t x){
	ufixed32_t integerExp = x >> F32_RSHIFT;
	ufixed32_t decimalExp = x & F32_LMASK;

	fixed32_t sinh;
	fixed32_t cosh;

	ufixed32_t y = ufixedPow32(F32_E, integerExp);
	abstractSineHyperbolicCORDIC32(decimalExp, &sinh, &cosh);

	return ufixedMul32(y, cosh + sinh);
}

fixed64_t  fixedExp64( fixed64_t x){
	uint8_t sign = 0;
	if(x < F64_ZERO){
		x = fixedNeg64(x);
		sign = 1;
	}

	fixed64_t y = ufixedExp64(x);
	
	if(sign){
		y = fixedRecp64(y);
	}

	return y;
}

ufixed64_t ufixedExp64(ufixed64_t x){
	ufixed64_t integerExp = x >> F64_RSHIFT;
	ufixed64_t decimalExp = x & F64_LMASK;

	fixed64_t sinh;
	fixed64_t cosh;

	ufixed64_t y = ufixedPow64(F64_E, integerExp);
	abstractSineHyperbolicCORDIC64(decimalExp, &sinh, &cosh);

	return ufixedMul64(y, cosh + sinh);
}

