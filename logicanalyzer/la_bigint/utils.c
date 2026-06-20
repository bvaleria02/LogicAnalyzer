#include "../liblogicanalyzer.h"
#include "laBigInt.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

uint64_t LAMin3_uint64(uint64_t n1, uint64_t n2, uint64_t n3){
	// Min, 3 inputs, unsigned int 64 bits 
	uint64_t value = (n1 <= n2) ? n1 : n2;
	return  (value <= n3) ? value : n3;
}

LAErrorCode LABigIntSymmetricalCopy(LABigInt_t *dest, LABigInt_t *src, const size_t startIndex){
	// Copy from src to dest, keep the same index, until src or dest are out of bound

	LA_HANDLE_NULLPTR(src, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(dest, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(src)))		return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(dest)))	return LA_ERROR_BIG_INT;

	// Do nothing if start is out of bound
	if((startIndex >= src->limbCount) || (startIndex >= dest->limbCount)) return LA_NO_ERROR;

	const size_t length = (src->limbCount < dest->limbCount) ? (src->limbCount - startIndex) : (dest->limbCount - startIndex);

	void *ret = memcpy(&(dest->limbs[startIndex]), &(src->limbs[startIndex]), length * sizeof(LABigIntLimb_t));
	if(ret == NULL) return LA_ERROR_NULLPTR;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntCarryPropagation(LABigInt_t *a, const bool initialCarry, const size_t index, bool *outputCarry){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;

	// End if a is smaller than index
	if(index >= a->limbCount) return LA_NO_ERROR;

	LABigIntLimb_t carry = initialCarry;
	LABigIntLimb_t acc   = 0;

	for(size_t i = index; i < a->limbCount; i++){
		acc = a->limbs[i] + carry;
		carry = (acc < a->limbs[i]) ? 0x1 : 0x0;
		a->limbs[i] = acc;
	}

	if(outputCarry != NULL) (*outputCarry) = carry & 0x1;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntLeftCopy2(LABigInt_t *dest, LABigInt_t *s1, LABigInt_t *s2){
	// Left copy menas "Copy the remaining limbs from either s1 or s2 to dest, if s1 and s2 aren't the same length"
	// 2 means two inputs

	LA_HANDLE_NULLPTR(dest, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(s1, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(s2, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(dest)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(s1)))		return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(s2)))		return LA_ERROR_BIG_INT;

	size_t	index  = LAMin3_uint64(s1->limbCount, s2->limbCount, dest->limbCount);
	LAErrorCode code = LA_NO_ERROR;

	// Early return if index is higher than dest length
	if(index >= dest->limbCount) return LA_NO_ERROR;

	// Copy extra data from:
	if(index < s1->limbCount){
		// s1 if s2 is smaller than s1
		code = LABigIntSymmetricalCopy(dest, s1, index);
	} else if(index < s2->limbCount){
		// s2 if s1 is smaller than s2
		code = LABigIntSymmetricalCopy(dest, s2, index);
	}
	if(code) return code;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntBorrowPropagation(LABigInt_t *a, const bool initialBorrow, const size_t index, bool *outputBorrow){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;

	// End if a is smaller than index
	if(index >= a->limbCount) return LA_NO_ERROR;

	LABigIntLimb_t borrow = initialBorrow;
	LABigIntLimb_t acc    = 0;

	for(size_t i = index; i <= a->limbCount; i++){
		acc = a->limbs[i] - borrow;
		borrow = (acc > a->limbs[i]) ? 0x1 : 0x0;
		a->limbs[i] = acc;
	}

	if(outputBorrow != NULL) (*outputBorrow) = borrow & 0x1;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntLimbSet(LABigInt_t *m, const size_t index, const size_t length, LABigIntLimb_t value){
	// Memset, but for limbs
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(m)))	return LA_ERROR_BIG_INT;

	if(index >= m->limbCount) return LA_NO_ERROR;

	size_t indexEnd = ((index + length) >= m->limbCount) ? m->limbCount : (index + length);

	for(size_t i = index; i < indexEnd; i++){
		// printf("\ti: %lu\n", i);
		m->limbs[i] = value;
	}

	return LA_NO_ERROR;
}

LAErrorCode LABigIntCopyLimbs(LABigInt_t *a, LABigInt_t *b, const size_t start, const size_t length){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))					return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))					return LA_ERROR_BIG_INT;
	
	const size_t end  = ((start + length) >= a->limbCount) ? a->limbCount : (start + length);
	const size_t end2 = (end >= b->limbCount) ? b->limbCount : end;

	if(start >= a->limbCount)	return LA_NO_ERROR;
	if(start >= b->limbCount)	return LA_NO_ERROR;

	for(size_t i = start; i < end2; i++){
		b->limbs[i] = a->limbs[i];
	}

	return LA_NO_ERROR;
}

LAErrorCode LABigIntClearBits(LABigInt_t *a, const size_t start, const size_t end){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))					return LA_ERROR_BIG_INT;

	if(start > end) return LA_NO_ERROR;

	for(size_t i = start; i < end; i++){
		size_t limb = i / LA_BIG_INT_BIT_SIZE;
		size_t bit  = i % LA_BIG_INT_BIT_SIZE;

		if(limb >= a->limbCount) break;

		a->limbs[limb] &= ~(0x1 << bit);
	}

	return LA_NO_ERROR;
}
