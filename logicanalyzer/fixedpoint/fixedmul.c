#include "fixedpoint.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "multin.h"

fixed32_t fixedMul32(fixed32_t x1, fixed32_t x2){
	int64_t y = ((int64_t)x1 * (int64_t)x2) >> F32_RSHIFT;
	if(y > UF32_MAX_VALUE){
		fixedSetErrno(FP_ERROR_OVERFLOW);
	}

	return (fixed32_t) y;
}

ufixed32_t ufixedMul32(ufixed32_t x1, ufixed32_t x2){
	uint64_t y =  ((uint64_t)x1 * (uint64_t)x2) >> F32_RSHIFT;
	if(y > UF32_MAX_VALUE){
		fixedSetErrno(FP_ERROR_OVERFLOW);
	}

	return (ufixed32_t) y;
}

fixed64_t fixedMul64(fixed64_t x1, fixed64_t x2){
	uint8_t sign = 0;

	if(x1 < F64_ZERO){
		x1 = fixedNeg64(x1);
		sign = !(sign);
	}

	if(x2 < F64_ZERO){
		x2 = fixedNeg64(x2);
		sign = !(sign);
	}

	ufixed64_t result = ufixedMul64(x1, x2);

	fixed64_t resultSigned;
	if(sign){
		resultSigned = fixedNeg64(result);
	} else {
		resultSigned = result;
	}

	return resultSigned;
}

ufixed64_t ufixedMul64(ufixed64_t x1, ufixed64_t x2){
	const uint8_t size1 = BUFFER_N_SIZE(F64_SIZE);
	const uint8_t size2 = BUFFER_N_SIZE(F64_SIZE);
	multiNBuffer y1[size1];
	multiNBuffer y2[size2];

	const uint8_t size3 = BUFFER_N_SIZE(_F128_SIZE);
	multiNBuffer a[size3];

	multiNSetInt(y1, size1, (uint64_t) x1);
	multiNSetInt(y2, size2, (uint64_t) x2);
	multiNBlank(a, size3);
	
	multiNMul(y1, size1, y2, size2, a, size3);

	uint64_t result = ((uint64_t) a[2] << (F64_SIZE >> 1))| (uint64_t) a[1];
	return result;
}

/*
void getIndex128(uint8_t index, uint8_t *highIndex, uint8_t *lowIndex){
	(*highIndex) = index >> U64_SIZE;
	( *lowIndex) = index  & U64_MASK;
}

void setBit128(uint64_t buffer128[BUFFER_128_SIZE], uint8_t index, uint8_t value){
	if(buffer128 == NULL)	return;
	uint8_t highIndex;
	uint8_t lowIndex;
	getIndex128(index, &highIndex, &lowIndex);
	buffer128[highIndex] = buffer128[highIndex] & ~((uint64_t)(0x1 << lowIndex));
	buffer128[highIndex] = buffer128[highIndex] |  ((uint64_t)(value & 0x1) << lowIndex);
}

uint8_t getBit128(uint64_t buffer128[BUFFER_128_SIZE], uint8_t index){
	if(buffer128 == NULL)	return 0;
	uint8_t highIndex;
	uint8_t lowIndex;
	getIndex128(index, &highIndex, &lowIndex);
	uint64_t bit = buffer128[highIndex] & ((uint64_t)(0x1 << lowIndex));
	return (bit >> lowIndex) & 0x1;
}

void placeBuffer128(uint64_t buffer128[BUFFER_128_SIZE], uint64_t value){
	if(buffer128 == NULL)	return;
	buffer128[0] = value;
	buffer128[1] = 0;
}

void leftShift128(uint64_t buffer128[BUFFER_128_SIZE], uint8_t shift){
	if(buffer128 == NULL)	return;
	uint64_t value  =  buffer128[0];
	if(shift == F64_SIZE){
		buffer128[0]	= 0;
		buffer128[1]	= value;
	} else if(shift == 0){
		return;
	} else {
		buffer128[0] 	= value << shift;
		buffer128[1] 	= value >> (F64_SIZE - shift);
	}
}

void add128(uint64_t buffer128_1[BUFFER_128_SIZE], uint64_t buffer128_2[BUFFER_128_SIZE]){
	uint8_t sign1;
	uint8_t sign2;
	uint8_t carry;

	sign1 = (buffer128_1[0] & I64_SIGN) >> (F64_SIZE - 1);	// I64_SIGN = 0x8000000000000000
	sign2 = (buffer128_2[0] & I64_SIGN) >> (F64_SIZE - 1);  // F64_SIZE = 64
	carry = sign1 ^ sign2;
	buffer128_1[0] = buffer128_1[0] + buffer128_2[0];
	buffer128_1[1] = buffer128_1[1] + buffer128_2[1] + carry;
}

void print128(uint64_t buffer128[BUFFER_128_SIZE]){
	printf("%016lX - %016lX\n", buffer128[1], buffer128[0]);
}
*/
