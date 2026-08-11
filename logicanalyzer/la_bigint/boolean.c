#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "laBigInt.h"

// If error, always return false
// LA_HANDLE_NULLPTR(ptr, false);

// Always, early return if false
// and return true at the end

bool LABigIntIsValid(LABigInt_t *a){
	LA_HANDLE_NULLPTR(a, false);
	
	if(a == NULL) 						return false;
	if(a->limbs == NULL) 				return false;
	if(!(a->flags & LA_BIG_INT_IS_SET)) return false;

	return true;
}

bool LABigIntIsZero(LABigInt_t *a){
	LA_HANDLE_NULLPTR(a, false);

	if(!(LABigIntIsValid(a)))	return false;

	for(size_t i = 0; i < a->limbCount; i++){
		if(a->limbs[i] != 0x0) return false;
	}

	return true;
}

bool LABigIntIsNeg(LABigInt_t *a){
	LA_HANDLE_NULLPTR(a, false);

	if(!(LABigIntIsValid(a)))	return false;
	
	if(LABigIntIsZero(a)) return false;

	return (a->sign == LA_BIG_INT_NEGATIVE);
}

bool LABigIntIsPos(LABigInt_t *a){
	LA_HANDLE_NULLPTR(a, false);

	if(!(LABigIntIsValid(a)))	return false;

	if(LABigIntIsZero(a)) return false;

	return (a->sign == LA_BIG_INT_POSITIVE);
}

bool LABigIntIsHigher(LABigInt_t *a, LABigInt_t *b){
	LA_HANDLE_NULLPTR(a, false);
	LA_HANDLE_NULLPTR(b, false);

	LAErrorCode code = LA_NO_ERROR;

	if(LABigIntIsZero(a) && LABigIntIsZero(b)) return false;

	// a and b != 0, validate sign before actual computation
	if((a->sign == LA_BIG_INT_POSITIVE) && (b->sign == LA_BIG_INT_NEGATIVE)){
		return true;
	} else if((a->sign == LA_BIG_INT_NEGATIVE) && (b->sign == LA_BIG_INT_POSITIVE)){
		return false;
	}

	// A > B
	LABigIntComp_t cmp = LA_BIG_INT_CMP_ERROR; 
	code = LABigIntValueComparator(a, b, &cmp);
	if(code) 						return false;
	if(cmp == LA_BIG_INT_CMP_ERROR) return false;

	if((a->sign == LA_BIG_INT_POSITIVE) && (b->sign == LA_BIG_INT_POSITIVE)){
		return (cmp == LA_BIG_INT_CMP_HIGHER);
	} else if((a->sign == LA_BIG_INT_NEGATIVE) && (b->sign == LA_BIG_INT_NEGATIVE)){
		return (cmp == LA_BIG_INT_CMP_LOWER);
	}

	// Failsafe
	return false;
}

bool LABigIntIsHigherOrEqual(LABigInt_t *a, LABigInt_t *b){
	LA_HANDLE_NULLPTR(a, false);
	LA_HANDLE_NULLPTR(b, false);

	LAErrorCode code = LA_NO_ERROR;

	// A > B
	LABigIntComp_t cmp = LA_BIG_INT_CMP_ERROR; 
	code = LABigIntValueComparator(a, b, &cmp);
	if(code) 						return false;
	if(cmp == LA_BIG_INT_CMP_ERROR) return false;

	if((a->sign == LA_BIG_INT_POSITIVE) && (b->sign == LA_BIG_INT_POSITIVE)){
		return ((cmp == LA_BIG_INT_CMP_HIGHER) || (cmp == LA_BIG_INT_CMP_EQUAL));
	} else if((a->sign == LA_BIG_INT_NEGATIVE) && (b->sign == LA_BIG_INT_NEGATIVE)){
		return ((cmp == LA_BIG_INT_CMP_LOWER) || (cmp == LA_BIG_INT_CMP_EQUAL));
	} else if((a->sign == LA_BIG_INT_POSITIVE) && (b->sign == LA_BIG_INT_NEGATIVE)){
		return true;
	} else if((a->sign == LA_BIG_INT_NEGATIVE) && (b->sign == LA_BIG_INT_POSITIVE)){
		return (cmp == LA_BIG_INT_CMP_EQUAL);
	}

	// Failsafe
	return false;
}

bool LABigIntIsLower(LABigInt_t *a, LABigInt_t *b){
	LA_HANDLE_NULLPTR(a, false);
	LA_HANDLE_NULLPTR(b, false);

	return true;
}

bool LABigIntIsLowerOrEqual(LABigInt_t *a, LABigInt_t *b){
	LA_HANDLE_NULLPTR(a, false);
	LA_HANDLE_NULLPTR(b, false);

	return true;
}

bool LABigIntIsNotEqual(LABigInt_t *a, LABigInt_t *b){
	LA_HANDLE_NULLPTR(a, false);
	LA_HANDLE_NULLPTR(b, false);

	return true;
}

bool LABigIntIsEqual(LABigInt_t *a, LABigInt_t *b){
	LA_HANDLE_NULLPTR(a, false);
	LA_HANDLE_NULLPTR(b, false);

	return true;
}
