#include "fixedpoint.h"
#include <stdint.h>

fixed32_t fixedSub32(fixed32_t x1, fixed32_t x2){
	fixed32_t y = x1 - x2;

	if((y > x1) && x2 > F32_ZERO){
		fixedSetErrno(FP_ERROR_UNDERFLOW);
	}
	return y;
}

ufixed32_t ufixedSub32(ufixed32_t x1, ufixed32_t x2){
	ufixed32_t y = x1 - x2;

	if(y > x1){
		fixedSetErrno(FP_ERROR_UNDERFLOW);
	}
	return y;
}

fixed64_t fixedSub64(fixed64_t x1, fixed64_t x2){
	fixed64_t y = x1 - x2;

	if((y > x1) && x2 > F64_ZERO){
		fixedSetErrno(FP_ERROR_UNDERFLOW);
	}
	return y;
}

ufixed64_t ufixedSub64(ufixed64_t x1, ufixed64_t x2){
	ufixed64_t y = x1 - x2;

	if(y > x1){
		fixedSetErrno(FP_ERROR_UNDERFLOW);
	}
	return y;
}

fixed32_t fixedSubScalar32(fixed32_t x1, int32_t x2, int8_t scale){
	fixed32_t f2 = convertIntToF32(x2, scale);
	return fixedSub32(x1, f2);
}

ufixed32_t ufixedSubScalar32(ufixed32_t x1, uint32_t x2, int8_t scale){
	ufixed32_t f2 = convertIntToUF32(x2, scale);
	return ufixedSub32(x1, f2);
}

fixed64_t fixedSubScalar64(fixed64_t x1,  int64_t x2, int8_t scale){
	fixed64_t f2 = convertIntToF64(x2, scale);
	return fixedSub64(x1, f2);
}

ufixed64_t ufixedSubScalar64(ufixed64_t x1, uint64_t x2, int8_t scale){
	ufixed64_t f2 = convertIntToUF64(x2, scale);
	return ufixedSub64(x1, f2);
}

fixed32_t fixedSUbUF32(fixed32_t x1, fixed32_t x2){
	fixedClearErrno();

	fixed32_t y = fixedSub32(x1, x2);
	FPErrorCode code = fixedGetErrno();
	if(code == FP_ERROR_UNDERFLOW){
		y = F32_MIN_VALUE;
	}
	return y;
}

ufixed32_t ufixedSubUF32(ufixed32_t x1, ufixed32_t x2){
	fixedClearErrno();

	ufixed32_t y = ufixedSub32(x1, x2);
	FPErrorCode code = fixedGetErrno();
	if(code == FP_ERROR_UNDERFLOW){
		y = UF32_MIN_VALUE;
	}
	return y;
}

fixed64_t fixedSubUF64(fixed64_t x1, fixed64_t x2){
	fixedClearErrno();

	fixed64_t y = fixedSub64(x1, x2);
	FPErrorCode code = fixedGetErrno();
	if(code == FP_ERROR_UNDERFLOW){
		y = F64_MIN_VALUE;
	}
	return y;
}

ufixed64_t ufixedSubUF64(ufixed64_t x1, ufixed64_t x2){
	fixedClearErrno();

	ufixed64_t y = ufixedSub64(x1, x2);
	FPErrorCode code = fixedGetErrno();
	if(code == FP_ERROR_UNDERFLOW){
		y = UF64_MIN_VALUE;
	}
	return y;
}