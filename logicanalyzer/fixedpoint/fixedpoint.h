#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H

#include <stdint.h>


#ifndef USE_VAR
	#define USE_VAR(x) (void) x
#endif // USE_VAR

#ifndef TRUE
	#define TRUE 1
#endif // TRUE

#ifndef FALSE
	#define FALSE 0
#endif // FALSE

/*
#define DEBUG_MODE 1
#define DEBUG_EXTRA_MODE 1
*/

#define TARGET_CAN_PRINTF 1
#define USE_CORDIC_OVER_TAYLOR

#define F32_SIZE		32
#define	F64_SIZE		64

#define F32_LSHIFT 		16
#define F64_LSHIFT 		32
#define F32_RSHIFT 		16
#define F64_RSHIFT 		32

#define F32_LMASK   	0x0000FFFF
#define F64_LMASK  		0x00000000FFFFFFFF
#define F32_HMASK   	0xFFFF0000
#define F64_HMASK   	0xFFFFFFFF00000000

#define F32_SCALE   	0x00010000
#define F64_SCALE   	0x0000000100000000

#define I32_SIGN		0x80000000
#define I32_MANTISSA	0x7FFFFFFF
#define I64_SIGN		0x8000000000000000
#define I64_MANTISSA	0x7FFFFFFFFFFFFFFF

#define F32_MAX_VALUE	0x7FFFFFFF
#define UF32_MAX_VALUE	0xFFFFFFFF
#define F64_MAX_VALUE	0x7FFFFFFFFFFFFFFF
#define UF64_MAX_VALUE	0xFFFFFFFFFFFFFFFF

#define F32_MIN_VALUE	0x80000000
#define UF32_MIN_VALUE	0x00000000
#define F64_MIN_VALUE	0x8000000000000000
#define UF64_MIN_VALUE	0x0000000000000000

// 2 << 6 = 64
#define U64_SIZE		6
#define U64_MASK		0x3F
#define U32_SIZE		5
#define U32_MASK		0x1F

#define GET_SIGN(x, f)  (x & f)
#define GET_MANT(x, f)  (x & f)

#define SQRT_S32_MAX 0x00b504f3
#define SQRT_U32_MAX 0x01000000
#define SQRT_S64_MAX 0x0000b504f333f92b
#define SQRT_U64_MAX 0x0001000000000000

#define F32_LN_2 0x0000b172
#define F64_LN_2 0x00000000b17217f7
#define F32_INV_LN_2 0x00017154
#define F64_INV_LN_2 0x0000000171547652
#define F32_LOG_10_2 0x00004d10
#define F64_LOG_10_2 0x000000004d104d42

#define F32_ZERO 0x00000000
#define F64_ZERO 0x0000000000000000

#define F32_ONE 0x00010000
#define F64_ONE 0x0000000100000000

#define F32_SQRT_2 0x00016A0A
#define F64_SQRT_2 0x000000016A09E667

#define F32_PI 0x0003243f
#define F64_PI 0x00000003243f6a88

#define F32_E 0x0002b7e1
#define F64_E 0x00000002b7e15162

#define CORDIC_CONST_F32 0x00009B74
#define CORDIC_CONST_F64 0x000000009B74EDA8
#define CORDIC_CONST_INV_F32 0x0001A592
#define CORDIC_CONST_INV_F64 0x00000001A592148C

#define F32_ERROR_VALUE 0x7FFFFFFF
#define F64_ERROR_VALUE 0x7FFFFFFFFFFFFFFF

#define ASSEMBLE_FIXED(size, sgn, mantissa, point, dest, mask) do{ \
	dest = (mantissa << ((size >> 1) - point)) & mask; 		\
	dest = dest | sgn;										\
	if(point <= 0){											\
		break;												\
	}														\
	for(uint8_t _i = 0; _i < point; _i++){					\
		if(sgn == I32_SIGN || sgn == I64_SIGN){				\
			dest = dest | (0x1 << (size - 1 - point + _i));	\
		};													\
	};														\
} while(0)

#define ASSEMBLE_UFIXED(size, x, point, dest) do{ \
	dest = (x << ((size >> 1) - point)); 			\
} while(0)

