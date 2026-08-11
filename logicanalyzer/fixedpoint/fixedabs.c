#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>

fixed32_t fixedAbs32(fixed32_t x){
	if(x < F32_ZERO){
		x = fixedNeg32(x);
	}
	return x;
}

ufixed32_t ufixedAbs32(ufixed32_t x){
	return x;
}

fixed64_t fixedAbs64(fixed64_t x){
	if(x < F64_ZERO){
		x = fixedNeg64(x);
	}
	return x;
}

ufixed64_t ufixedAbs64(ufixed64_t x){
	return x;
}

fixed32_t fixedSgn32(fixed32_t x){
	fixed32_t y = F32_ZERO;

	if(x < F32_ZERO){
		y = fixedNeg32(F32_ONE);
	} else if(x > F32_ZERO){
		y = F32_ONE;
	}

	return y;
}

ufixed32_t ufixedSgn32(ufixed32_t x){
	ufixed32_t y = F32_ZERO;

	if(x > F32_ZERO){
		y = F32_ONE;
	}

	return y;
}

fixed64_t fixedSgn64(fixed64_t x){
	fixed64_t y = F64_ZERO;

	if(x < F64_ZERO){
		y = fixedNeg64(F64_ONE);
	} else if(x > F64_ZERO){
		y = F64_ONE;
	}

	return y;
}

ufixed64_t ufixedSgn64(ufixed64_t x){
	ufixed64_t y = F64_ZERO;

	if(x > F64_ZERO){
		y = F64_ONE;
	}

	return y;
}
