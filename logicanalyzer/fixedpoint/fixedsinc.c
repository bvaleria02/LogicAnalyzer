#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

fixed32_t  fixedSinc32( fixed32_t x){
	x = fixedAbs32(x);
	return ufixedSinc32(x);
}

ufixed32_t ufixedSinc32(ufixed32_t x){
	if(x == F32_ZERO){
		return F32_ONE;
	}

	ufixed32_t sin = ufixedSin32(x);
	ufixed32_t rx  = ufixedRecp32(x);
	return fixedMul32(sin, rx);
}

fixed64_t  fixedSinc64( fixed64_t x){
	x = fixedAbs64(x);
	return ufixedSinc64(x);
}

ufixed64_t ufixedSinc64(ufixed64_t x){
	if(x == F64_ZERO){
		return F64_ONE;
	}
	ufixed64_t sin = ufixedSin64(x);
	ufixed64_t rx  = ufixedRecp64(x);
	return fixedMul64(sin, rx);
}