#define CMP_GREATER 1
#define CMP_EQUAL 0
#define CMP_LESS -1

typedef int32_t   fixed32_t;
typedef int64_t   fixed64_t;
typedef uint32_t ufixed32_t;
typedef uint64_t ufixed64_t;

typedef enum _FP_ERROR_ {
	FP_NO_ERROR 		= 0,
	FP_ERROR_NULLPTR	= 1,
	FP_ERROR_OUTOFRANGE	= 2,
	FP_ERROR_NEGATIVE	= 3,
	FP_ERROR_OVERFLOW	= 4,
	FP_ERROR_UNDERFLOW	= 5,
	FP_ERROR_INFINITY	= 6,
	FP_ERROR_ZERO		= 7
} FPErrorCode;

typedef uint8_t FPErrno;
extern _Thread_local FPErrno fp_errno;

// src/fixedpoint/fixedconstans.c
//#include "fixedconstants.h"

// src/fixedpoint/convert.c
 fixed32_t convertFloatToF32(float x);
 fixed64_t convertFloatToF64(float x);
ufixed32_t convertFloatToUF32(float x);
ufixed64_t convertFloatToUF64(float x);

 fixed32_t convertDoubleToF32(double x);
 fixed64_t convertDoubleToF64(double x);
ufixed32_t convertDoubleToUF32(double x);
ufixed64_t convertDoubleToUF64(double x);

 fixed32_t convertIntToF32(  int32_t x, int8_t point);
 fixed64_t convertIntToF64(  int64_t x, int8_t point);
ufixed32_t convertIntToUF32(uint32_t x, int8_t point);
ufixed64_t convertIntToUF64(uint64_t x, int8_t point);

float convertF32ToFloat(  fixed32_t x);
float convertF64ToFloat(  fixed64_t x);
float convertUF32ToFloat(ufixed32_t x);
float convertUF64ToFloat(ufixed64_t x);

double convertF32ToDouble(  fixed32_t x);
double convertF64ToDouble(  fixed64_t x);
double convertUF32ToDouble(ufixed32_t x);
double convertUF64ToDouble(ufixed64_t x);

 int32_t convertF32ToInt(  fixed32_t x, int8_t point);
 int64_t convertF64ToInt(  fixed64_t x, int8_t point);
uint32_t convertUF32ToInt(ufixed32_t x, int8_t point);
uint64_t convertUF64ToInt(ufixed64_t x, int8_t point);

// src/fixedpoint/fixedadd.c
 fixed32_t  fixedAdd32( fixed32_t x1,  fixed32_t x2);
 fixed64_t  fixedAdd64( fixed64_t x1,  fixed64_t x2);
ufixed32_t ufixedAdd32(ufixed32_t x1, ufixed32_t x2);
ufixed64_t ufixedAdd64(ufixed64_t x1, ufixed64_t x2);
 fixed32_t  fixedAddScalar32( fixed32_t x1,  int32_t x2, int8_t scale);
 fixed64_t  fixedAddScalar64( fixed64_t x1,  int64_t x2, int8_t scale);
ufixed32_t ufixedAddScalar32(ufixed32_t x1, uint32_t x2, int8_t scale);
ufixed64_t ufixedAddScalar64(ufixed64_t x1, uint64_t x2, int8_t scale);
 fixed32_t  fixedAddOF32( fixed32_t x1,  fixed32_t x2);
 fixed64_t  fixedAddOF64( fixed64_t x1,  fixed64_t x2);
ufixed32_t ufixedAddOF32(ufixed32_t x1, ufixed32_t x2);
ufixed64_t ufixedAddOF64(ufixed64_t x1, ufixed64_t x2);

// src/fixedpoint/fixedsub.c
 fixed32_t  fixedSub32( fixed32_t x1,  fixed32_t x2);
 fixed64_t  fixedSub64( fixed64_t x1,  fixed64_t x2);
ufixed32_t ufixedSub32(ufixed32_t x1, ufixed32_t x2);
ufixed64_t ufixedSub64(ufixed64_t x1, ufixed64_t x2);
 fixed32_t  fixedSubScalar32( fixed32_t x1,  int32_t x2, int8_t scale);
 fixed64_t  fixedSubScalar64( fixed64_t x1,  int64_t x2, int8_t scale);
