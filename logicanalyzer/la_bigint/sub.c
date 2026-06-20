#include "../liblogicanalyzer.h"
#include "laBigInt.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

LAErrorCode LABigIntSub_2_1_backend(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c, const bool saturation){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(c)))	return LA_ERROR_BIG_INT;

	LAErrorCode     code		= LA_NO_ERROR;
	LABigIntLimb_t 	acc 		= 0;
	size_t 			borrow 		= 0x0;
	size_t 			borrow_old 	= 0x0;
	size_t			length      = LAMin3_uint64(a->limbCount, b->limbCount, c->limbCount);

	// Regular sub
	for(size_t i = 0; i < length; i++){
		acc    = a->limbs[i] - b->limbs[i];
		borrow = (acc > a->limbs[i]) ? 0x1 : 0x0;
		
		c->limbs[i]  = acc - borrow_old;
		borrow 		+= (c->limbs[i] > acc) ? 0x1 : 0x0;

		borrow_old = borrow;
	}

	code = LABigIntLeftCopy2(c, a, b);
	if(code) return code;

	bool lastBorrow = false;
	code = LABigIntBorrowPropagation(c, borrow_old, length, &lastBorrow);
	if(code) return code;

	if(lastBorrow && saturation){
		code = LABigIntLimbSet(c, 0, c->limbCount, LA_BIG_INT_LIMB_MIN);
		if(code) return code;
	}

	return LA_NO_ERROR;
}

LAErrorCode LABigIntSub(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigIntComp_t cmp = LA_BIG_INT_CMP_ERROR;
	code = LABigIntValueComparator(a, b, &cmp);
	if(code) goto finally;
	if(cmp == LA_BIG_INT_CMP_ERROR) goto finally;

	// Both have the same sign
	if((a->sign == b->sign) && (cmp == LA_BIG_INT_CMP_HIGHER)){
		//   A  -   B  =   |A| - |B|
		// (-A) - (-B) = -(|A| - |B|)
		code = LABigIntSub_2_1_backend(a, b, c, false);
		c->sign = a->sign;
		goto finally;
	} else if ((a->sign == b->sign) && (cmp != LA_BIG_INT_CMP_HIGHER)){
		//   A  -   B  = -(|B| - |A|)
		// (-A) - (-B) =   |B| - |A|
		code = LABigIntSub_2_1_backend(b, a, c, false);
		c->sign = (a->sign == LA_BIG_INT_POSITIVE) ? LA_BIG_INT_NEGATIVE : LA_BIG_INT_POSITIVE;
		goto finally;
	}

	// a.sign != b.sign

	//   A  - (-B) =   |A| + |B|
	// (-A) -   B  = -(|A| + |B|)
	code = LABigIntAdd_2_1_backend(a, b, c, false);
	c->sign = a->sign;
	goto finally;

finally:
	return code;
}

LAErrorCode LABigIntSubSat(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigIntComp_t cmp = LA_BIG_INT_CMP_ERROR;
	code = LABigIntValueComparator(a, b, &cmp);
	if(code) goto finally;
	if(cmp == LA_BIG_INT_CMP_ERROR) goto finally;

	// Both have the same sign
	if((a->sign == b->sign) && (cmp == LA_BIG_INT_CMP_HIGHER)){
		//   A  -   B  =   |A| - |B|
		// (-A) - (-B) = -(|A| - |B|)
		code = LABigIntSub_2_1_backend(a, b, c, true);
		c->sign = a->sign;
		goto finally;
	} else if ((a->sign == b->sign) && (cmp != LA_BIG_INT_CMP_HIGHER)){
		//   A  -   B  = -(|B| - |A|)
		// (-A) - (-B) =   |B| - |A|
		code = LABigIntSub_2_1_backend(b, a, c, true);
		c->sign = (a->sign == LA_BIG_INT_POSITIVE) ? LA_BIG_INT_NEGATIVE : LA_BIG_INT_POSITIVE;
		goto finally;
	}

	// a.sign != b.sign

	//   A  - (-B) =   |A| + |B|
	// (-A) -   B  = -(|A| + |B|)
	code = LABigIntAdd_2_1_backend(a, b, c, true);
	c->sign = a->sign;
	goto finally;

finally:
	return code;
}

