#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "laBigInt.h"

LAErrorCode LABigIntDiv_integer_backend(LABigInt_t *a, uint32_t b, uint32_t *rem){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;

	uint64_t 	   acc 		= 0x0;
	LABigIntLimb_t limbL 	= 0x0;
	LABigIntLimb_t limbH 	= 0x0;
	LABigIntLimb_t acc2 	= 0x0;
	LABigIntLimb_t carry2 	= 0x0;

	for(size_t i = 0; i < a->limbCount; i++){
//		size_t k = a->limbCount - 1 - i;

		acc = (((uint64_t)a->limbs[i]) << LA_BIG_INT_BIT_SIZE) / ((uint64_t)b);
		limbL = acc & 0xFFFFFFFF;
		limbH = acc >> LA_BIG_INT_BIT_SIZE;
		
		printf("i: %lu\tacc: %016lX\n", i, acc);

		a->limbs[i] = limbH;

		if((limbL > 0) && (i == 0)){
			if(rem != NULL) (*rem) = ((uint64_t)limbL * (uint64_t)b) >> LA_BIG_INT_BIT_SIZE;
		} else if((limbL == 0) && (i == 0)){
			if(rem != NULL) (*rem) = 0x0;
		} else if((limbL > 0) && (i > 0)){

			acc2 = a->limbs[i-1] + limbL;
			carry2 = (acc2 < a->limbs[i-1]) ? 0x1 : 0x0;
			a->limbs[i-1] = acc2;

			// Unhandled carry
			a->limbs[i] += carry2;
		}
	}

	return LA_NO_ERROR;
}

LAErrorCode LABigIntDiv_restoring_backend(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(c)))	return LA_ERROR_BIG_INT;

	LAErrorCode code = LA_NO_ERROR;

	LABigInt_t ca, cb; // copy of a, b
	code = LABigIntCreateFromOther(&ca, a, a->limbCount + 1);
	if(code) goto cleanup;
	code = LABigIntCreateFromOther(&cb, b, a->limbCount + 1);
	if(code) goto cleanup;
	code = LABigIntLimbSet(c, 0, c->limbCount, 0x0);
	if(code) goto cleanup;

	size_t lzA = 0x0;
	size_t lzB = 0x0;
	code = LABigIntCLZ(&ca,  &lzA, LA_BIG_INT_CLZ_BUILTIN);
	if(code) goto cleanup;
	code = LABigIntCLZ(&cb, &lzB, LA_BIG_INT_CLZ_BUILTIN);
	if(code) goto cleanup;
	size_t tzA = 0x0;
	code = LABigIntCTZ(&ca, &tzA);
	if(code) goto cleanup;
	size_t sizeA = (LA_BIG_INT_BIT_SIZE * ca.limbCount) - lzA;
	size_t sizeB = (LA_BIG_INT_BIT_SIZE * cb.limbCount) - lzB;

//	printf("lzA: %lu\n", lzA);

	code = LABigIntLSLInplace(&ca,   lzA - (LA_BIG_INT_BIT_SIZE + 1));
	if(code) goto cleanup;
	code = LABigIntLSLInplace(&cb, lzB - LA_BIG_INT_BIT_SIZE);
	if(code) goto cleanup;

	LABigIntSign_t signA = a->sign;
	LABigIntSign_t signB = b->sign;
	ca.sign = LA_BIG_INT_POSITIVE;
	cb.sign = LA_BIG_INT_POSITIVE;
	
	for(size_t i = 0; i < sizeA /*!(LABigIntIsZero(a))*/; i++){
		code = LABigIntLSLInplace(c, 1);
		if(code) goto cleanup;

		if(LABigIntIsHigherOrEqual(&ca, &cb)){
			code = LABigIntSubInplace(&ca, &cb);
			if(code) goto cleanup;
			code = LABigIntAddUIntInplace(c, 0x1);
			if(code) goto cleanup;
		} else {
			code = LABigIntAddUIntInplace(c, 0x0);
			if(code) goto cleanup;
		}

		code = LABigIntLSLInplace(&ca, 1);
		if(code) goto cleanup;
	}

	if(sizeB >= 2){
		code = LABigIntLSRInplace(c, sizeB - 2);
		if(code) goto cleanup;
	}


	a->sign = signA;
	b->sign = signB;

	if(signA != signB) c->sign = LA_BIG_INT_NEGATIVE;
	else               c->sign = LA_BIG_INT_POSITIVE;

	goto cleanup;

cleanup:
	LABigIntDestroy(&ca);
	LABigIntDestroy(&cb);
	return code;
}

