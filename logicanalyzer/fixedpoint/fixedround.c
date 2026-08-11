#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

fixed32_t  fixedFloor32( fixed32_t x){
	uint8_t sign = 0;
	fixed32_t decimal;
	fixed32_t integer;

	if(x < F32_ZERO){
		sign = 1;
		x = fixedAbs32(x);
	}
	
	decimal = x & F32_LMASK;
	integer = x & F32_HMASK;

	if(decimal && sign != 0){
		integer += F32_ONE;
	}

	if(sign){
		integer = fixedNeg32(integer);
	}

	return integer;
}

ufixed32_t ufixedFloor32(ufixed32_t x){
	return x & F32_LMASK;
}

fixed64_t  fixedFloor64( fixed64_t x){
	uint8_t sign = 0;
	fixed64_t decimal;
	fixed64_t integer;

	if(x < F64_ZERO){
		sign = 1;
		x = fixedAbs64(x);
	}
	
	decimal = x & F64_LMASK;
	integer = x & F64_HMASK;

	if(decimal && sign != 0){
		integer += F64_ONE;
	}

	if(sign){
		integer = fixedNeg64(integer);
	}

	return integer;
}

ufixed64_t ufixedFloor64(ufixed64_t x){
	return x & F64_LMASK;
}

fixed32_t  fixedCeil32( fixed32_t x){
	uint8_t sign = 0;
	fixed32_t decimal;
	fixed32_t integer;

	if(x < F32_ZERO){
		sign = 1;
		x = fixedAbs32(x);
	}
	
	decimal = x & F32_LMASK;
	integer = x & F32_HMASK;

	if(decimal && sign == 0){
		integer += F32_ONE;
	}

	if(sign){
		integer = fixedNeg32(integer);
	}

	return integer;
}

ufixed32_t ufixedCeil32(ufixed32_t x){
	ufixed32_t integer = x & F32_HMASK;
	ufixed32_t decimal = x & F32_LMASK;
	
	if(decimal){
		integer += F32_ONE;
	}

	return integer;
}

fixed64_t  fixedCeil64( fixed64_t x){
	uint8_t sign = 0;
	fixed64_t decimal;
	fixed64_t integer;

	if(x < F64_ZERO){
		sign = 1;
		x = fixedAbs64(x);
	}
	
	decimal = x & F64_LMASK;
	integer = x & F64_HMASK;

	if(decimal && sign == 0){
		integer += F64_ONE;
	}

	if(sign){
		integer = fixedNeg64(integer);
	}

	return integer;
}

ufixed64_t ufixedCeil64(ufixed64_t x){
	ufixed64_t integer = x & F64_HMASK;
	ufixed64_t decimal = x & F64_LMASK;
	
	if(decimal){
		integer += F64_ONE;
	}

	return integer;
}

fixed32_t  fixedRound32( fixed32_t x, int8_t exp){
	if(exp >= F32_RSHIFT || exp < -(F32_RSHIFT)){
		fixedSetErrno(FP_ERROR_OUTOFRANGE);
	}

	uint8_t sign = 0;
	if(x < F32_ZERO){
		sign = 1;
		x = fixedAbs32(x);
	}

	fixed64_t y = ufixedRound32(x, exp);

	if(sign){
		y = fixedNeg32(y);
	}

	return y;
}

ufixed32_t ufixedRound32(ufixed32_t x, int8_t exp){
	if(exp > F32_RSHIFT || exp < -(F32_RSHIFT)){
		fixedSetErrno(FP_ERROR_OUTOFRANGE);
	}

	ufixed32_t mask = F32_ONE;

	if(exp > 0){
		mask = mask << exp;
	} else if (exp < 0){
		mask = mask >> abs(exp);
	}

	// 0x0001 0000 -> 0x0000 8000
	ufixed32_t half = mask >> 1;

	// 0x0001 0000 -> 0x0000 FFFF
	mask = mask - 1;

	x += half;

	x = x & ~(mask);

	return x;
}

fixed64_t  fixedRound64( fixed64_t x, int8_t exp){
	if(exp >= F64_RSHIFT || exp < -(F64_RSHIFT)){
		fixedSetErrno(FP_ERROR_OUTOFRANGE);
	}

	uint8_t sign = 0;
	if(x < F64_ZERO){
		sign = 1;
		x = fixedAbs64(x);
	}

	fixed64_t y = ufixedRound64(x, exp);

	if(sign){
		y = fixedNeg64(y);
	}

	return y;
}

ufixed64_t ufixedRound64(ufixed64_t x, int8_t exp){
	if(exp > F64_RSHIFT || exp < -(F64_RSHIFT)){
		fixedSetErrno(FP_ERROR_OUTOFRANGE);
	}

	ufixed64_t mask = F64_ONE;

	if(exp > 0){
		mask = mask << exp;
	} else if (exp < 0){
		mask = mask >> abs(exp);
	}

	// 0x00000001 00000000 -> 0x00000000 80000000
	ufixed64_t half = mask >> 1;

	// 0x00000001 00000000 -> 0x00000000 FFFFFFFF
	mask = mask - 1;

	x += half;

	x = x & ~(mask);

	return x;
}

fixed32_t  fixedFract32( fixed32_t x){
	uint8_t sign = 0;
	if(x < F32_ZERO){
		sign = 1;
		x = fixedAbs32(x);
	}

	fixed32_t y = ufixedFract32(x);

	if(sign){
		y = fixedNeg32(y);
	}

	return y;
}

ufixed32_t ufixedFract32(ufixed32_t x){
	return x & F32_LMASK;
}

fixed64_t  fixedFract64( fixed64_t x){
	uint8_t sign = 0;
	if(x < F64_ZERO){
		sign = 1;
		x = fixedAbs64(x);
	}

	fixed64_t y = ufixedFract64(x);

	if(sign){
		y = fixedNeg64(y);
	}

	return y;
}

ufixed64_t ufixedFract64(ufixed64_t x){
	return x & F64_LMASK;
}
