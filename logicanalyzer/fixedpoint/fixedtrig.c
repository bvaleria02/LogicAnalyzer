#include <stdint.h>
#include <stdio.h>
#include "fixedpoint.h"
#include "fixedtrig/fixedsine.h"
#include "fixedtrig/fixedatan2.h"
#include "fixedtrig/fixedtan.h"
#include "utils/fixedlimits.h"
#include "utils/atan2utils.h"
#include "fixedtrig/fixedasine.h"
#include "fixedtrig/fixedsinehyper.h"
#include "fixedtrig/fixedmagnitude.h"

fixed32_t  fixedSin32( fixed32_t x){
	uint32_t mul = 1;
	if(x < 0){
		x = x * -1;
		mul = -1;
	}

#ifdef USE_CORDIC_OVER_TAYLOR
	return abstractSineCORDIC32(x) * mul;
#else
	return abstractSineTaylor32(x) * mul;
#endif
}

ufixed32_t ufixedSin32(ufixed32_t x){
#ifdef USE_CORDIC_OVER_TAYLOR
	return abstractSineCORDIC32(x);
#else
	return abstractSineTaylor32(x);
#endif
}

fixed32_t  fixedSinAlt32( fixed32_t x){
	uint32_t mul = 1;
	if(x < 0){
		x = x * -1;
		mul = -1;
	}

#ifdef USE_CORDIC_OVER_TAYLOR
	return abstractSineTaylor32(x) * mul;
#else
	return abstractSineCORDIC32(x) * mul;
#endif
}

ufixed32_t ufixedSinAlt32(ufixed32_t x){
#ifdef USE_CORDIC_OVER_TAYLOR
	return abstractSineTaylor32(x);
#else
	return abstractSineCORDIC32(x);
#endif
}

fixed64_t  fixedSin64( fixed64_t x){
	int64_t mul = 1; 
	if(x < 0){
		x = x * -1;
		mul = -1;
	}
	
#ifdef USE_CORDIC_OVER_TAYLOR
	return abstractSineCORDIC64(x) * mul;
#else
	return abstractSineTaylor64(x) * mul;
#endif
}

ufixed64_t ufixedSin64(ufixed64_t x){
#ifdef USE_CORDIC_OVER_TAYLOR
	return abstractSineCORDIC64(x);
#else
	return abstractSineTaylor64(x);
#endif
}

fixed64_t  fixedSinAlt64( fixed64_t x){
	int64_t mul = 1; 
	if(x < 0){
		x = x * -1;
		mul = -1;
	}
	
#ifdef USE_CORDIC_OVER_TAYLOR
	return abstractSineTaylor64(x) * mul;
#else
	return abstractSineCORDIC64(x) * mul;
#endif
}

ufixed64_t ufixedSinAlt64(ufixed64_t x){
#ifdef USE_CORDIC_OVER_TAYLOR
	return abstractSineTaylor64(x);
#else
	return abstractSineCORDIC64(x);
#endif
}


fixed32_t  fixedCos32( fixed32_t x){
#ifdef USE_CORDIC_OVER_TAYLOR
	return abstractSineCORDIC32(x + F32_HALF_PI);
#else
	return abstractSineTaylor32(x + F32_HALF_PI);
#endif
}

ufixed32_t ufixedCos32(ufixed32_t x){
#ifdef USE_CORDIC_OVER_TAYLOR
	return abstractSineCORDIC32(x + F32_HALF_PI);
#else
	return abstractSineTaylor32(x + F32_HALF_PI);
#endif
}

fixed32_t  fixedCosAlt32( fixed32_t x){
#ifdef USE_CORDIC_OVER_TAYLOR
	return abstractSineTaylor32(x + F32_HALF_PI);
#else
	return abstractSineCORDIC32(x + F32_HALF_PI);
#endif
}

