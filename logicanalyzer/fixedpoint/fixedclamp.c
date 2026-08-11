#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

fixed32_t  fixedReLU32( fixed32_t x){
	if(x < F32_ZERO){
		x = F32_ZERO;
	}

	return x;
}

ufixed32_t ufixedReLU32(ufixed32_t x){
	return x;
}

fixed64_t  fixedReLU64( fixed64_t x){
	if(x < F64_ZERO){
		x = F64_ZERO;
	}

	return x;
}

ufixed64_t ufixedReLU64(ufixed64_t x){
	return x;
}

fixed32_t  fixedClamp32( fixed32_t x,  fixed32_t min,  fixed32_t max){
	if(x <= min){
		x = min;
	} else if (x >= max){
		x = max;
	}

	return x;
}

ufixed32_t ufixedClamp32(ufixed32_t x, ufixed32_t min, ufixed32_t max){
	if(x <= min){
		x = min;
	} else if (x >= max){
		x = max;
	}

	return x;
}

fixed64_t  fixedClamp64( fixed64_t x,  fixed64_t min,  fixed64_t max){
	if(x <= min){
		x = min;
	} else if (x >= max){
		x = max;
	}

	return x;
}

ufixed64_t ufixedClamp64(ufixed64_t x, ufixed64_t min, ufixed64_t max){
	if(x <= min){
		x = min;
	} else if (x >= max){
		x = max;
	}

	return x;
}
