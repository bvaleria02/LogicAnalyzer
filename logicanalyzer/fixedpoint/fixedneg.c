#include "fixedpoint.h"
#include <stdio.h>
#include <stdint.h>

fixed32_t fixedNeg32(fixed32_t x){
	return -x;
}

ufixed32_t ufixedNeg32(ufixed32_t x){
	return (~(x) + 1);
}

fixed64_t fixedNeg64(fixed64_t x){
	return -x;
}

ufixed64_t ufixedNeg64(ufixed64_t x){
	return (~(x) + 1);
}
