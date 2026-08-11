#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>

int8_t fixedCmp32(fixed32_t x1, fixed32_t x2){
	if(x1 > x2){
		return 1;
	} else if(x1 < x2){
		return -1;
	}
	return 0;
}

int8_t ufixedCmp32(ufixed32_t x1, ufixed32_t x2){
	if(x1 > x2){
		return 1;
	} else if(x1 < x2){
		return -1;
	}
	return 0;
}

int8_t fixedCmp64(fixed64_t x1, fixed64_t x2){
	if(x1 > x2){
		return 1;
	} else if(x1 < x2){
		return -1;
	}
	return 0;
}

int8_t ufixedCmp64(ufixed64_t x1, ufixed64_t x2){
	if(x1 > x2){
		return 1;
	} else if(x1 < x2){
		return -1;
	}
	return 0;
}
