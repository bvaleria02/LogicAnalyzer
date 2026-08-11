#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "laBigInt.h"

LAErrorCode LABigIntReverseLimbsInplace(LABigInt_t *a, const size_t start, const size_t end){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;

	if(start >= a->limbCount) return LA_NO_ERROR;
	if(end   >= a->limbCount) return LA_NO_ERROR;
	if(start >= end) 		  return LA_NO_ERROR;

	LABigIntLimb_t aux = 0x0;
	const size_t midpoint = start + ((end - start) / 2);

	for(size_t i = start; i <= midpoint; i++){
		size_t k = end + start - i;

		aux = a->limbs[i];
		a->limbs[i] = a->limbs[k];
		a->limbs[k] = aux;
	}

	return LA_NO_ERROR;
}

LAErrorCode LABigIntReverseLimbs(LABigInt_t *a, LABigInt_t *b, const size_t start, const size_t end){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))	return LA_ERROR_BIG_INT;

	if(start >= a->limbCount) return LA_NO_ERROR;
	if(end   >= a->limbCount) return LA_NO_ERROR;
	if(start >= end) 		  return LA_NO_ERROR;

	const size_t midpoint = start + ((end - start) / 2);

	for(size_t i = start; i <= midpoint; i++){
		size_t k = end + start - i;
		if(k >= b->limbCount) break;
		if(i >= b->limbCount) break;

		b->limbs[k] = a->limbs[i];
		b->limbs[i] = a->limbs[k];
	}

	return LA_NO_ERROR;
}

LAErrorCode LABigIntRotateLimbsInplace(LABigInt_t *a, const size_t shift, const bool rotateRight){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;

	LAErrorCode code = LA_NO_ERROR;
	size_t nshift = (rotateRight) ? (a->limbCount - shift) : shift;
	if(nshift == 0) return LA_NO_ERROR;

	code = LABigIntReverseLimbsInplace(a, 0, 		a->limbCount - 1);
	if(code) return code;
	code = LABigIntReverseLimbsInplace(a, nshift, 	a->limbCount - 1);
	if(code) return code;
	code = LABigIntReverseLimbsInplace(a, 0, 		nshift - 1);
	if(code) return code;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntRotateLimbs(LABigInt_t *a, LABigInt_t *b, const size_t shift, const bool rotateRight){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))	return LA_ERROR_BIG_INT;

	LAErrorCode code = LA_NO_ERROR;

	if(shift == 0x0) {
		code = LABigIntCopyLimbs(a, b, 0, a->limbCount);
		if(code) return code;
		return LA_NO_ERROR;
	}
	if(shift >= a->limbCount) return LA_NO_ERROR;
	
	if(rotateRight){
		for(size_t i = 0; i < a->limbCount; i++){
			size_t k = (i + shift) % (a->limbCount);
			if(i >= b->limbCount) break;

			b->limbs[i] = a->limbs[k];
		}
	} else {
		for(size_t i = 0; i < a->limbCount; i++){
			size_t j = a->limbCount - 1 - i;
			size_t k = (j + shift) % a->limbCount;
			if(k >= b->limbCount) continue;

			b->limbs[k] = a->limbs[j];
		}
	}

	return LA_NO_ERROR;
}

LAErrorCode LABigIntShiftLimbsInplace(LABigInt_t *a, const size_t shift, const bool shiftRight){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))					return LA_ERROR_BIG_INT;

	LAErrorCode code = LA_NO_ERROR;

	if(shift == 0) return LA_NO_ERROR;
	if(shift >= a->limbCount){
		code = LABigIntLimbSet(a, 0, a->limbCount, 0x0);
		if(code) return code;
		return LA_NO_ERROR;
	}

	const size_t length = a->limbCount - shift;

	if(shiftRight){
		for(size_t i = 0; i < length; i++){
			size_t k = i + shift;
			a->limbs[i] = a->limbs[k];
		}

		code = LABigIntLimbSet(a, length, a->limbCount, 0x0);
		if(code) return code;

	} else {
		for(size_t i = 0; i < length; i++){
			size_t j = a->limbCount - 1 - i;
			size_t k = j - shift;
			//printf("j: %lu\tk: %lu\n", j, k);
			a->limbs[j] = a->limbs[k];
		}

		code = LABigIntLimbSet(a, 0, shift, 0x0);
		if(code) return code;
	}

	return LA_NO_ERROR;
}

LAErrorCode LABigIntShiftLimbs(LABigInt_t *a, LABigInt_t *b, const size_t shift, const bool shiftRight){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))					return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))					return LA_ERROR_BIG_INT;

	LAErrorCode code = LA_NO_ERROR;

	if(shift == 0) return LA_NO_ERROR;
	if(shift >= b->limbCount){
		code = LABigIntLimbSet(b, 0, b->limbCount, 0x0);
		if(code) return code;
		code = LABigIntCopyLimbs(a, b, 0, a->limbCount);
		if(code) return code;
		return LA_NO_ERROR;
	}

	const size_t length = a->limbCount - shift;

	if(shiftRight){
		for(size_t i = 0; i < a->limbCount; i++){
			size_t k = i + shift;
			if(i >= b->limbCount) break;
			b->limbs[i] = a->limbs[k];
		}

		code = LABigIntLimbSet(b, length, b->limbCount, 0x0);
		if(code) return code;

	} else {
		for(size_t i = 0; i < a->limbCount; i++){
			size_t j = a->limbCount - 1 - i;
			size_t k = j + shift;
			if(k >= b->limbCount) continue;
			b->limbs[k] = a->limbs[j];
		}

		code = LABigIntLimbSet(b, 0, shift, 0x0);
		if(code) return code;
		code = LABigIntLimbSet(b, a->limbCount + shift, b->limbCount, 0x0);
		if(code) return code;
	}

	return LA_NO_ERROR;
}