ufixed32_t ufixedSubScalar32(ufixed32_t x1, uint32_t x2, int8_t scale);
ufixed64_t ufixedSubScalar64(ufixed64_t x1, uint64_t x2, int8_t scale);
 fixed32_t  fixedSubUF32( fixed32_t x1,  fixed32_t x2);
 fixed64_t  fixedSubUF64( fixed64_t x1,  fixed64_t x2);
ufixed32_t ufixedSubUF32(ufixed32_t x1, ufixed32_t x2);
ufixed64_t ufixedSubUF64(ufixed64_t x1, ufixed64_t x2);

// src/fixedpoint/fixedmul.c
 fixed32_t  fixedMul32( fixed32_t x1,  fixed32_t x2);
 fixed64_t  fixedMul64( fixed64_t x1,  fixed64_t x2);
ufixed32_t ufixedMul32(ufixed32_t x1, ufixed32_t x2);
ufixed64_t ufixedMul64(ufixed64_t x1, ufixed64_t x2);

// src/fixedpoint/fixeddiv.c
 fixed32_t  fixedDiv32( fixed32_t x1,  fixed32_t x2);
ufixed32_t ufixedDiv32(ufixed32_t x1, ufixed32_t x2);
 fixed64_t  fixedDiv64( fixed64_t x1,  fixed64_t x2);
ufixed64_t ufixedDiv64(ufixed64_t x1, ufixed64_t x2);

// src/fixedpoint/fixedcmp.c
int8_t fixedCmp32(fixed32_t x1, fixed32_t x2);
int8_t ufixedCmp32(ufixed32_t x1, ufixed32_t x2);
int8_t fixedCmp64(fixed64_t x1, fixed64_t x2);
int8_t ufixedCmp64(ufixed64_t x1, ufixed64_t x2);

// src/fixedpoint/fixedsqrt.c
 fixed32_t  fixedSqrt32( fixed32_t x);
ufixed32_t ufixedSqrt32(ufixed32_t x);
 fixed64_t  fixedSqrt64( fixed64_t x);
ufixed64_t ufixedSqrt64(ufixed64_t x);

// src/fixedpoint/fixedbit.c
uint8_t  fixedGetBit32( fixed32_t x, uint8_t pos);
uint8_t ufixedGetBit32(ufixed32_t x, uint8_t pos);
uint8_t  fixedGetBit64( fixed64_t x, uint8_t pos);
uint8_t ufixedGetBit64(ufixed64_t x, uint8_t pos);

 fixed32_t  fixedSetBit32( fixed32_t x, uint8_t pos, uint8_t value);
ufixed32_t ufixedSetBit32(ufixed32_t x, uint8_t pos, uint8_t value);
 fixed64_t  fixedSetBit64( fixed64_t x, uint8_t pos, uint8_t value);
ufixed64_t ufixedSetBit64(ufixed64_t x, uint8_t pos, uint8_t value);

// src/fixedpoint/fixedln.c
 fixed32_t  fixedLn32( fixed32_t x);
ufixed32_t ufixedLn32(ufixed32_t x);
 fixed64_t  fixedLn64( fixed64_t x);
ufixed64_t ufixedLn64(ufixed64_t x);
 fixed32_t  fixedLog232( fixed32_t x);
ufixed32_t ufixedLog232(ufixed32_t x);
 fixed64_t  fixedLog264( fixed64_t x);
ufixed64_t ufixedLog264(ufixed64_t x);
 fixed32_t  fixedLog1032( fixed32_t x);
ufixed32_t ufixedLog1032(ufixed32_t x);
 fixed64_t  fixedLog1064( fixed64_t x);
ufixed64_t ufixedLog1064(ufixed64_t x);

// src/fixedpoint/fixedtrig.c
fixed32_t  fixedSin32( fixed32_t x);
ufixed32_t ufixedSin32(ufixed32_t x);
 fixed32_t  fixedSinAlt32( fixed32_t x);
ufixed32_t ufixedSinAlt32(ufixed32_t x);
 fixed64_t  fixedSin64( fixed64_t x);
