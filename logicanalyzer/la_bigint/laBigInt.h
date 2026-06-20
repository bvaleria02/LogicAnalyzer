#ifndef LA_BIG_INT
#define LA_BIG_INT

#include "../liblogicanalyzer.h"
#include <stdlib.h>
#include <stdbool.h>


typedef enum {
	LA_BIG_INT_READ				= 0x1,
	LA_BIG_INT_WRITE			= 0x2,
	LA_BIG_INT_IS_SET			= 0x4,
	LA_BIG_INT_IS_STACK			= 0x8
} LABigIntFlags;

typedef uint32_t LABigIntLimb_t;
#define LA_BIG_INT_BIT_SIZE (sizeof(LABigIntLimb_t) * 8)
#define LA_BIG_INT_LIMB_MAX ((LABigIntLimb_t)-1)
#define LA_BIG_INT_LIMB_MIN 0x0

typedef enum {
	LA_BIG_INT_POSITIVE			= 0x0,
	LA_BIG_INT_NEGATIVE			= 0x1,
} LABigIntSign_t;

typedef enum {
	LA_BIG_INT_CMP_ERROR			= 0x0,
	LA_BIG_INT_CMP_LOWER			= 0x1,
	LA_BIG_INT_CMP_EQUAL			= 0x2,
	LA_BIG_INT_CMP_HIGHER			= 0x3
} LABigIntComp_t;

typedef enum {
	LA_BIG_INT_CLZ_BUILTIN			= 0x0,
	LA_BIG_INT_CLZ_LUT				= 0x1,
	LA_BIG_INT_CLZ_REALTIME			= 0x2
} LABigIntCLZMode;

typedef struct {
	size_t    		 limbCount;
	size_t			 capacity;
	LABigIntSign_t	 sign;
	LABigIntFlags	 flags;
	LABigIntLimb_t 	*limbs;
} LABigInt_t;

// create.c
LAErrorCode LABigIntCreateFlags(LABigInt_t *m, size_t limbCount, LABigIntFlags flags);
LAErrorCode LABigIntCreate(LABigInt_t *m, size_t limbCount);
LAErrorCode LABigIntDestroy(LABigInt_t *m);
LAErrorCode LABigIntSetLimbs(LABigInt_t *m, const size_t index, const LABigIntLimb_t *value, const size_t limbCount);
LAErrorCode LABigIntSetInt(LABigInt_t *m, const size_t index, const LABigIntLimb_t value);
LAErrorCode LABigIntCreateFromHexString(LABigInt_t *m, const char *hexString);
LAErrorCode LABigIntCreateFromIntStack(LABigInt_t *a, int64_t n, LABigIntLimb_t *limbs);
LAErrorCode LABigIntCreateFromUIntStack(LABigInt_t *a, uint64_t n, LABigIntLimb_t *limbs);
LAErrorCode LABigIntCreateFromInt(LABigInt_t *a, int64_t n);
LAErrorCode LABigIntCreateFromUInt(LABigInt_t *a, uint64_t n);
LAErrorCode LABigIntCreateFromDecString(LABigInt_t *a, const char *decString);
LAErrorCode LABigIntCreateFromOther(LABigInt_t *dest, LABigInt_t *src, const size_t limbCount);

// utils.c
uint64_t LAMin3_uint64(uint64_t n1, uint64_t n2, uint64_t n3);
LAErrorCode LABigIntSymmetricalCopy(LABigInt_t *dest, LABigInt_t *src, const size_t startIndex);
LAErrorCode LABigIntCarryPropagation(LABigInt_t *a, const bool initialCarry, const size_t index, bool *outputCarry);
LAErrorCode LABigIntLeftCopy2(LABigInt_t *dest, LABigInt_t *s1, LABigInt_t *s2);
LAErrorCode LABigIntBorrowPropagation(LABigInt_t *a, const bool initialBorrow, const size_t index, bool *outputBorrow);
LAErrorCode LABigIntLimbSet(LABigInt_t *m, const size_t index, const size_t length, LABigIntLimb_t value);
LAErrorCode LABigIntCopyLimbs(LABigInt_t *a, LABigInt_t *b, const size_t start, const size_t length);
LAErrorCode LABigIntClearBits(LABigInt_t *a, const size_t start, const size_t end);