LAErrorCode LABigIntSub_1_1_backend(LABigInt_t *a, LABigInt_t *b, const bool saturation, const bool reverse){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))	return LA_ERROR_BIG_INT;

	LAErrorCode     code		= LA_NO_ERROR;
	LABigIntLimb_t 	acc 		= 0;
	size_t 			borrow 		= 0x0;
	size_t 			borrow_old 	= 0x0;
	size_t			length      = (a->limbCount <= b->limbCount) ? a->limbCount : b->limbCount;

	// Regular sub
	if(reverse){
		for(size_t i = 0; i < length; i++){
			acc    		 = b->limbs[i] - a->limbs[i];
			borrow  	 = (acc > b->limbs[i]) ? 0x1 : 0x0;
		
			a->limbs[i]  = acc - borrow_old;
			borrow 		+= (a->limbs[i] > acc) ? 0x1 : 0x0;

			borrow_old = borrow;
		}
	} else {
		for(size_t i = 0; i < length; i++){
			acc    		 = a->limbs[i] - b->limbs[i];
			borrow  	 = (acc > a->limbs[i]) ? 0x1 : 0x0;

			a->limbs[i]  = acc - borrow_old;
			borrow 		+= (a->limbs[i] > acc) ? 0x1 : 0x0;

			borrow_old = borrow;
		}
	}

	bool lastBorrow = false;
	code = LABigIntBorrowPropagation(a, borrow_old, length, &lastBorrow);
	if(code) return code;

	if(lastBorrow && saturation){
		code = LABigIntLimbSet(a, 0, a->limbCount, LA_BIG_INT_LIMB_MIN);
		if(code) return code;
	}

	return LA_NO_ERROR;
}

LAErrorCode LABigIntSubInplace(LABigInt_t *a, LABigInt_t *b){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigIntComp_t cmp = LA_BIG_INT_CMP_ERROR;
	code = LABigIntValueComparator(a, b, &cmp);
	if(code) goto finally;
	if(cmp == LA_BIG_INT_CMP_ERROR) goto finally;

	// Both have the same sign
	if((a->sign == b->sign) && (cmp == LA_BIG_INT_CMP_HIGHER)){
		//   A  -   B  =   |A| - |B|
		// (-A) - (-B) = -(|A| - |B|)
		code = LABigIntSub_1_1_backend(a, b, false, false);
		goto finally;
	} else if ((a->sign == b->sign) && (cmp != LA_BIG_INT_CMP_HIGHER)){
//		printf("B is higher\n");
		//   A  -   B  = -(|B| - |A|)
		// (-A) - (-B) =   |B| - |A|
		code = LABigIntSub_1_1_backend(a, b, false, true);
		a->sign = (a->sign == LA_BIG_INT_POSITIVE) ? LA_BIG_INT_NEGATIVE : LA_BIG_INT_POSITIVE;
		goto finally;
	}

	// a.sign != b.sign

	//   A  - (-B) =   |A| + |B|
	// (-A) -   B  = -(|A| + |B|)
	code = LABigIntAdd_1_1_backend(a, b, false, false);
	goto finally;

finally:
	return code;
}

LAErrorCode LABigIntSubSatInplace(LABigInt_t *a, LABigInt_t *b){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigIntComp_t cmp = LA_BIG_INT_CMP_ERROR;
	code = LABigIntValueComparator(a, b, &cmp);
	if(code) goto finally;
	if(cmp == LA_BIG_INT_CMP_ERROR) goto finally;

	// Both have the same sign
	if((a->sign == b->sign) && (cmp == LA_BIG_INT_CMP_HIGHER)){
		//   A  -   B  =   |A| - |B|
		// (-A) - (-B) = -(|A| - |B|)
		code = LABigIntSub_1_1_backend(a, b, true, false);
		goto finally;
	} else if ((a->sign == b->sign) && (cmp != LA_BIG_INT_CMP_HIGHER)){
		//   A  -   B  = -(|B| - |A|)
		// (-A) - (-B) =   |B| - |A|
		code = LABigIntSub_1_1_backend(a, b, true, true);
		a->sign = (a->sign == LA_BIG_INT_POSITIVE) ? LA_BIG_INT_NEGATIVE : LA_BIG_INT_POSITIVE;
		goto finally;
	}

	// a.sign != b.sign

	//   A  - (-B) =   |A| + |B|
	// (-A) -   B  = -(|A| + |B|)
	code = LABigIntAdd_1_1_backend(a, b, true, false);
	goto finally;

finally:
	return code;
}

// Deprecated: Will be removed
LAErrorCode LABigIntSubClamp(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c){
	return LABigIntSubSat(a, b, c);
}

// Deprecated: Will be removed
LAErrorCode LABigIntSubClampInplace(LABigInt_t *a, LABigInt_t *b){
	return LABigIntSubSatInplace(a, b);
}