ufixed64_t ufixedSin64(ufixed64_t x);
 fixed64_t  fixedSinAlt64( fixed64_t x);
ufixed64_t ufixedSinAlt64(ufixed64_t x);
 fixed32_t  fixedCos32( fixed32_t x);
ufixed32_t ufixedCos32(ufixed32_t x);
 fixed32_t  fixedCosAlt32( fixed32_t x);
ufixed32_t ufixedCosAlt32(ufixed32_t x);
 fixed64_t  fixedCos64( fixed64_t x);
ufixed64_t ufixedCos64(ufixed64_t x);
 fixed64_t  fixedCosAlt64( fixed64_t x);
ufixed64_t ufixedCosAlt64(ufixed64_t x);
 fixed32_t  fixedAtan32(  fixed32_t x);
ufixed32_t ufixedAtan32( ufixed32_t x);
 fixed64_t  fixedAtan64(  fixed64_t x);
ufixed64_t ufixedAtan64( ufixed64_t x);
 fixed32_t  fixedAtan232( fixed32_t y,  fixed32_t x);
ufixed32_t ufixedAtan232(ufixed32_t y, ufixed32_t x);
 fixed64_t  fixedAtan264( fixed64_t y,  fixed64_t x);
ufixed64_t ufixedAtan264(ufixed64_t y, ufixed64_t x);
 fixed32_t  fixedTan32( fixed32_t x);
ufixed32_t ufixedTan32(ufixed32_t x);
 fixed64_t  fixedTan64( fixed64_t x);
ufixed64_t ufixedTan64(ufixed64_t x);
 fixed32_t  fixedAsin32( fixed32_t x);
ufixed32_t ufixedAsin32(ufixed32_t x);
 fixed64_t  fixedAsin64( fixed64_t x);
ufixed64_t ufixedAsin64(ufixed64_t x);
 fixed32_t  fixedAcos32( fixed32_t x);
ufixed32_t ufixedAcos32(ufixed32_t x);
 fixed64_t  fixedAcos64( fixed64_t x);
ufixed64_t ufixedAcos64(ufixed64_t x);
fixed32_t  fixedSinh32( fixed32_t x);
ufixed32_t ufixedSinh32(ufixed32_t x);
 fixed64_t  fixedSinh64( fixed64_t x);
ufixed64_t ufixedSinh64(ufixed64_t x);
 fixed32_t  fixedCosh32( fixed32_t x);
ufixed32_t ufixedCosh32(ufixed32_t x);
 fixed64_t  fixedCosh64( fixed64_t x);
ufixed64_t ufixedCosh64(ufixed64_t x);
 fixed32_t  fixedATanh32( fixed32_t x);
ufixed32_t ufixedATanh32(ufixed32_t x);
 fixed64_t  fixedATanh64( fixed64_t x);
ufixed64_t ufixedATanh64(ufixed64_t x);
 fixed32_t  fixedATanh232( fixed32_t x,  fixed32_t y);
ufixed32_t ufixedATanh232(ufixed32_t x, ufixed32_t y);
 fixed64_t  fixedATanh264( fixed64_t x,  fixed64_t y);
ufixed64_t ufixedATanh264(ufixed64_t x, ufixed64_t y);

// src/fixedpoint/fixedpow.c
 fixed32_t  fixedPow32( fixed32_t x, uint16_t n);
ufixed32_t ufixedPow32(ufixed32_t x, uint16_t n);
 fixed64_t  fixedPow64( fixed64_t x, uint16_t n);
ufixed64_t ufixedPow64(ufixed64_t x, uint16_t n);
 fixed32_t  fixedPowf32( fixed32_t x,  fixed32_t n);
ufixed32_t ufixedPowf32(ufixed32_t x, ufixed32_t n);
 fixed64_t  fixedPowf64( fixed64_t x,  fixed64_t n);
ufixed64_t ufixedPowf64(ufixed64_t x, ufixed64_t n);

// src/fixedpoint/fixedexp.c
 fixed32_t  fixedExp32( fixed32_t x);
ufixed32_t ufixedExp32(ufixed32_t x);
 fixed64_t  fixedExp64( fixed64_t x);
ufixed64_t ufixedExp64(ufixed64_t x);

