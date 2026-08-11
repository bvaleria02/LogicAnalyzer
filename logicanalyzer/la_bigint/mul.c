#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "laBigInt.h"

LAErrorCode LABigIntKaratsuba_core(LABigIntLimb_t x1, LABigIntLimb_t x2, LABigIntLimb_t y1, LABigIntLimb_t y2, uint64_t *z0, uint64_t *z1, uint64_t *z2){
	LA_HANDLE_NULLPTR(z0, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(z1, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(z2, LA_PROPAGATE_ERROR);

	(*z0) = (uint64_t)x1 * (uint64_t)y1;
	(*z1) = (uint64_t)x2 * (uint64_t)y2;
	(*z2) = (((uint64_t)x1 + (uint64_t)x2) * ((uint64_t)y1 + (uint64_t)y2)) - (*z1) - (*z0);

	return LA_NO_ERROR;
}

LAErrorCode LABigIntMul_schoolbook_backend(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c, const bool sat){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(c)))	return LA_ERROR_BIG_INT;
	
	LAErrorCode code = LA_NO_ERROR;

	code = LABigIntLimbSet(c, 0, c->limbCount, 0x0);
	if(code) return code;

	uint64_t acc = 0;
	LABigIntLimb_t limbL 	 = 0x0;
	LABigIntLimb_t limbH 	 = 0x0;
	LABigIntLimb_t acc_2	 = 0;
	LABigIntLimb_t carry_L 	 = 0x0;
	LABigIntLimb_t carry_H   = 0x0;
	bool lastCarry			 = false;

	for(size_t i = 0; i < a->limbCount; i++){
		for(size_t j = 0; j < b->limbCount; j++){
			size_t k = i + j;
			if(k >= c->limbCount) continue;

			acc   = (uint64_t)a->limbs[i] * (uint64_t)b->limbs[j];
			if(acc == 0x0) continue;

			limbL = acc & LA_BIG_INT_LIMB_MAX;
			limbH = acc >> LA_BIG_INT_BIT_SIZE;
			
			
			acc_2   = c->limbs[k] + limbL;
			carry_L = (acc_2 < c->limbs[k]) ? 0x1 : 0x0;
			c->limbs[k] = acc_2;

			if((k+1) >= c->limbCount) continue;

			acc_2	     	= c->limbs[k+1] + limbH;
			carry_H      	= (acc_2 < c->limbs[k+1]) ? 0x1 : 0x0;
			c->limbs[k+1]	= acc_2 + carry_L;
			carry_H 	   += (c->limbs[k+1] < acc_2) ? 0x1 : 0x0;
			
			code = LABigIntCarryPropagation(c, carry_H, k+2, &lastCarry);
			if(code) return code;
		}
	}

	if(lastCarry && sat){
		code = LABigIntLimbSet(c, 0, c->limbCount, LA_BIG_INT_LIMB_MAX);
		if(code) return code;
	}

	return LA_NO_ERROR;
}