LAErrorCode LABigIntBitShift(LABigInt_t *a, LABigInt_t *b, const size_t shift, const bool shiftRight, const bool rotate){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))					return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)) && (b != NULL))	return LA_ERROR_BIG_INT;
	if(shift == 0) return LA_NO_ERROR;
	
	const size_t s = LA_BIG_INT_BIT_SIZE - shift;
	const LABigIntLimb_t maskR = (shiftRight) ? (LABigIntLimb_t)((0x1 << shift) - 0x1) : (0xFFFFFFFF & (((0x1 << shift) - 0x1) << s));

	LABigIntLimb_t aux 		= 0x0;
	LABigIntLimb_t aux_old 	= 0x0;
	LABigIntLimb_t aux_i0 	= 0x0;
	LABigInt_t 	   *c       = (b != NULL) ? b : a;

	if(shiftRight){
		aux_i0 = a->limbs[0] & maskR;

		for(size_t i = 0; i < a->limbCount; i++){
			size_t k = a->limbCount - 1 - i;
			if(k >= c->limbCount) break;

			aux = a->limbs[k] & maskR;

			c->limbs[k] = ((a->limbs[k]) >> shift) | (aux_old << s);

			aux_old = aux;
		}
		
		if(rotate && (a->limbCount >= c->limbCount)){
			c->limbs[a->limbCount - 1] |= (aux_i0 << s);
		}

	} else {
		aux_i0 = a->limbs[a->limbCount - 1] & maskR;

		for(size_t i = 0; i < a->limbCount; i++){
			aux    = a->limbs[i] & maskR;
			if(i >= c->limbCount) break;

			c->limbs[i] = ((a->limbs[i]) << shift) | (aux_old >> s);

			aux_old = aux;
		}

		if(rotate){
			c->limbs[0] |= (aux_i0 >> s);
		}
	}

	return LA_NO_ERROR;
}


LAErrorCode LABigIntLSL(LABigInt_t *a, size_t n, LABigInt_t *b){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))	return LA_ERROR_BIG_INT;

	// Early return if n == 0
	if(n == 0) return LA_NO_ERROR;

	LAErrorCode code = LA_NO_ERROR;

	const size_t limbShift = n / LA_BIG_INT_BIT_SIZE;
	const size_t bitShift  = n % LA_BIG_INT_BIT_SIZE;

	code = LABigIntShiftLimbs(a, b, limbShift, false);
	if(code) return code;

	code = LABigIntBitShift(b, NULL, bitShift, false, false);
	if(code) return code;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntLSLInplace(LABigInt_t *a, size_t n){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;

	// Early return if n == 0
	if(n == 0) return LA_NO_ERROR;

	LAErrorCode code = LA_NO_ERROR;

	const size_t limbShift = n / LA_BIG_INT_BIT_SIZE;
	const size_t bitShift  = n % LA_BIG_INT_BIT_SIZE;

//	printf("A: %lu\tB: %lu\n", limbShift, bitShift);
//	LABigIntPrintBin(a, "A1");

	code = LABigIntShiftLimbsInplace(a, limbShift, false);
	if(code) return code;

//	LABigIntPrintBin(a, "A2");

	code = LABigIntBitShift(a, NULL, bitShift, false, false);
	if(code) return code;

//	LABigIntPrintBin(a, "A3");

	return LA_NO_ERROR;
}