// src/fixedpoint/fixedneg.c
 fixed32_t  fixedNeg32( fixed32_t x);
ufixed32_t ufixedNeg32(ufixed32_t x);
 fixed64_t  fixedNeg64( fixed64_t x);
ufixed64_t ufixedNeg64(ufixed64_t x);

// src/fixedpoint/fixedrsqrt.c
 fixed32_t  fixedRSqrt32( fixed32_t x);
ufixed32_t ufixedRSqrt32(ufixed32_t x);
 fixed64_t  fixedRSqrt64( fixed64_t x);
ufixed64_t ufixedRSqrt64(ufixed64_t x);

// src/fixedpoint/fixedlerp.c
 fixed32_t  fixedLerp32( fixed32_t y0,  fixed32_t y1, ufixed32_t x);
ufixed32_t ufixedLerp32(ufixed32_t y0, ufixed32_t y1, ufixed32_t x);
 fixed64_t  fixedLerp64( fixed64_t y0,  fixed64_t y1, ufixed64_t x);
ufixed64_t ufixedLerp64(ufixed64_t y0, ufixed64_t y1, ufixed64_t x);

// src/fixedpoint/fixedcserp.c
 fixed32_t  fixedCSerp32( fixed32_t y0,  fixed32_t y1,  fixed32_t y2,  fixed32_t y3, ufixed32_t x);
ufixed32_t ufixedCSerp32(ufixed32_t y0, ufixed32_t y1, ufixed32_t y2, ufixed32_t y3, ufixed32_t x);
 fixed64_t  fixedCSerp64( fixed64_t y0,  fixed64_t y1,  fixed64_t y2,  fixed64_t y3, ufixed64_t x);
ufixed64_t ufixedCSerp64(ufixed64_t y0, ufixed64_t y1, ufixed64_t y2, ufixed64_t y3, ufixed64_t x);

// src/fixedpoint/fixedlaerp.c
 fixed32_t  fixedLaerp32( fixed32_t y0,  fixed32_t y1,  fixed32_t y2,  fixed32_t y3, ufixed32_t x);
ufixed32_t ufixedLaerp32(ufixed32_t y0, ufixed32_t y1, ufixed32_t y2, ufixed32_t y3, ufixed32_t x);
 fixed64_t  fixedLaerp64( fixed64_t y0,  fixed64_t y1,  fixed64_t y2,  fixed64_t y3, ufixed64_t x);
ufixed64_t ufixedLaerp64(ufixed64_t y0, ufixed64_t y1, ufixed64_t y2, ufixed64_t y3, ufixed64_t x);

// src/fixedpoint/fixednerp.c
 fixed32_t  fixedNerp32( fixed32_t y0,  fixed32_t y1,  fixed32_t y2,  fixed32_t y3,  fixed32_t y4,  fixed32_t y5,  fixed32_t x);
ufixed32_t ufixedNerp32(ufixed32_t y0, ufixed32_t y1, ufixed32_t y2, ufixed32_t y3, ufixed32_t y4, ufixed32_t y5, ufixed32_t x);
 fixed64_t  fixedNerp64( fixed64_t y0,  fixed64_t y1,  fixed64_t y2,  fixed64_t y3,  fixed64_t y4,  fixed64_t y5,  fixed64_t x);
ufixed64_t ufixedNerp64(ufixed64_t y0, ufixed64_t y1, ufixed64_t y2, ufixed64_t y3, ufixed64_t y4, ufixed64_t y5, ufixed64_t x);

// src/fixedpoint/fixedmod.c
 fixed32_t  fixedMod32( fixed32_t x,  fixed32_t mod);
ufixed32_t ufixedMod32(ufixed32_t x, ufixed32_t mod);
 fixed64_t  fixedMod64( fixed64_t x,  fixed64_t mod);
ufixed64_t ufixedMod64(ufixed64_t x, ufixed64_t mod);

// src/fixedpoint/fixedrecp.c
 fixed32_t  fixedRecp32( fixed32_t x);
ufixed32_t ufixedRecp32(ufixed32_t x);
 fixed64_t  fixedRecp64( fixed64_t x);
ufixed64_t ufixedRecp64(ufixed64_t x);

