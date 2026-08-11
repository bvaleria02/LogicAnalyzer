#include "fixedpoint.h"
#include <stdint.h>

fixed32_t fixedAdd32(fixed32_t x1, fixed32_t x2){
	fixed32_t y = x1 + x2;

	if((y < x1) && x2 > F32_ZERO){
		fixedSetErrno(FP_ERROR_OVERFLOW);
	}
	return y;
}

ufixed32_t ufixedAdd32(ufixed32_t x1, ufixed32_t x2){
	ufixed32_t y = x1 + x2;

	if(y < x1){
		fixedSetErrno(FP_ERROR_OVERFLOW);
	}
	return y;
}

fixed64_t fixedAdd64(fixed64_t x1, fixed64_t x2){
	fixed64_t y = x1 + x2;

	if((y < x1) && x2 > F64_ZERO){
		fixedSetErrno(FP_ERROR_OVERFLOW);
	}
	return y;
}
ufixed64_t ufixedAdd64(ufixed64_t x1, ufixed64_t x2){
	ufixed64_t y = x1 + x2;

	if(y < x1){
		fixedSetErrno(FP_ERROR_OVERFLOW);
	}
	return y;
}

fixed32_t fixedAddScalar32(fixed32_t x1, int32_t x2, int8_t scale){
	fixed32_t f2 = convertIntToF32(x2, scale);
	return fixedAdd32(x1, f2);
}

ufixed32_t ufixedAddScalar32(ufixed32_t x1, uint32_t x2, int8_t scale){
	ufixed32_t f2 = convertIntToUF32(x2, scale);
	return ufixedAdd32(x1, f2);
}

fixed64_t fixedAddScalar64(fixed64_t x1,  int64_t x2, int8_t scale){
	fixed64_t f2 = convertIntToF64(x2, scale);
	return fixedAdd64(x1, f2);
}

ufixed64_t ufixedAddScalar64(ufixed64_t x1, uint64_t x2, int8_t scale){
	ufixed64_t f2 = convertIntToUF64(x2, scale);
	return ufixedAdd64(x1, f2);
}

fixed32_t fixedAddOF32(fixed32_t x1, fixed32_t x2){
	fixedClearErrno();

	fixed32_t y = fixedAdd32(x1, x2);
	FPErrorCode code = fixedGetErrno();
	if(code == FP_ERROR_OVERFLOW){
		y = F32_MAX_VALUE;
	}
	return y;
}

ufixed32_t ufixedAddOF32(ufixed32_t x1, ufixed32_t x2){
	fixedClearErrno();

	ufixed32_t y = ufixedAdd32(x1, x2);
	FPErrorCode code = fixedGetErrno();
	if(code == FP_ERROR_OVERFLOW){
		y = UF32_MAX_VALUE;
	}
	return y;
}

fixed64_t fixedAddOF64(fixed64_t x1, fixed64_t x2){
	fixedClearErrno();

	fixed64_t y = fixedAdd64(x1, x2);
	FPErrorCode code = fixedGetErrno();
	if(code == FP_ERROR_OVERFLOW){
		y = F64_MAX_VALUE;
	}
	return y;
}

ufixed64_t ufixedAddOF64(ufixed64_t x1, ufixed64_t x2){
	fixedClearErrno();

	ufixed64_t y = ufixedAdd64(x1, x2);
	FPErrorCode code = fixedGetErrno();
	if(code == FP_ERROR_OVERFLOW){
		y = UF64_MAX_VALUE;
	}
	return y;
}