LAErrorCode LABigIntLSR(LABigInt_t *a, size_t n, LABigInt_t *b){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))	return LA_ERROR_BIG_INT;

	// Early return if n == 0
	if(n == 0) return LA_NO_ERROR;

	LAErrorCode code = LA_NO_ERROR;

	const size_t limbShift = n / LA_BIG_INT_BIT_SIZE;
	const size_t bitShift  = n % LA_BIG_INT_BIT_SIZE;

	code = LABigIntShiftLimbs(a, b, limbShift, true);
	if(code) return code;

	code = LABigIntBitShift(b, NULL, bitShift, true, false);
	if(code) return code;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntLSRInplace(LABigInt_t *a, size_t n){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;

	// Early return if n == 0
	if(n == 0) return LA_NO_ERROR;

	LAErrorCode code = LA_NO_ERROR;

	const size_t limbShift = n / LA_BIG_INT_BIT_SIZE;
	const size_t bitShift  = n % LA_BIG_INT_BIT_SIZE;

	code = LABigIntShiftLimbsInplace(a, limbShift, true);
	if(code) return code;

	code = LABigIntBitShift(a, NULL, bitShift, true, false);
	if(code) return code;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntRSL(LABigInt_t *a, size_t n, LABigInt_t *b){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))	return LA_ERROR_BIG_INT;

	// Early return if n == 0
	if(n == 0) return LA_NO_ERROR;

	LAErrorCode code = LA_NO_ERROR;

	const size_t limbShift = n / LA_BIG_INT_BIT_SIZE;
	const size_t bitShift  = n % LA_BIG_INT_BIT_SIZE;

	code = LABigIntRotateLimbs(a, b, limbShift, false);
	if(code) return code;

	code = LABigIntBitShift(b, NULL, bitShift, false, true);
	if(code) return code;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntRSLInplace(LABigInt_t *a, size_t n){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;

	// Early return if n == 0
	if(n == 0) return LA_NO_ERROR;

	LAErrorCode code = LA_NO_ERROR;

	const size_t limbShift = n / LA_BIG_INT_BIT_SIZE;
	const size_t bitShift  = n % LA_BIG_INT_BIT_SIZE;

	code = LABigIntRotateLimbsInplace(a, limbShift, false);
	if(code) return code;

	code = LABigIntBitShift(a, NULL, bitShift, false, true);
	if(code) return code;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntRSR(LABigInt_t *a, size_t n, LABigInt_t *b){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))	return LA_ERROR_BIG_INT;

	// Early return if n == 0
	if(n == 0) return LA_NO_ERROR;

	LAErrorCode code = LA_NO_ERROR;

	const size_t limbShift = n / LA_BIG_INT_BIT_SIZE;
	const size_t bitShift  = n % LA_BIG_INT_BIT_SIZE;

	code = LABigIntRotateLimbs(a, b, limbShift, true);
	if(code) return code;

	code = LABigIntBitShift(b, NULL, bitShift, true, true);
	if(code) return code;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntRSRInplace(LABigInt_t *a, size_t n){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;

	// Early return if n == 0
	if(n == 0) return LA_NO_ERROR;

	LAErrorCode code = LA_NO_ERROR;

	const size_t limbShift = n / LA_BIG_INT_BIT_SIZE;
	const size_t bitShift  = n % LA_BIG_INT_BIT_SIZE;

	code = LABigIntRotateLimbsInplace(a, limbShift, true);
	if(code) return code;

	code = LABigIntBitShift(a, NULL, bitShift, true, true);
	if(code) return code;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntLSRR(LABigInt_t *a, size_t n, LABigInt_t *b){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))	return LA_ERROR_BIG_INT;

	// Early return if n == 0
	if(n == 0) 										return LA_NO_ERROR;
	if(n >= (LA_BIG_INT_BIT_SIZE * a->limbCount)) 	return LA_NO_ERROR;

	LAErrorCode code = LA_NO_ERROR;

	const size_t limbShift = n / LA_BIG_INT_BIT_SIZE;
	const size_t bitShift  = n % LA_BIG_INT_BIT_SIZE;

	const size_t k = n-1;
	const size_t limbShiftK = k / LA_BIG_INT_BIT_SIZE;
	const size_t bitShiftK  = k % LA_BIG_INT_BIT_SIZE;
	size_t bitK = (a->limbs[limbShiftK] >> bitShiftK) & 0x1;

	code = LABigIntShiftLimbs(a, b, limbShift, true);
	if(code) return code;

	code = LABigIntBitShift(b, NULL, bitShift, true, false);
	if(code) return code;

	if(bitK == 0x1){
		LABigIntSign_t sign = b->sign;
		b->sign = LA_BIG_INT_POSITIVE;
		code = LABigIntAddUIntInplace(b, 0x1);
		if(code) return code;
		b->sign = sign;
	}

	return LA_NO_ERROR;
}

LAErrorCode LABigIntLSRRInplace(LABigInt_t *a, size_t n){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;

	// Early return if n == 0
	if(n == 0) 										return LA_NO_ERROR;
	if(n >= (LA_BIG_INT_BIT_SIZE * a->limbCount)) 	return LA_NO_ERROR;

	LAErrorCode code = LA_NO_ERROR;

	const size_t limbShift = n / LA_BIG_INT_BIT_SIZE;
	const size_t bitShift  = n % LA_BIG_INT_BIT_SIZE;

	const size_t k = n-1;
	const size_t limbShiftK = k / LA_BIG_INT_BIT_SIZE;
	const size_t bitShiftK  = k % LA_BIG_INT_BIT_SIZE;
	size_t bitK = (a->limbs[limbShiftK] >> bitShiftK) & 0x1;

	code = LABigIntShiftLimbsInplace(a, limbShift, true);
	if(code) return code;

	code = LABigIntBitShift(a, NULL, bitShift, true, false);
	if(code) return code;

	if(bitK == 0x1){
		LABigIntSign_t sign = a->sign;
		a->sign = LA_BIG_INT_POSITIVE;
		code = LABigIntAddUIntSatInplace(a, 0x1);
		if(code) return code;
		a->sign = sign;
	}

	return LA_NO_ERROR;
}