// src/fixedpoint/fixedshift.c
 fixed32_t  fixedLShift32( fixed32_t x, uint8_t shift);
ufixed32_t ufixedLShift32(ufixed32_t x, uint8_t shift);
 fixed64_t  fixedLShift64( fixed64_t x, uint8_t shift);
ufixed64_t ufixedLShift64(ufixed64_t x, uint8_t shift);
 fixed32_t  fixedRShift32( fixed32_t x, uint8_t shift);
ufixed32_t ufixedRShift32(ufixed32_t x, uint8_t shift);
 fixed64_t  fixedRShift64( fixed64_t x, uint8_t shift);
ufixed64_t ufixedRShift64(ufixed64_t x, uint8_t shift);

// src/fixedpoint/fixedgamma.c
 fixed32_t  fixedGamma32( fixed32_t x);
ufixed32_t ufixedGamma32(ufixed32_t x);
 fixed64_t  fixedGamma64( fixed64_t x);
ufixed64_t ufixedGamma64(ufixed64_t x);
 fixed32_t  fixedGammaRecp32( fixed32_t x);
ufixed32_t ufixedGammaRecp32(ufixed32_t x);
 fixed64_t  fixedGammaRecp64( fixed64_t x);
ufixed64_t ufixedGammaRecp64(ufixed64_t x);

// src/fixedpoint/fixederf.c
 fixed32_t  fixedErf32(   fixed32_t x);
ufixed32_t ufixedErf32(  ufixed32_t x);
 fixed64_t  fixedErf64(   fixed64_t x);
ufixed64_t ufixedErf64(  ufixed64_t x);
 fixed32_t  fixedErfc32(  fixed32_t x);
ufixed32_t ufixedErfc32( ufixed32_t x);
 fixed64_t  fixedErfc64(  fixed64_t x);
ufixed64_t ufixedErfc64( ufixed64_t x);
 fixed32_t  fixedQFunc32( fixed32_t x);
ufixed32_t ufixedQFunc32(ufixed32_t x);
 fixed64_t  fixedQFunc64( fixed64_t x);
ufixed64_t ufixedQFunc64(ufixed64_t x);

// src/fixedpoint/fixedabs.c
 fixed32_t  fixedAbs32( fixed32_t x);
ufixed32_t ufixedAbs32(ufixed32_t x);
 fixed64_t  fixedAbs64( fixed64_t x);
ufixed64_t ufixedAbs64(ufixed64_t x);
 fixed32_t  fixedSgn32( fixed32_t x);
ufixed32_t ufixedSgn32(ufixed32_t x);
 fixed64_t  fixedSgn64( fixed64_t x);
ufixed64_t ufixedSgn64(ufixed64_t x);

// src/fixedpoint/fixedround.c
 fixed32_t  fixedFloor32( fixed32_t x);
ufixed32_t ufixedFloor32(ufixed32_t x);
 fixed64_t  fixedFloor64( fixed64_t x);
ufixed64_t ufixedFloor64(ufixed64_t x);
 fixed32_t  fixedCeil32( fixed32_t x);
ufixed32_t ufixedCeil32(ufixed32_t x);
 fixed64_t  fixedCeil64( fixed64_t x);
ufixed64_t ufixedCeil64(ufixed64_t x);
 fixed32_t  fixedRound32( fixed32_t x, int8_t exp);
ufixed32_t ufixedRound32(ufixed32_t x, int8_t exp);
 fixed64_t  fixedRound64( fixed64_t x, int8_t exp);
ufixed64_t ufixedRound64(ufixed64_t x, int8_t exp);
 fixed32_t  fixedFract32( fixed32_t x);
ufixed32_t ufixedFract32(ufixed32_t x);
 fixed64_t  fixedFract64( fixed64_t x);
ufixed64_t ufixedFract64(ufixed64_t x);

// src/fixedpoint/fixedclamp.c
 fixed32_t  fixedClamp32( fixed32_t x,  fixed32_t min,  fixed32_t max);
ufixed32_t ufixedClamp32(ufixed32_t x, ufixed32_t min, ufixed32_t max);
 fixed64_t  fixedClamp64( fixed64_t x,  fixed64_t min,  fixed64_t max);
