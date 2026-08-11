#include "fixedpoint.h"
#include <stdio.h>
#include <stdlib.h>

void signAndAbs32(fixed32_t *x, uint8_t *sign){
	if((*x) < F32_ZERO){
		(*sign) = !(*sign);
		(*x) = fixedNeg32(*x);
	}
}

void signAndAbs64(fixed64_t *x, uint8_t *sign){
	if((*x) < F64_ZERO){
		(*sign) = !(*sign);
		(*x) = fixedNeg64(*x);
	}
}

fixed32_t  fixedLShift32( fixed32_t x, uint8_t shift){
	uint8_t sign = 0;
	signAndAbs32(&x, &sign);

	fixed32_t y = ufixedLShift32(x, shift);

	if(sign){
		y = fixedNeg32(y);
	}

	return y;
}

ufixed32_t ufixedLShift32(ufixed32_t x, uint8_t shift){
	return x << shift;
}

fixed64_t  fixedLShift64( fixed64_t x, uint8_t shift){
	uint8_t sign = 0;
	signAndAbs64(&x, &sign);

	fixed64_t y = ufixedLShift64(x, shift);

	if(sign){
		y = fixedNeg64(y);
	}

	return y;
}

ufixed64_t ufixedLShift64(ufixed64_t x, uint8_t shift){
	return x << shift;
}

fixed32_t  fixedRShift32( fixed32_t x, uint8_t shift){
	uint8_t sign = 0;
	signAndAbs32(&x, &sign);

	fixed32_t y = ufixedRShift32(x, shift);

	if(sign){
		y = fixedNeg32(y);
	}

	return y;
}

ufixed32_t ufixedRShift32(ufixed32_t x, uint8_t shift){
	return x >> shift;
}

fixed64_t  fixedRShift64( fixed64_t x, uint8_t shift){
	uint8_t sign = 0;
	signAndAbs64(&x, &sign);

	fixed64_t y = ufixedRShift64(x, shift);

	if(sign){
		y = fixedNeg64(y);
	}

	return y;
}

ufixed64_t ufixedRShift64(ufixed64_t x, uint8_t shift){
	return x >> shift;
}