LAErrorCode LABigIntDiv_non_restoring_backend(LABigInt_t *q, LABigInt_t *m, LABigInt_t *a){
	LA_HANDLE_NULLPTR(q, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(q)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(m)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;

	LAErrorCode code = LA_NO_ERROR;

	size_t lzQ = 0x0;
	code = LABigIntCLZ(q, &lzQ, LA_BIG_INT_CLZ_BUILTIN);
	if(code) goto cleanup;
	const size_t N = (q->limbCount * LA_BIG_INT_BIT_SIZE) - lzQ;
	code = LABigIntLSLInplace(q, lzQ);
	if(code) goto cleanup;

	// Clear A
	code = LABigIntLimbSet(a, 0, a->limbCount, 0x0);
	if(code) goto cleanup;
	a->sign = LA_BIG_INT_POSITIVE;
	q->sign = LA_BIG_INT_POSITIVE;
	m->sign = LA_BIG_INT_POSITIVE;
	bool isNeg = LABigIntIsNeg(a);

	for(size_t i = 0; i < N; i++){
		uint64_t bit = (q->limbs[q->limbCount-1] >> (LA_BIG_INT_BIT_SIZE - 1)) & 0x1;

		code = LABigIntLSLInplace(q, 0x1);
		if(code) goto cleanup;
		code = LABigIntLSLInplace(a, 0x1);
		if(code) goto cleanup;
		code = LABigIntAddUIntInplace(a, bit);
		if(code) goto cleanup;

		if(isNeg){
			code = LABigIntAddInplace(a, m);
			if(code) goto cleanup;
		} else {
			code = LABigIntSubInplace(a, m);
			if(code) goto cleanup;
		}
		
		isNeg = LABigIntIsNeg(a);
		q->limbs[0] &= ~(0x1);
		q->limbs[0] |= (isNeg) ? 0x0 : 0x1;
	}

	if(a->sign == LA_BIG_INT_NEGATIVE){
		code = LABigIntAddInplace(a, m);
		if(code) goto cleanup;
	}

	goto cleanup;

cleanup:
	return code;
}

/*		g(x)  =   1/x - b
		g'(x) =  -1/x^2

		x_n+1 = x_n - (1/x_n - b)/(-1/x_n^2)
		      = x_n + (1/x_n - b)x_n^2
			  = x_n + x_n - b*x_n^2
			  = x_n * (2 - b*x_n)

		c     = x * a
			    (x = 1/b)

	Using fixed point QN.N, where N is max(a->limbCount, b->limbCount)

	1. xn*N * b = b*xn*N, fixed point aligned
	
		 (2 - y) = -(-2 + y) = -(y - 2)
	2. bxn*N - 2*N = (b*xn*N - 2*N)
	3. -(b*xn*N - 2*N)

	4.	xn*N * -(b*xn*N - 2*N) = -(b*xn^2*N^2 - 2*xn*N^2)
							   = -(b*xn^2 - 2*xn) * N^2

	5. -(b*xn^2 - 2*xn) * N^2 / N = -(b*xn^2 - 2*xn) * N

	6. a * -(b*xn^2 - 2*xn) * N

	7. a * -(b*xn^2 - 2*xn) * N / N =  a * -(b*xn^2 - 2*xn)

	How to calculate x0

	(n.1) b = 2^e * m
			e = size(b) - clz(b) - 1
				0001, e = 4 - 3 - 1 = 0,  1 * 1.000
				0111, e = 4 - 1 - 1 = 2,  4 * 0.111
				b = 0 is undefined

		  0011 0000 (3.0), e = 1, m = 0.75 -> 3 = 2^1 * 0.75 = 3.0
	(n.2) >> e (1)
		  0001 1000 (1.5), (Everything should be between 1.0 and 2.0)

	2-+   |		 	 f(x) = 1/x      x = (0, 4)
	  |    |
	  |     \
	  |      \
	1-+       *^\
	  |          ^-_
	  |             ^^*----____
	  |________________________^^^^---*__>
	 0        |       |		  |       |  
	          1       2       3       4

	(1,1) -> (2, 0.5)	m (slope)  = -0.5
						n (offset) = 1.5

			y = mx + n	(y = x0 * 2^e, x = b/2^e * 1.m)

	(n.3) y = -(x >> 2) + 1.5

	(n.4) x0 = y / 2^e (y >> e)

*/

LAErrorCode LABigIntDiv_newton_backend(LABigInt_t *a, LABigInt_t *b, LABigInt_t *c){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(b)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(c)))	return LA_ERROR_BIG_INT;

	LAErrorCode code = LA_NO_ERROR;

	// length = 2^k = N = ks
	size_t length = (a->limbCount > b->limbCount) ? a->limbCount : b->limbCount;
	size_t ks     = LA_BIG_INT_BIT_SIZE * length;

	LABigInt_t xn;	// x_n
	LABigInt_t bxn;	// 2 - b * x_n
	LABigInt_t xn_1;	// x_n+1
	LABigInt_t two;	// 2

	code = LABigIntCreateFromOther(&xn, b, length * 2);
	if(code) goto cleanup;
	code = LABigIntCreate(&bxn, length * 2);
	if(code) goto cleanup;
	code = LABigIntCreate(&xn_1, length * 4);
	if(code) goto cleanup;
	code = LABigIntCreate(&two, length * 2);
	if(code) goto cleanup;

	code = LABigIntLSLInplace(&xn, ks);
	if(code) goto cleanup;
	code = LABigIntAddUIntInplace(&two, 2);
	if(code) goto cleanup;
	code = LABigIntLSLInplace(&two, ks);
	if(code) goto cleanup;

	/*******************************
		         Calculate x0
	********************************/

	// (n.1)
	size_t lzB = 0x0;
	code = LABigIntCLZ(b, &lzB, LA_BIG_INT_CLZ_BUILTIN);
	if(code) goto cleanup;

	size_t b_e = (LA_BIG_INT_BIT_SIZE * b->limbCount) - lzB - 1;

	// (n.2)
	code = LABigIntLSRInplace(&xn, b_e);
	if(code) goto cleanup;

	// (n.3)
	// x * 0.5
	code = LABigIntLSRInplace(&xn, 1);	
	if(code) goto cleanup;
	xn.sign = (xn.sign == LA_BIG_INT_POSITIVE) ? LA_BIG_INT_NEGATIVE : LA_BIG_INT_POSITIVE;

	code = LABigIntLSRInplace(&two, 1);
	if(code) goto cleanup;
	code = LABigIntAddInplace(&xn, &two);
	if(code) goto cleanup;
	code = LABigIntLSRInplace(&two, 1);
	if(code) goto cleanup;
	code = LABigIntAddInplace(&xn, &two);
	if(code) goto cleanup;
	code = LABigIntLSLInplace(&two, 2);
	if(code) goto cleanup;

	// (n.4)
	code = LABigIntLSRInplace(&xn, b_e);	
	if(code) goto cleanup;

	/*******************************
		Newton Raphson iterations
	********************************/
	size_t nmax = log(ks) / log(2.0);
	
	for(size_t i = 0; i < nmax; i++){
		// 1. bxn = b * x_n
		code = LABigIntMulSat(&xn, b, &bxn);
		if(code) goto cleanup;

		// 2. bxn - 2
		code = LABigIntSubSatInplace(&bxn, &two);
		if(code) goto cleanup;

		// 3. -(bxn)
		bxn.sign = (bxn.sign == LA_BIG_INT_POSITIVE) ? LA_BIG_INT_NEGATIVE : LA_BIG_INT_POSITIVE;
	
		// 4. xn_1 = bxn * xn
		code = LABigIntMulSat(&bxn, &xn, &xn_1);
		if(code) goto cleanup;

		// 5. xn = xn_1 >> k
		code = LABigIntLSRR(&xn_1, ks, &xn);
		if(code) goto cleanup;

		//code = LABigIntCopyLimbs(&xn_1, &xn, 0, 2*length);
		//if(code) goto cleanup;
	}

	// 6. xn_1 = a * xn
	code = LABigIntMulSat(&xn, a, &xn_1);
	if(code) goto cleanup;

	// 7. c = xn_1 >> k
	code = LABigIntLSRRInplace(&xn_1, ks);
	if(code) goto cleanup;

	code = LABigIntCopyLimbs(&xn_1, c, 0, length);
	if(code) goto cleanup;

	goto cleanup;