// add.c
LAErrorCode LABigIntAdd_2_1_backend(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c, const bool saturation);
LAErrorCode LABigIntAdd_1_1_backend(LABigInt_t *a, LABigInt_t *b, const bool saturation, const bool reverse);
LAErrorCode LABigIntAdd(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c);
LAErrorCode LABigIntAddInplace(LABigInt_t *a, LABigInt_t *b);
LAErrorCode LABigIntAddClamp(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c);
LAErrorCode LABigIntAddClampInplace(LABigInt_t *a, LABigInt_t *b);
LAErrorCode LABigIntAddSat(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c);
LAErrorCode LABigIntAddSatInplace(LABigInt_t *a, LABigInt_t *b);
LAErrorCode LABigIntAddInt(LABigInt_t *a, int64_t n, LABigInt_t *c);
LAErrorCode LABigIntAddIntSat(LABigInt_t *a, int64_t n, LABigInt_t *c);
LAErrorCode LABigIntAddIntInplace(LABigInt_t *a, int64_t n);
LAErrorCode LABigIntAddIntSatInplace(LABigInt_t *a, int64_t n);
LAErrorCode LABigIntAddUInt(LABigInt_t *a, uint64_t n, LABigInt_t *c);
LAErrorCode LABigIntAddUIntSat(LABigInt_t *a, uint64_t n, LABigInt_t *c);
LAErrorCode LABigIntAddUIntInplace(LABigInt_t *a, uint64_t n);
LAErrorCode LABigIntAddUIntSatInplace(LABigInt_t *a, uint64_t n);

// sub.c
LAErrorCode LABigIntSub_2_1_backend(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c, const bool saturation);
LAErrorCode LABigIntSub_1_1_backend(LABigInt_t *a, LABigInt_t *b, const bool saturation, const bool reverse);
LAErrorCode LABigIntSub(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c);
LAErrorCode LABigIntSubInplace(LABigInt_t *a, LABigInt_t *b);
LAErrorCode LABigIntSubSat(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c);
LAErrorCode LABigIntSubSatInplace(LABigInt_t *a, LABigInt_t *b);
LAErrorCode LABigIntSubClamp(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c);
LAErrorCode LABigIntSubClampInplace(LABigInt_t *a, LABigInt_t *b);
LAErrorCode LABigIntValueComparator(LABigInt_t *a, LABigInt_t *b, LABigIntComp_t *cmp);
LAErrorCode LABigIntSubInt(LABigInt_t *a, int64_t n, LABigInt_t *c);
LAErrorCode LABigIntSubIntSat(LABigInt_t *a, int64_t n, LABigInt_t *c);
LAErrorCode LABigIntSubIntInplace(LABigInt_t *a, int64_t n);
LAErrorCode LABigIntSubIntSatInplace(LABigInt_t *a, int64_t n);
LAErrorCode LABigIntSubUInt(LABigInt_t *a, uint64_t n, LABigInt_t *c);
LAErrorCode LABigIntSubUIntSat(LABigInt_t *a, uint64_t n, LABigInt_t *c);
LAErrorCode LABigIntSubUIntInplace(LABigInt_t *a, uint64_t n);
LAErrorCode LABigIntSubUIntSatInplace(LABigInt_t *a, uint64_t n);

// mul.c
LAErrorCode LABigIntKaratsuba_core(LABigIntLimb_t x1, LABigIntLimb_t x2, LABigIntLimb_t y1, LABigIntLimb_t y2, uint64_t *z0, uint64_t *z1, uint64_t *z2);
LAErrorCode LABigIntMul_schoolbook_backend(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c, const bool sat);
LAErrorCode LABigIntMul_integer_backend(LABigInt_t *a, uint32_t b, const bool sat);
LAErrorCode LABigIntMul(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c);
LAErrorCode LABigIntMulSat(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c);
LAErrorCode LABigIntMulUIntInplace(LABigInt_t *a, uint32_t b);
LAErrorCode LABigIntMulUIntSatInplace(LABigInt_t *a, uint32_t b);
LAErrorCode LABigIntMulIntInplace(LABigInt_t *a, int32_t b);
LAErrorCode LABigIntMulIntSatInplace(LABigInt_t *a, int32_t b);

