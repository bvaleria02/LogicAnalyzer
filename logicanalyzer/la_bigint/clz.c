#include "../liblogicanalyzer.h"
#include "laBigInt.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

const uint8_t _laBigIntCLZlut[256] = {
	8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

size_t LABigIntCLZ_lut_backend(LABigIntLimb_t a){
	size_t c = 0;
	if(a == 0x0) return LA_BIG_INT_BIT_SIZE;
	static const size_t size = LA_BIG_INT_BIT_SIZE >> 3;

	for(size_t i = 0; i < size; i++){
		size_t  k      = size - 1 - i;
		uint8_t value = (a >> (k << 3)) & 0xFF;

		c += _laBigIntCLZlut[value];
		if(value > 0x0) break;
	}

	return c;
}

size_t LABigIntCLZ_realtime_backend(LABigIntLimb_t a){
	size_t c = 0;
	if(a == 0x0) return LA_BIG_INT_BIT_SIZE;

	for(size_t i = 0; i < LA_BIG_INT_BIT_SIZE; i++){
		size_t bit = (a >> (LA_BIG_INT_BIT_SIZE - 1 - i)) & 0x1;
		if(bit == 0x1){
			c = i;
			break;
		}
	}

	return c;
}

LAErrorCode LABigIntCLZ(LABigInt_t *a, size_t *b, LABigIntCLZMode mode){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	
	size_t value = 0;

	for(size_t i = 0; i < a->limbCount; i++){
		size_t k = a->limbCount - 1 - i;

		if(a->limbs[k] == 0x0){
			value += LA_BIG_INT_BIT_SIZE;
			continue;
		} else {
			switch(mode){
				case LA_BIG_INT_CLZ_BUILTIN: 	value += __builtin_clz(a->limbs[k]);
						break;
				case LA_BIG_INT_CLZ_LUT: 		value += LABigIntCLZ_lut_backend(a->limbs[k]);
						break;
				case LA_BIG_INT_CLZ_REALTIME: 	value += LABigIntCLZ_realtime_backend(a->limbs[k]);
						break;
			}
			break;
		}
	}

	(*b) = value;
	return LA_NO_ERROR;
}

LAErrorCode LABigIntCTZ(LABigInt_t *a, size_t *b){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(a)))	return LA_ERROR_BIG_INT;
	
	size_t value = 0;

	for(size_t i = 0; i < a->limbCount; i++){

		if(a->limbs[i] == 0x0){
			value += LA_BIG_INT_BIT_SIZE;
			continue;
		} else {
			value += __builtin_ctz(a->limbs[i]);
			break;
		}
	}

	(*b) = value;
	return LA_NO_ERROR;
}