LAErrorCode LABigIntMul_integer_backend(LABigInt_t *a, uint32_t b, const bool sat){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	
	LAErrorCode    code     = LA_NO_ERROR;
	uint64_t 	   acc 		= 0x0;
	LABigIntLimb_t limbL 	= 0x0;
	LABigIntLimb_t limbH 	= 0x0;
	LABigIntLimb_t acc2 	= 0x0;
	LABigIntLimb_t carry2 	= 0x0;
	bool		   overf	= false;

	for(size_t i = 0; i < a->limbCount; i++){
		size_t k = a->limbCount - 1 - i;

		acc = (uint64_t)a->limbs[k] * (uint64_t)b;
		limbL = acc & LA_BIG_INT_LIMB_MAX;
		limbH = acc >> LA_BIG_INT_BIT_SIZE;
		if((limbH != 0) && (i == 0))	overf = true;

		a->limbs[k] = limbL;

		if(limbH > 0){
			carry2 = limbH;
			for(size_t j = k+1; j < a->limbCount; j++){
				if(j >= a->limbCount) break;

				acc2 = a->limbs[j] + carry2;
				carry2 = (acc2 < a->limbs[j]) ? 0x1 : 0x0;
				a->limbs[j] = acc2;
			}
		}
	}

	if(sat && overf){
		code = LABigIntLimbSet(a, 0, a->limbCount, LA_BIG_INT_LIMB_MAX);
		if(code) return code;
	}

	return LA_NO_ERROR;
}
/*
LAErrorCode LABigIntMul_karatsuba_backend(LABigInt_t *x, LABigInt_t *y, LABigInt_t *z, const size_t startX, const size_t endX, const size_t startY, const size_t endY ){
	LA_HANDLE_NULLPTR(x, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(z, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(x)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(y)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(z)))	return LA_ERROR_BIG_INT;

	size_t sX = (startX >= x->limbCount) ? x->limbCount - 1 : startX;
	size_t eX = (endX   >= x->limbCount) ? x->limbCount - 1 : endX;
	size_t sY = (startY >= y->limbCount) ? y->limbCount - 1 : startY;
	size_t eY = (endY   >= y->limbCount) ? y->limbCount - 1 : endY;
	if(eX > sX){
		size_t temp = sX;
		eX = sX;
		sX = temp;
	}
	if(eY > sY){
		size_t temp = sY;
		eY = sY;
		sY = temp;
	}
	size_t lX = eX - sX + 1;
	size_t lY = eY - sY + 1;
	if(c->limbCount > (lX + lY)) return LA_ERROR_BIG_INT;

	if((lX == 1) && (lY == 1)){
		uint64_t acc = (uint64_t)x->limbs[sX] * (uint64_t)y->limbs[sY];
		z->limbs[0] = acc & LA_BIG_INT_LIMB_MAX;
		z->limbs[1] = acc >> LA_BIG_INT_BIT_SIZE;
	} else if((lX == 2) && (lY == 1)){
		uint64_t acc1 = (uint64_t)x->limbs[sX]   * (uint64_t)y->limbs[sY];
		uint64_t acc2 = (uint64_t)x->limbs[sX+1] * (uint64_t)y->limbs[sY];

		z->limbs[0] = acc1 & LA_BIG_INT_LIMB_MAX;
		z->limbs[1] = (acc1 >> LA_BIG_INT_BIT_SIZE) + (acc2 & LA_BIG_INT_LIMB_MAX);
		z->limbs[2] = acc2 >> LA_BIG_INT_BIT_SIZE;
			
		if((z->limbs[1] < (acc1 >> LA_BIG_INT_BIT_SIZE)){
			z->limbs[2] += 1;
		}
	} else if((lX == 1) && (lY == 2)){
		uint64_t acc1 = (uint64_t)x->limbs[sX]   * (uint64_t)y->limbs[sY];
		uint64_t acc2 = (uint64_t)x->limbs[sX]   * (uint64_t)y->limbs[sY+1];

		z->limbs[0] = acc1 & LA_BIG_INT_LIMB_MAX;
		z->limbs[1] = (acc1 >> LA_BIG_INT_BIT_SIZE) + (acc2 & LA_BIG_INT_LIMB_MAX);
		z->limbs[2] = acc2 >> LA_BIG_INT_BIT_SIZE;
			
		if((z->limbs[1] < (acc1 >> LA_BIG_INT_BIT_SIZE)){
			z->limbs[2] += 1;
		}
	} else if((lX == 2) && (lY == 2)){
		z->limbs[0] = x->limbs[sX] * y->limbs[sY];
		uint8_t carry0 = (z->limbs[0] < x->limbs[sX]) ? 0x1 : 0x0;

		z->limbs[2] = x->limbs[sX+1] * y->limbs[sY+1];
		uint8_t carry2 = (z->limbs[2] < x->limbs[sX+1]) ? 0x1 : 0x0;

		LABigIntLimb_t sum1 = x->limbs[sX]   + y->limbs[sY];
		LABigIntLimb_t sum2 = x->limbs[sX+1] + y->limbs[sY+1];
		z->limbs[1] = sum1 * sum2;

		z->limbs[1] -= z->limbs[0];
		z->limbs[1] -= z->limbs[2];

	} else if((lX >  2) && (lY == 1)){
	} else if((lX >  2) && (lY == 2)){
	} else if((lX == 1) && (lY >  2)){
	} else if((lX == 2) && (lY >  2)){
	} else if((lX >  2) && (lY >  2)){
	}


	return LA_NO_ERROR;
}
*/
LAErrorCode LABigIntMul(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(c)))	return LA_ERROR_BIG_INT;

	LABigIntSign_t signA = a->sign;
	LABigIntSign_t signB = b->sign;

	a->sign = LA_BIG_INT_POSITIVE;
	b->sign = LA_BIG_INT_POSITIVE;
	c->sign = LA_BIG_INT_POSITIVE;

	LAErrorCode code = LA_NO_ERROR;

	code = LABigIntMul_schoolbook_backend(a, b, c, false);
	if(code) return code;

	c->sign = (signA != signB) ? LA_BIG_INT_NEGATIVE : LA_BIG_INT_POSITIVE;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntMulSat(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(c)))	return LA_ERROR_BIG_INT;

	LABigIntSign_t signA = a->sign;
	LABigIntSign_t signB = b->sign;

	a->sign = LA_BIG_INT_POSITIVE;
	b->sign = LA_BIG_INT_POSITIVE;
	c->sign = LA_BIG_INT_POSITIVE;

	LAErrorCode code = LA_NO_ERROR;

	code = LABigIntMul_schoolbook_backend(a, b, c, true);
	if(code) return code;

	c->sign = (signA != signB) ? LA_BIG_INT_NEGATIVE : LA_BIG_INT_POSITIVE;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntMulUIntInplace(LABigInt_t *a, uint32_t b){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;

	LAErrorCode code = LA_NO_ERROR;
	LABigIntSign_t signA = a->sign;
	a->sign = LA_BIG_INT_POSITIVE;

	code = LABigIntMul_integer_backend(a, b, false);
	if(code) return code;

	a->sign = signA;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntMulUIntSatInplace(LABigInt_t *a, uint32_t b){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;

	LAErrorCode code = LA_NO_ERROR;
	LABigIntSign_t signA = a->sign;
	a->sign = LA_BIG_INT_POSITIVE;

	code = LABigIntMul_integer_backend(a, b, true);
	if(code) return code;

	a->sign = signA;

	return LA_NO_ERROR;
}


LAErrorCode LABigIntMulIntInplace(LABigInt_t *a, int32_t b){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;

	LAErrorCode code = LA_NO_ERROR;

	LABigIntSign_t signA = a->sign;
	LABigIntSign_t signB = (b < 0) ? LA_BIG_INT_NEGATIVE : LA_BIG_INT_POSITIVE;
	a->sign = LA_BIG_INT_POSITIVE;

	code = LABigIntMul_integer_backend(a, b, false);
	if(code) return code;

	a->sign = (signA != signB) ? LA_BIG_INT_NEGATIVE : LA_BIG_INT_POSITIVE;

	return LA_NO_ERROR;
}

LAErrorCode LABigIntMulIntSatInplace(LABigInt_t *a, int32_t b){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;

	LAErrorCode code = LA_NO_ERROR;

	LABigIntSign_t signA = a->sign;
	LABigIntSign_t signB = (b < 0) ? LA_BIG_INT_NEGATIVE : LA_BIG_INT_POSITIVE;
	a->sign = LA_BIG_INT_POSITIVE;

	code = LABigIntMul_integer_backend(a, b, true);
	if(code) return code;

	a->sign = (signA != signB) ? LA_BIG_INT_NEGATIVE : LA_BIG_INT_POSITIVE;

	return LA_NO_ERROR;
}