// div.c
LAErrorCode LABigIntDiv_integer_backend(LABigInt_t *a, uint32_t b, uint32_t *rem);
LAErrorCode LABigIntDiv_restoring_backend(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c);
LAErrorCode LABigIntDiv_non_restoring_backend(LABigInt_t *q, LABigInt_t *m, LABigInt_t *a);
LAErrorCode LABigIntDiv_newton_backend(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c);
LAErrorCode LABigIntDiv(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c);
LAErrorCode LABigIntDivInplace(LABigInt_t *a, LABigInt_t *b);

// logical.c
LAErrorCode LABigIntReverseLimbsInplace(LABigInt_t *a, const size_t start, const size_t end);
LAErrorCode LABigIntReverseLimbs(LABigInt_t *a, LABigInt_t *b, const size_t start, const size_t end);
LAErrorCode LABigIntRotateLimbsInplace(LABigInt_t *a, const size_t shift, const bool rotateRight);
LAErrorCode LABigIntRotateLimbs(LABigInt_t *a, LABigInt_t *b, const size_t shift, const bool rotateRight);
LAErrorCode LABigIntShiftLimbsInplace(LABigInt_t *a, const size_t shift, const bool shiftRight);
LAErrorCode LABigIntShiftLimbs(LABigInt_t *a, LABigInt_t *b, const size_t shift, const bool shiftRight);
LAErrorCode LABigIntBitShift(LABigInt_t *a, LABigInt_t *b, const size_t shift, const bool shiftRight, const bool rotate);
LAErrorCode LABigIntLSL(LABigInt_t *a, size_t n, LABigInt_t *b);
LAErrorCode LABigIntLSLInplace(LABigInt_t *a, size_t n);
LAErrorCode LABigIntLSR(LABigInt_t *a, size_t n, LABigInt_t *b);
LAErrorCode LABigIntLSRInplace(LABigInt_t *a, size_t n);
LAErrorCode LABigIntRSL(LABigInt_t *a, size_t n, LABigInt_t *b);
LAErrorCode LABigIntRSLInplace(LABigInt_t *a, size_t n);
LAErrorCode LABigIntRSR(LABigInt_t *a, size_t n, LABigInt_t *b);
LAErrorCode LABigIntRSRInplace(LABigInt_t *a, size_t n);
LAErrorCode LABigIntLSRR(LABigInt_t *a, size_t n, LABigInt_t *b);
LAErrorCode LABigIntLSRRInplace(LABigInt_t *a, size_t n);

// boolean.c
bool LABigIntIsValid(LABigInt_t *a);
bool LABigIntIsZero(LABigInt_t *a);
bool LABigIntIsNeg(LABigInt_t *a);
bool LABigIntIsPos(LABigInt_t *a);
bool LABigIntIsHigher(LABigInt_t *a, LABigInt_t *b);
bool LABigIntIsHigherOrEqual(LABigInt_t *a, LABigInt_t *b);
bool LABigIntIsLower(LABigInt_t *a, LABigInt_t *b);
bool LABigIntIsLowerOrEqual(LABigInt_t *a, LABigInt_t *b);
bool LABigIntIsNotEqual(LABigInt_t *a, LABigInt_t *b);
bool LABigIntIsEqual(LABigInt_t *a, LABigInt_t *b);

// print.c
LAErrorCode LABigIntPrintHex(LABigInt_t *m, const char *label);
LAErrorCode LABigIntPrintBin(LABigInt_t *m, const char *label);
LAErrorCode LABigIntPrintDec(LABigInt_t *m, const char *label);
	
// clz.c
LAErrorCode LABigIntCLZ(LABigInt_t *a, size_t *b, LABigIntCLZMode mode);
LAErrorCode LABigIntCTZ(LABigInt_t *a, size_t *b);

#endif // LA_BIG_INT