LAErrorCode LABigIntValueComparator(LABigInt_t *a, LABigInt_t *b, LABigIntComp_t *cmp){
	LA_HANDLE_NULLPTR(a, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(cmp, 	LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))	return LA_ERROR_BIG_INT;

	(*cmp) = LA_BIG_INT_CMP_ERROR;

	size_t			length      = (a->limbCount <= b->limbCount) ? a->limbCount : b->limbCount;

	// Iterate over excess limbs
	if(a->limbCount > b->limbCount){
		for(size_t i = length; i < a->limbCount; i++){
			if(a->limbs[i] != 0x0) goto is_higher;
		}
	} else if(b->limbCount > a->limbCount){
		for(size_t i = length; i < b->limbCount; i++){
			if(b->limbs[i] != 0x0) goto is_lower;
		}
	}

	bool 		   borrow 		= false;
	LABigIntLimb_t acc 	  		= 0x0;

	// Performs A - B
	for(size_t i = 0; i < length; i++){
		size_t k = length - 1 - i;

		acc = a->limbs[k] - b->limbs[k];
		borrow = (acc > a->limbs[k]);

		if(borrow) 		goto is_lower;
		if(acc != 0x0)	goto is_higher;
	}

	(*cmp) = LA_BIG_INT_CMP_EQUAL;
	return LA_NO_ERROR;

is_higher:
	(*cmp) = LA_BIG_INT_CMP_HIGHER;
	return LA_NO_ERROR;

is_lower:
	(*cmp) = LA_BIG_INT_CMP_LOWER;
	return LA_NO_ERROR;
}

LAErrorCode LABigIntSubInt(LABigInt_t *a, int64_t n, LABigInt_t *c){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigInt_t b;
	LABigIntLimb_t limbs[2];
	code = LABigIntCreateFromIntStack(&b, n, (LABigIntLimb_t *)limbs);
	if(code) return code;

	code = LABigIntSub(a, &b, c);
	return code;
}

LAErrorCode LABigIntSubIntSat(LABigInt_t *a, int64_t n, LABigInt_t *c){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigInt_t b;
	LABigIntLimb_t limbs[2];
	code = LABigIntCreateFromIntStack(&b, n, (LABigIntLimb_t *)limbs);
	if(code) return code;

	code = LABigIntSubSat(a, &b, c);
	return code;
}

LAErrorCode LABigIntSubIntInplace(LABigInt_t *a, int64_t n){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigInt_t b;
	LABigIntLimb_t limbs[2];
	code = LABigIntCreateFromIntStack(&b, n, (LABigIntLimb_t *)limbs);
	if(code) return code;

	code = LABigIntSubInplace(a, &b);
	return code;
}

LAErrorCode LABigIntSubIntSatInplace(LABigInt_t *a, int64_t n){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigInt_t b;
	LABigIntLimb_t limbs[2];
	code = LABigIntCreateFromIntStack(&b, n, (LABigIntLimb_t *)limbs);
	if(code) return code;

	code = LABigIntSubSatInplace(a, &b);
	return code;
}

LAErrorCode LABigIntSubUInt(LABigInt_t *a, uint64_t n, LABigInt_t *c){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigInt_t b;
	LABigIntLimb_t limbs[2];
	code = LABigIntCreateFromIntStack(&b, n, (LABigIntLimb_t *)limbs);
	if(code) return code;

	code = LABigIntSub(a, &b, c);
	return code;
}

LAErrorCode LABigIntSubUIntSat(LABigInt_t *a, uint64_t n, LABigInt_t *c){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigInt_t b;
	LABigIntLimb_t limbs[2];
	code = LABigIntCreateFromUIntStack(&b, n, (LABigIntLimb_t *)limbs);
	if(code) return code;

	code = LABigIntSubSat(a, &b, c);
	return code;
}

LAErrorCode LABigIntSubUIntInplace(LABigInt_t *a, uint64_t n){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigInt_t b;
	LABigIntLimb_t limbs[2];
	code = LABigIntCreateFromUIntStack(&b, n, (LABigIntLimb_t *)limbs);
	if(code) return code;

	code = LABigIntSubInplace(a, &b);
	return code;
}

LAErrorCode LABigIntSubUIntSatInplace(LABigInt_t *a, uint64_t n){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LABigInt_t b;
	LABigIntLimb_t limbs[2];
	code = LABigIntCreateFromUIntStack(&b, n, (LABigIntLimb_t *)limbs);
	if(code) return code;

	code = LABigIntSubSatInplace(a, &b);
	return code;
}
