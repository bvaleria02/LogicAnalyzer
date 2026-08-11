#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>
#include "fixedconstants.h"
#include "utils/fixedlimits.h"
#include "multin.h"

fixed32_t fixedDiv32(fixed32_t x1, fixed32_t x2){
	if(x2 == F32_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return F32_MAX_VALUE;
	}

	fixed32_t y = (((int64_t)x1 * F32_SCALE)/ (int32_t)x2);
	return y;
}

ufixed32_t ufixedDiv32(ufixed32_t x1, ufixed32_t x2){
	if(x2 == F32_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return UF32_MAX_VALUE;
	}

	ufixed32_t y = (((uint64_t)x1) * F32_SCALE) / ((uint64_t) x2);
	return y;
}

uint8_t canFastPath(ufixed64_t x1, ufixed64_t x2, ufixed64_t *r){
	uint8_t lowestOne = getLowestOne64(x2);
#ifdef DEBUG_MODE
	printf("lowestOne: %hhu\n", lowestOne);
#endif
	int8_t rawShift = ((F64_SIZE >> 1) - lowestOne);
#ifdef DEBUG_MODE
	printf("rawShift: %hhi\n", rawShift);
	printf("before shifting x2: %016lx\n", x2);
#endif

	if(rawShift < 0){
#ifdef DEBUG_MODE
		printf("x2 is > 1\n");
#endif
		x2 = x2 >> (rawShift * -1);
	} else {
#ifdef DEBUG_MODE
		printf("x2 is < 1\n");
#endif
		x2 = x2 << rawShift;
	}
#ifdef DEBUG_MODE
	printf("x2: %016lx\n", x2);
#endif

	uint8_t fastPath = 0;

	x2 = x2 >> (F64_SIZE >> 1);
#ifdef DEBUG_MODE
	printf("int x2: %016lx\n", x2);
#endif
	if(x2 < RECIPROCAL_F64_COUNT && x2 >= 1){
#ifdef DEBUG_MODE
		printf("Using lut: value %li\t", x2 - 1);
#endif
		(*r) = ufixedMul64(x1, reciprocalF64[x2 - 1]);
		fastPath = 1;
	}
#ifdef DEBUG_MODE
	printf("r: %016lx\n", *r);
#endif

	if(fastPath == 0){
		return 0;
	}

	if(rawShift < 0){
		(*r) = (*r) >> (rawShift * -1);
	} else {
		(*r) = (*r) << rawShift;
	}
#ifdef DEBUG_MODE
	printf("r: %016lx\n", *r);
#endif

	return 1;
}

ufixed64_t ufixedDiv64(ufixed64_t x1, ufixed64_t x2){
	ufixed64_t r = 0;

	if(x2 == F64_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return UF64_MAX_VALUE;
	}

	if(canFastPath(x1, x2, &r)){
		return r;
	}

	const uint8_t size1 = BUFFER_N_SIZE(_F96_SIZE);
	multiNBuffer y1[size1];
	multiNBlank(y1, size1);
	multiNBuffer tmp1[size1];
	multiNBlank(tmp1, size1);
	multiNSetInt(y1, size1, (uint64_t) x1);
#ifdef DEBUG_MODE
	multiNPrintBinary(y1, size1);
#endif

	const uint8_t size2 = BUFFER_N_SIZE(_F96_SIZE);
	multiNBuffer y2[size2];
	multiNBlank(y2, size2);
	multiNBuffer tmp2[size2];
	multiNBlank(tmp2, size2);
	multiNSetInt(y2, size2, (uint64_t) x2);
#ifdef DEBUG_MODE
	multiNPrintBinary(y2, size2);
#endif

	const uint8_t size3 = BUFFER_N_SIZE(_F128_SIZE);
	multiNBuffer a[size3];
	multiNBlank(a, size3);

	const uint8_t size4 = BUFFER_N_SIZE(_F96_SIZE);
	multiNBuffer b[size4];
	multiNBlank(b, size4);

	multiNDiv(y1, size1, y2, size2, a, size3, b, size4, tmp1, tmp2);

#ifdef DEBUG_MODE
	multiNPrintBinary(y1, size1);
	multiNPrintBinary(y2, size2);
	multiNPrintBinary(a, size3);
	multiNPrintBinary(b, size4);
	multiNPrintBinary(tmp1, size1);
	multiNPrintBinary(tmp2, size2);
#endif
	return ((uint64_t)a[2] << F32_SIZE) | (uint64_t)a[1];
}

fixed64_t fixedDiv64(fixed64_t x1, fixed64_t x2){
	uint8_t sign = 0;

	if(x2 == F64_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return F64_MAX_VALUE;
	}

	if(x1 < F64_ZERO){
		x1 = fixedNeg64(x1);
		sign = !(sign);
	}

	if(x2 < F64_ZERO){
		x2 = fixedNeg64(x2);
		sign = !(sign);
	}

	ufixed64_t result = ufixedDiv64(x1, x2);
	fixed64_t resultSigned;

	if(sign){
		resultSigned = fixedNeg64(result);
	} else {
		resultSigned = result;
	}

	return resultSigned;
}