ufixed32_t ufixedCosAlt32(ufixed32_t x){
#ifdef USE_CORDIC_OVER_TAYLOR
	return abstractSineTaylor32(x + F32_HALF_PI);
#else
	return abstractSineCORDIC32(x + F32_HALF_PI);
#endif
}

fixed64_t  fixedCos64( fixed64_t x){
#ifdef USE_CORDIC_OVER_TAYLOR
	return abstractSineCORDIC64(x + F64_HALF_PI);
#else
	return abstractSineTaylor64(x + F64_HALF_PI);
#endif
}

ufixed64_t ufixedCos64(ufixed64_t x){
#ifdef USE_CORDIC_OVER_TAYLOR
	return abstractSineCORDIC64(x + F64_HALF_PI);
#else
	return abstractSineTaylor64(x + F64_HALF_PI);
#endif
}

fixed64_t  fixedCosAlt64( fixed64_t x){
#ifdef USE_CORDIC_OVER_TAYLOR
	return abstractSineTaylor64(x + F64_HALF_PI);
#else
	return abstractSineCORDIC64(x + F64_HALF_PI);
#endif
}

ufixed64_t ufixedCosAlt64(ufixed64_t x){
#ifdef USE_CORDIC_OVER_TAYLOR
	return abstractSineTaylor64(x + F64_HALF_PI);
#else
	return abstractSineCORDIC64(x + F64_HALF_PI);
#endif
}

// arctan

fixed32_t fixedAtan32(fixed32_t x){
	return abstractAtan2CORDIC32(x, F32_ONE);
}

ufixed32_t ufixedAtan32(ufixed32_t x){
	return abstractAtan2CORDIC32(x, F32_ONE);
}

fixed64_t fixedAtan64(fixed64_t x){
	return abstractAtan2CORDIC64(x, F64_ONE);
}

ufixed64_t ufixedAtan64(ufixed64_t x){
	return abstractAtan2CORDIC64(x, F64_ONE);
}

// atan

fixed32_t fixedAtan232(fixed32_t y, fixed32_t x){
	int8_t quadrant 	= getQuadrant32(&y, &x);
	fixed32_t angle  	= abstractAtan2CORDIC32(y, x);
	return angleQuadrantAtan32(quadrant, angle);
}

ufixed32_t ufixedAtan232(ufixed32_t y, ufixed32_t x){
	return abstractAtan2CORDIC32(y, x);
}

fixed64_t fixedAtan264(fixed64_t y, fixed64_t x){
	int8_t quadrant 	= getQuadrant64(&y, &x);
	fixed64_t angle  	= abstractAtan2CORDIC64(y, x);
	return angleQuadrantAtan64(quadrant, angle);
}

ufixed64_t ufixedAtan264(ufixed64_t y, ufixed64_t x){
	return abstractAtan2CORDIC64(y, x);
}

// tan

fixed32_t fixedTan32(fixed32_t x){
	return abstractTanLUT32(x);
}

ufixed32_t ufixedTan32(ufixed32_t x){
	return abstractTanLUT32(x);
}

fixed64_t fixedTan64(fixed64_t x){
	return abstractTanLUT64(x);
}

ufixed64_t ufixedTan64(ufixed64_t x){
	return abstractTanLUT64(x);
}

// asin

ufixed32_t ufixedAsin32(ufixed32_t x){
	return abstractAsin32CORDIC(x);
}

fixed32_t fixedAsin32(fixed32_t x){
	return abstractAsin32CORDIC(x);
}

ufixed64_t ufixedAsin64(ufixed64_t x){
	return abstractAsin64CORDIC(x);
}

fixed64_t fixedAsin64(fixed64_t x){
	return abstractAsin64CORDIC(x);
}

// acos

ufixed32_t ufixedAcos32(ufixed32_t x){
	return abstractAcos32CORDIC(x);
}

fixed32_t fixedAcos32(fixed32_t x){
	return abstractAcos32CORDIC(x);
}

ufixed64_t ufixedAcos64(ufixed64_t x){
	return abstractAcos64CORDIC(x);
}