ufixed64_t ufixedClamp64(ufixed64_t x, ufixed64_t min, ufixed64_t max);
 fixed32_t  fixedReLU32( fixed32_t x);
ufixed32_t ufixedReLU32(ufixed32_t x);
 fixed64_t  fixedReLU64( fixed64_t x);
ufixed64_t ufixedReLU64(ufixed64_t x);

// src/fixedpoint/fixedmagnitude.c
 fixed32_t  fixedMag32( fixed32_t x,  fixed32_t y);
ufixed32_t ufixedMag32(ufixed32_t x, ufixed32_t y);
 fixed64_t  fixedMag64( fixed64_t x,  fixed64_t y);
ufixed64_t ufixedMag64(ufixed64_t x, ufixed64_t y);
 fixed32_t  fixedMagh32( fixed32_t x,  fixed32_t y);
ufixed32_t ufixedMagh32(ufixed32_t x, ufixed32_t y);
 fixed64_t  fixedMagh64( fixed64_t x,  fixed64_t y);
ufixed64_t ufixedMagh64(ufixed64_t x, ufixed64_t y);

// src/fixedpoint/fixedsinc.c
 fixed32_t  fixedSinc32( fixed32_t x);
ufixed32_t ufixedSinc32(ufixed32_t x);
 fixed64_t  fixedSinc64( fixed64_t x);
ufixed64_t ufixedSinc64(ufixed64_t x);

// src/fixedpoint/fixedutils.c
uint8_t  fixedIsNeg32( fixed32_t x);
uint8_t ufixedIsNeg32(ufixed32_t x);
uint8_t  fixedIsNeg64( fixed64_t x);
uint8_t ufixedIsNeg64(ufixed64_t x);
uint8_t  fixedIsZero32( fixed32_t x);
uint8_t ufixedIsZero32(ufixed32_t x);
uint8_t  fixedIsZero64( fixed64_t x);
uint8_t ufixedIsZero64(ufixed64_t x);
uint8_t  fixedIsPos32( fixed32_t x);
uint8_t ufixedIsPos32(ufixed32_t x);
uint8_t  fixedIsPos64( fixed64_t x);
uint8_t ufixedIsPos64(ufixed64_t x);
uint8_t  fixedIsInt32( fixed32_t x);
uint8_t ufixedIsInt32(ufixed32_t x);
uint8_t  fixedIsInt64( fixed64_t x);
uint8_t ufixedIsInt64(ufixed64_t x);

FPErrorCode  fixedFrexp32(  fixed32_t x, int8_t *exponent,  fixed32_t *mantissa);
FPErrorCode ufixedFrexp32( ufixed32_t x, int8_t *exponent, ufixed32_t *mantissa);
FPErrorCode  fixedFrexp64(  fixed64_t x, int8_t *exponent,  fixed64_t *mantissa);
FPErrorCode ufixedFrexp64( ufixed64_t x, int8_t *exponent, ufixed64_t *mantissa);
FPErrorCode  fixedFrexp232( fixed32_t x, int8_t *exponent,  fixed32_t *mantissa);
FPErrorCode ufixedFrexp232(ufixed32_t x, int8_t *exponent, ufixed32_t *mantissa);
FPErrorCode  fixedFrexp264( fixed64_t x, int8_t *exponent,  fixed64_t *mantissa);
FPErrorCode ufixedFrexp264(ufixed64_t x, int8_t *exponent, ufixed64_t *mantissa);

FPErrorCode  fixedModf32(   fixed32_t x,  fixed32_t *integer,   fixed32_t *decimal);
FPErrorCode ufixedModf32(  ufixed32_t x, ufixed32_t *integer,  ufixed32_t *decimal);
FPErrorCode  fixedModf64(   fixed64_t x,  fixed64_t *integer,   fixed64_t *decimal);
FPErrorCode ufixedModf64(  ufixed64_t x, ufixed64_t *integer,  ufixed64_t *decimal);

// src/fixedpoint/fixederrno.c
FPErrorCode fixedGetErrno();
void fixedClearErrno();
void fixedSetErrno(FPErrorCode code);

#endif // FIXEDPOINT_H
