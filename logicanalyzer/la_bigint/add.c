#include "../liblogicanalyzer.h"
#include "laBigInt.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

LAErrorCode LABigIntAdd_2_1_backend(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c, const bool saturation){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(c)))	return LA_ERROR_BIG_INT;

	LAErrorCode     code		= LA_NO_ERROR;
	LABigIntLimb_t 	acc 		= 0;
	size_t 			carry 		= 0x0;
	size_t 			carry_old 	= 0x0;
	size_t			length      = LAMin3_uint64(a->limbCount, b->limbCount, c->limbCount);

	// Regular sum
	for(size_t i = 0; i < length; i++){
		acc    = a->limbs[i] + b->limbs[i];
		carry  = (acc < a->limbs[i]) ? 0x1 : 0x0;
		
		c->limbs[i]  = acc + carry_old;
		carry 		+= (c->limbs[i] < acc) ? 0x1 : 0x0;

		carry_old = carry;
	}
	
	code = LABigIntLeftCopy2(c, a, b);
	if(code) return code;

	bool lastCarry = false;
	code = LABigIntCarryPropagation(c, carry_old, length, &lastCarry);
	if(code) return code;

	if(lastCarry && saturation){
		code = LABigIntLimbSet(c, 0, c->limbCount, LA_BIG_INT_LIMB_MAX);
		if(code) return code;
	}

	return LA_NO_ERROR;
}

LAErrorCode LABigIntAdd(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	// Both have the same sign
	if(a->sign == b->sign){
		// A + B = A + B
		// (-A) + (-B) = -(A + B)
		code = LABigIntAdd_2_1_backend(a, b, c, false);
		c->sign = a->sign;
		goto finally;
	}

	// a.sign != b.sign
	LABigIntComp_t cmp = LA_BIG_INT_CMP_ERROR;
	code = LABigIntValueComparator(a, b, &cmp);
	if(code) goto finally;
	if(cmp == LA_BIG_INT_CMP_ERROR) goto finally;

	if(cmp == LA_BIG_INT_CMP_HIGHER){
		//   A  + (-B) =   |A| - |B|
		// (-A) +   B  = -(|A| - |B|)
		code = LABigIntSub_2_1_backend(a, b, c, false);
		c->sign = a->sign;
		goto finally;
	} else if (cmp == LA_BIG_INT_CMP_EQUAL){
		code = LABigIntLimbSet(c, 0, c->limbCount, 0x0);
		c->sign = LA_BIG_INT_POSITIVE;
		goto finally;
	} else {
		//   A  + (-B) = -(|B| - |A|)
		// (-A) +   B  =   |B| - |A|
		code = LABigIntSub_2_1_backend(b, a, c, false);
		c->sign = b->sign;
		goto finally;
	}

finally:
	return code;
}

LAErrorCode LABigIntAddSat(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	// Both have the same sign
	if(a->sign == b->sign){
		// A + B = A + B
		// (-A) + (-B) = -(A + B)
		code = LABigIntAdd_2_1_backend(a, b, c, true);
		c->sign = a->sign;
		goto finally;
	}

	// a.sign != b.sign
	LABigIntComp_t cmp = LA_BIG_INT_CMP_ERROR;
	code = LABigIntValueComparator(a, b, &cmp);
	if(code) goto finally;
	if(cmp == LA_BIG_INT_CMP_ERROR) goto finally;

	if(cmp == LA_BIG_INT_CMP_HIGHER){
		//   A  + (-B) =   |A| - |B|
		// (-A) +   B  = -(|A| - |B|)
		code = LABigIntSub_2_1_backend(a, b, c, true);
		c->sign = a->sign;
		goto finally;
	} else if (cmp == LA_BIG_INT_CMP_EQUAL){
		code = LABigIntLimbSet(c, 0, c->limbCount, 0x0);
		c->sign = LA_BIG_INT_POSITIVE;
		goto finally;
	} else {
		//   A  + (-B) = -(|B| - |A|)
		// (-A) +   B  =   |B| - |A|
		code = LABigIntSub_2_1_backend(b, a, c, true);
		c->sign = b->sign;
		goto finally;
	}

finally:
	return code;
}

LAErrorCode LABigIntAdd_1_1_backend(LABigInt_t *a, LABigInt_t *b, const bool saturation, const bool reverse){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))	return LA_ERROR_BIG_INT;

	LAErrorCode     code		= LA_NO_ERROR;
	LABigIntLimb_t 	acc 		= 0;
	size_t 			carry 		= 0x0;
	size_t 			carry_old 	= 0x0;
	size_t			length      = (a->limbCount <= b->limbCount) ? a->limbCount : b->limbCount;

	// Regular sum
	for(size_t i = 0; i < length; i++){
		acc    = a->limbs[i] + b->limbs[i];
		carry  = (acc < a->limbs[i]) ? 0x1 : 0x0;
		
		a->limbs[i]  = acc + carry_old;
		carry 		+= (a->limbs[i] < acc) ? 0x1 : 0x0;

		carry_old = carry;
	}

	bool lastCarry = false;
	code = LABigIntCarryPropagation(a, carry_old, length, &lastCarry);
	if(code) return code;

	if(lastCarry && saturation){
		code = LABigIntLimbSet(a, 0, a->limbCount, LA_BIG_INT_LIMB_MAX);
		if(code) return code;
	}

	(void) reverse;
	return LA_NO_ERROR;
}