cleanup:
	LABigIntDestroy(&xn);
	LABigIntDestroy(&bxn);
	LABigIntDestroy(&xn_1);
	LABigIntDestroy(&two);
	return code;
}

LAErrorCode LABigIntDiv(LABigInt_t *q, LABigInt_t *m, LABigInt_t *a){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(q, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(q)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(m)))	return LA_ERROR_BIG_INT;
	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;

	LAErrorCode code = LA_NO_ERROR;
	LABigIntSign_t signQ = q->sign;
	LABigIntSign_t signM = m->sign;

	// Handle q is zero
	if(LABigIntIsZero(q)){
		code = LABigIntLimbSet(q, 0, q->limbCount, 0x0);
		if(code) return code;
		code = LABigIntLimbSet(a, 0, a->limbCount, 0x0);
		if(code) return code;
		return LA_NO_ERROR;
	};

	// Handle zero div
	if(LABigIntIsZero(m)) return LA_ERROR_ZERODIV;

	code = LABigIntDiv_non_restoring_backend(q, m, a);
	if(code) return code;

	if(LABigIntIsZero(a)){
		a->sign = LA_BIG_INT_POSITIVE;
	} else {
		a->sign = (signQ == signM) ? LA_BIG_INT_POSITIVE : LA_BIG_INT_NEGATIVE;
	}

	return LA_NO_ERROR;
}


LAErrorCode LABigIntDivInplace(LABigInt_t *a, LABigInt_t *b){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	return LA_NO_ERROR;
}

