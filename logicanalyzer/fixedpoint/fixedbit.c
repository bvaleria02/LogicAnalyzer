#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>

uint8_t fixedGetBit32(fixed32_t x, uint8_t pos){
	return (x >> pos) & 0x1;
}

uint8_t ufixedGetBit32(ufixed32_t x, uint8_t pos){
	return (x >> pos) & 0x1;
}

uint8_t fixedGetBit64(fixed64_t x, uint8_t pos){
	return (x >> pos) & 0x1;
}

uint8_t ufixedGetBit64(ufixed64_t x, uint8_t pos){
	return (x >> pos) & 0x1;
}

fixed32_t fixedSetBit32(fixed32_t x, uint8_t pos, uint8_t value){
	return (x & ~((uint32_t) 1 << pos)) | ((uint32_t)(value & 0x1) << pos);
}

ufixed32_t ufixedSetBit32(ufixed32_t x, uint8_t pos, uint8_t value){
	return (x & ~((uint32_t) 1 << pos)) | ((uint32_t)(value & 0x1) << pos);
}

fixed64_t fixedSetBit64(fixed64_t x, uint8_t pos, uint8_t value){
	return (x & ~((uint64_t) 1 << pos)) | ((uint64_t)(value & 0x1) << pos);
}

ufixed64_t ufixedSetBit64(ufixed64_t x, uint8_t pos, uint8_t value){
	return (x & ~((uint64_t) 1 << pos)) | ((uint64_t)(value & 0x1) << pos);
}
