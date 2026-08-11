#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

fixed32_t  fixedMod32( fixed32_t x,  fixed32_t mod){
	// Special case: mod = 0 returns 0
	if(mod == F32_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return F32_ZERO;
	}
	
	// Converts mod to positive if negative
	uint8_t modsign = 0;
	if(mod < F32_ZERO){
		mod = fixedNeg32(mod);
		modsign = 1;
	}

	// Converts x to positive if negative
	uint8_t xsign = 0;
	if(x < F32_ZERO){
		x = fixedNeg32(x);
		xsign = 1;
	}

	// Actual mod
	fixed32_t y = x % mod;

	// Handles negative sign for x
	if(xsign){
		y = mod - y;
	}

	// Handles positive sign for mod
	if(modsign){
		y = mod - y;
	}

	// If y = mod (due to x being negative, or mod being negative), returns 0
	if(y == mod){
		y = F32_ZERO;
	}

	return y;
}

ufixed32_t ufixedMod32(ufixed32_t x, ufixed32_t mod){
	return fixedMod32(x, mod);
}

fixed64_t  fixedMod64( fixed64_t x,  fixed64_t mod){
	// Special case: mod = 0 returns 0
	if(mod == F64_ZERO){
		fixedSetErrno(FP_ERROR_ZERO);
		return F64_ZERO;
	}
	
	// Converts mod to positive if negative
	uint8_t modsign = 0;
	if(mod < F64_ZERO){
		mod = fixedNeg64(mod);
		modsign = 1;
	}

	// Converts x to positive if negative
	uint8_t xsign = 0;
	if(x < F64_ZERO){
		x = fixedNeg64(x);
		xsign = 1;
	}

	// Actual mod
	fixed64_t y = x % mod;

	// Handles negative sign for x
	if(xsign){
		y = mod - y;
	}

	// Handles positive sign for mod
	if(modsign){
		y = mod - y;
	}

	// If y = mod (due to x being negative, or mod being negative), returns 0
	if(y == mod){
		y = F64_ZERO;
	}

	return y;
};

ufixed64_t ufixedMod64(ufixed64_t x, ufixed64_t mod){
	return fixedMod64(x, mod);
}