LAErrorCode LABigIntAddInplace(LABigInt_t *a, LABigInt_t *b){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	// Both have the same sign
	if(a->sign == b->sign){
		// A + B = A + B
		// (-A) + (-B) = -(A + B)
		code = LABigIntAdd_1_1_backend(a, b, false, false);
		goto finally;
	}

	// a.sign != b.sign
	LABigIntComp_t cmp = LA_BIG_INT_CMP_ERROR;
	code = LABigIntValueComparator(a, b, &cmp);
	if(code) goto finally;
	if(cmp == LA_BIG_INT_CMP_ERROR) goto finally;

	if(cmp == LA_BIG_INT_CMP_HIGHER){
		//   A  + (-B) =   |A| - |B|
		// (-A) +   B  = -(|A| - |B|)
		code = LABigIntSub_1_1_backend(a, b, false, false);
		goto finally;
	} else if (cmp == LA_BIG_INT_CMP_EQUAL){
		code = LABigIntLimbSet(a, 0, a->limbCount, 0x0);
		a->sign = LA_BIG_INT_POSITIVE;
		goto finally;
	} else {
		//   A  + (-B) = -(|B| - |A|)
		// (-A) +   B  =   |B| - |A|
		code = LABigIntSub_1_1_backend(a, b, false, true);
		a->sign = b->sign;
		goto finally;
	}

finally:
	return code;
}

LAErrorCode LABigIntAddSatInplace(LABigInt_t *a, LABigInt_t *b){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	// Both have the same sign
	if(a->sign == b->sign){
		// A + B = A + B
		// (-A) + (-B) = -(A + B)
		code = LABigIntAdd_1_1_backend(a, b, true, false);
		goto finally;
	}

	// a.sign != b.sign
	LABigIntComp_t cmp = LA_BIG_INT_CMP_ERROR;
	code = LABigIntValueComparator(a, b, &cmp);
	if(code) goto finally;
	if(cmp == LA_BIG_INT_CMP_ERROR) goto finally;

	if(cmp == LA_BIG_INT_CMP_HIGHER){
		//   A  + (-B) =   |A| - |B|
		// (-A) +   B  = -(|A| - |B|)
		code = LABigIntSub_1_1_backend(a, b, true, false);
		goto finally;
	} else if (cmp == LA_BIG_INT_CMP_EQUAL){
		code = LABigIntLimbSet(a, 0, a->limbCount, 0x0);
		a->sign = LA_BIG_INT_POSITIVE;
		goto finally;
	} else {
		//   A  + (-B) = -(|B| - |A|)
		// (-A) +   B  =   |B| - |A|
		code = LABigIntSub_1_1_backend(a, b, true, true);
		a->sign = b->sign;
		goto finally;
	}

finally:
	return code;
}

// Deprecated, will be removed
LAErrorCode LABigIntAddClamp(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c){
	return LABigIntAddSat(a, b, c);
}

// Deprecated, will be removed
LAErrorCode LABigIntAddClampInplace(LABigInt_t *a, LABigInt_t *b){
	return LABigIntAddSatInplace(a, b);
}

LAErrorCode LABigIntAddInt(LABigInt_t *a, int64_t n, LABigInt_t *c){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigInt_t b;
	LABigIntLimb_t limbs[2];
	code = LABigIntCreateFromIntStack(&b, n, (LABigIntLimb_t *)limbs);
	if(code) return code;

	code = LABigIntAdd(a, &b, c);
	return code;
}

LAErrorCode LABigIntAddIntSat(LABigInt_t *a, int64_t n, LABigInt_t *c){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigInt_t b;
	LABigIntLimb_t limbs[2];
	code = LABigIntCreateFromIntStack(&b, n, (LABigIntLimb_t *)limbs);
	if(code) return code;

	code = LABigIntAddSat(a, &b, c);
	return code;
}

LAErrorCode LABigIntAddIntInplace(LABigInt_t *a, int64_t n){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigInt_t b;
	LABigIntLimb_t limbs[2];
	code = LABigIntCreateFromIntStack(&b, n, (LABigIntLimb_t *)limbs);
	if(code) return code;

	code = LABigIntAddInplace(a, &b);
	return code;
}

LAErrorCode LABigIntAddIntSatInplace(LABigInt_t *a, int64_t n){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigInt_t b;
	LABigIntLimb_t limbs[2];
	code = LABigIntCreateFromIntStack(&b, n, (LABigIntLimb_t *)limbs);
	if(code) return code;

	code = LABigIntAddSatInplace(a, &b);
	return code;
}

LAErrorCode LABigIntAddUInt(LABigInt_t *a, uint64_t n, LABigInt_t *c){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigInt_t b;
	LABigIntLimb_t limbs[2];
	code = LABigIntCreateFromIntStack(&b, n, (LABigIntLimb_t *)limbs);
	if(code) return code;

	code = LABigIntAdd(a, &b, c);
	return code;
}

LAErrorCode LABigIntAddUIntSat(LABigInt_t *a, uint64_t n, LABigInt_t *c){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigInt_t b;
	LABigIntLimb_t limbs[2];
	code = LABigIntCreateFromUIntStack(&b, n, (LABigIntLimb_t *)limbs);
	if(code) return code;

	code = LABigIntAddSat(a, &b, c);
	return code;
}

LAErrorCode LABigIntAddUIntInplace(LABigInt_t *a, uint64_t n){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigInt_t b;
	LABigIntLimb_t limbs[2];
	code = LABigIntCreateFromUIntStack(&b, n, (LABigIntLimb_t *)limbs);
	if(code) return code;

	code = LABigIntAddInplace(a, &b);
	return code;
}

LAErrorCode LABigIntAddUIntSatInplace(LABigInt_t *a, uint64_t n){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigInt_t b;
	LABigIntLimb_t limbs[2];
	code = LABigIntCreateFromUIntStack(&b, n, (LABigIntLimb_t *)limbs);
	if(code) return code;

	code = LABigIntAddSatInplace(a, &b);
	return code;
}