fixed64_t fixedAcos64(fixed64_t x){
	return abstractAcos64CORDIC(x);
}

// sinh

fixed32_t fixedSinh32(fixed32_t angle){
	fixed32_t sinh;
	fixed32_t cosh;

	abstractSineHyperbolicCORDIC32(angle, &sinh, &cosh);

	(void) cosh;
	return sinh;
}

ufixed32_t ufixedSinh32(ufixed32_t angle){
	fixed32_t sinh;
	fixed32_t cosh;

	abstractSineHyperbolicCORDIC32(angle, &sinh, &cosh);

	(void) cosh;
	return sinh;
}

fixed64_t fixedSinh64(fixed64_t angle){
	fixed64_t sinh;
	fixed64_t cosh;

	abstractSineHyperbolicCORDIC64(angle, &sinh, &cosh);

	(void) cosh;
	return sinh;
}

ufixed64_t ufixedSinh64(ufixed64_t angle){
	fixed64_t sinh;
	fixed64_t cosh;

	abstractSineHyperbolicCORDIC64(angle, &sinh, &cosh);

	(void) cosh;
	return sinh;
}

fixed32_t fixedCosh32(fixed32_t angle){
	fixed32_t sinh;
	fixed32_t cosh;

	abstractSineHyperbolicCORDIC32(angle, &sinh, &cosh);

	(void) sinh;
	return cosh;
}

ufixed32_t ufixedCosh32(ufixed32_t angle){
	fixed32_t sinh;
	fixed32_t cosh;

	abstractSineHyperbolicCORDIC32(angle, &sinh, &cosh);

	(void) sinh;
	return cosh;
}

fixed64_t fixedCosh64(fixed64_t angle){
	fixed64_t sinh;
	fixed64_t cosh;

	abstractSineHyperbolicCORDIC64(angle, &sinh, &cosh);

	(void) sinh;
	return cosh;
}

ufixed64_t ufixedCosh64(ufixed64_t angle){
	fixed64_t sinh;
	fixed64_t cosh;

	abstractSineHyperbolicCORDIC64(angle, &sinh, &cosh);

	(void) sinh;
	return cosh;
}

fixed32_t  fixedATanh32( fixed32_t x){
	uint8_t sign = 0;
	if(x < F32_ZERO){
		x = fixedAbs32(x);
		sign = 1;
	}

	fixed32_t y = ufixedATanh32(x);

	if(sign){
		y = fixedNeg32(y);
	}

	return y;
}

ufixed32_t ufixedATanh32(ufixed32_t x){
	fixed32_t atanh;
	abstractMagnitudeHyperCORDIC32(F32_ONE, x, &atanh);
	return atanh;
}

fixed64_t  fixedATanh64( fixed64_t x){
	uint8_t sign = 0;
	if(x < F64_ZERO){
		x = fixedAbs64(x);
		sign = 1;
	}

	fixed64_t y = ufixedATanh64(x);

	if(sign){
		y = fixedNeg64(y);
	}

	return y;
}

ufixed64_t ufixedATanh64(ufixed64_t x){
	fixed64_t atanh;
	abstractMagnitudeHyperCORDIC64(F64_ONE, x, &atanh);
	return atanh;
}

fixed32_t  fixedATanh232( fixed32_t x,  fixed32_t y){
	return ufixedATanh232(x, y);
}

ufixed32_t ufixedATanh232(ufixed32_t x, ufixed32_t y){
	fixed32_t atanh;
	abstractMagnitudeHyperCORDIC32(x, y, &atanh);
	return atanh;
}

fixed64_t  fixedATanh264( fixed64_t x,  fixed64_t y){
	return ufixedATanh264(x, y);
}

ufixed64_t ufixedATanh264(ufixed64_t x, ufixed64_t y){
	fixed64_t atanh;
	abstractMagnitudeHyperCORDIC64(x, y, &atanh);
	return atanh;
}
