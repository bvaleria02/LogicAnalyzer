#include "fixedpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "fixedtrig/fixedmagnitude.h"

fixed32_t  fixedMag32( fixed32_t x,  fixed32_t y){
	x = fixedAbs32(x);
	y = fixedAbs32(y);
	return ufixedMag32(x, y);
}

ufixed32_t ufixedMag32(ufixed32_t x, ufixed32_t y){
	return abstractMagnitudeCORDIC32(x, y, NULL);
}

fixed64_t  fixedMag64( fixed64_t x,  fixed64_t y){
	x = fixedAbs64(x);
	y = fixedAbs64(y);
	return ufixedMag64(x, y);
}

ufixed64_t ufixedMag64(ufixed64_t x, ufixed64_t y){
	return abstractMagnitudeCORDIC64(x, y, NULL);
}

fixed32_t  fixedMag32h( fixed32_t x,  fixed32_t y){
	x = fixedAbs32(x);
	y = fixedAbs32(y);
	return ufixedMagh32(x, y);
}

ufixed32_t ufixedMagh32(ufixed32_t x, ufixed32_t y){
	return abstractMagnitudeHyperCORDIC32(x, y, NULL);
}

fixed64_t  fixedMagh64( fixed64_t x,  fixed64_t y){
	x = fixedAbs64(x);
	y = fixedAbs64(y);
	return ufixedMagh64(x, y);
}

ufixed64_t ufixedMagh64(ufixed64_t x, ufixed64_t y){
	return abstractMagnitudeHyperCORDIC64(x, y, NULL);
}
