#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "fixedpoint.h"
#include "multin.h"

void multiNSetInt(multiNBuffer *y, uint8_t size, uint64_t value){
	if(y == NULL) return;
	for(uint8_t i = 0; i < 2; i++){
		if(i >= size){
			break;
		}
		y[i] = (value >> (i * F64_RSHIFT)) & F64_LMASK;
	}	
}

void multiNBlank(multiNBuffer *y, uint64_t size){
	if(y == NULL) return;
	for(uint64_t i = 0; i < size; i++){
		y[i] = 0;
	}
}

void multiNMul(multiNBuffer *y1, uint64_t size1, multiNBuffer *y2, uint64_t size2, multiNBuffer *a, uint64_t size3){
	if(y1 == NULL || y2 == NULL || a == NULL){
		return;
	}
	
	uint64_t index = 0;
	uint64_t acc = 0;
	uint64_t tmp = 0;
	uint64_t overflow = 0;
	for(uint64_t i = 0; i < size1; i++){
		for(uint64_t j = 0; j < size2; j++){
			overflow = 0;
			index = i + j;

			if(index >= size3){
				break;
			};

			acc = ((uint64_t) y1[i] * (uint64_t) y2[j]);
			tmp = acc;
			acc += (uint64_t) a[index];
			if(acc < tmp){
				overflow++;		
			};

			a[index] = acc & F64_LMASK;

			index += 1;
			if(index >= size3){
				break;
			};

			tmp = a[index];
			a[index] += (acc >> (F64_SIZE >> 1));
			if(a[index] < tmp){
				overflow++;		
			}
			
			while(overflow != 0){
				index += 1;
				if(index >= size3){
					break;
				}

				tmp = a[index];
				a[index] += overflow;
				if(a[index] < tmp){
					overflow = 1;
				} else {
					overflow = 0;
				}
			}
		}
	}
}	

void multiNPrint(multiNBuffer *y, uint64_t size){
#ifdef TARGET_CAN_PRINTF
	if(y == NULL) return;
	for(uint64_t i = 0; i < size; i++){
		printf("%08X - ", y[size - 1 - i]);
	}
	printf("\n");
#else
	return;
#endif
}

void multiNPrintBinary(multiNBuffer *y, uint64_t size){
#ifdef TARGET_CAN_PRINTF
	if(y == NULL) return;

	uint64_t absSize = size * F32_SIZE;
	uint8_t  bit;

	for(uint64_t i = 0; i < absSize; i++){
		bit = multiNGetBit(y, size, absSize - 1 - i);
		printf("%1X", bit);
		if((i & 0x7) == 0x7){
			printf(" ");
		}

		if((i & 0x1F) == 0x1F){
			printf("- ");
		}
	}
	printf("\n");
#else
	return;
#endif
}


void multiNIndex(multiNBuffer *y, uint64_t size, uint64_t index, uint64_t *highIndex, uint64_t *lowIndex){
	if(y == NULL || highIndex == NULL || lowIndex == NULL){
		return;
	}
	
	if(index >= (size * F32_SIZE)){
		return;
	}

	(*lowIndex) 	= index &  U32_MASK;
	(*highIndex) 	= index >> U32_SIZE;
}

uint8_t multiNGetBit(multiNBuffer *y, uint64_t size, uint64_t index){
	if(y == NULL || size == 0){
		return 0;
	}

	uint64_t  lowIndex;
	uint64_t highIndex;
	multiNIndex(y, size, index, &highIndex, &lowIndex);

	if(highIndex >= size){
		return 0;
	};

	return (y[highIndex] >> lowIndex) & 0x1;
}

void multiNSetBit(multiNBuffer *y, uint64_t size, uint64_t index, uint8_t value){
	if(y == NULL || size == 0){
		return;
	}

	uint64_t  lowIndex;
	uint64_t highIndex;
	multiNIndex(y, size, index, &highIndex, &lowIndex);

	if(highIndex >= size){
		return;
	};
	
	uint32_t tmp = y[highIndex];

	tmp = tmp & ~(0x1 << lowIndex);
	tmp = tmp | ((value & 0x1) << lowIndex);
	
	y[highIndex] = tmp;
}

void multiNLeftShift(multiNBuffer *y, uint64_t size, uint64_t shift){
	if(y == NULL || size == 0 || shift == 0){
		return;
	}

	uint64_t absSize = size * F32_SIZE;
	uint64_t idxSrc;
	uint64_t idxDest;
	uint8_t  bit;

	for(uint64_t i = 0; i < absSize; i++){
		idxSrc  = absSize - 1 - i;
		idxDest = idxSrc + shift;

		if(idxDest >= absSize){
			continue;
		}

		bit = multiNGetBit(y, size, idxSrc);
		multiNSetBit(y, size, idxDest, bit);
		multiNSetBit(y, size, idxSrc,    0);
	}

}

void multiNRightShift(multiNBuffer *y, uint64_t size, uint64_t shift){
	if(y == NULL || size == 0 || shift == 0){
		return;
	}

	uint64_t absSize = size * F32_SIZE;
	uint64_t idxSrc;
	int64_t  idxDest;
	uint8_t  bit;

	for(uint64_t i = 0; i < absSize; i++){
		idxSrc  = i;
		idxDest = idxSrc - shift;

		if(idxDest < 0){
			continue;
		}

		bit = multiNGetBit(y, size, idxSrc);
		multiNSetBit(y, size, idxDest, bit);
		multiNSetBit(y, size, idxSrc,    0);
	}

}

void multiNAdd(multiNBuffer *y1, uint64_t size1, multiNBuffer *y2, uint64_t size2, multiNBuffer *a, uint64_t size3){
	if(y1 == NULL || size1 == 0 || y2 == NULL || size2 == 0 || a == NULL || size3 == 0){
		return;
	}

	uint32_t acc 			= 0;
	uint32_t tmp 			= 0;
	uint8_t  overflow 		= 0;
	uint64_t index 			= 0;
	uint64_t indexCounter 	= 0;

	for(uint64_t i = 0; i < size3; i++){
		if(i >= size1 || i >= size2){
			break;
		}
	
		indexCounter = 1;	
		overflow = 0;

		acc = y1[i];
		tmp = acc;
		acc = acc + y2[i];
		if(tmp > acc){
			overflow++;
		}

		tmp = acc;
		acc = acc + a[i];
		if(tmp > acc){
			overflow++;
		}
		
		a[i] = acc;
		
		while(overflow > 0){
			index = i + indexCounter;
			if(index >= size3){
				break;
			}

			acc = a[index];
			tmp = acc;
			acc = acc + overflow;
			if(tmp > acc){
				overflow = 1;
			} else {
				overflow = 0;
			}
				
			a[index] = acc;
			indexCounter++;
		}

	}

}

void multiNSub(multiNBuffer *y1, uint64_t size1, multiNBuffer *y2, uint64_t size2, multiNBuffer *a, uint64_t size3){
	if(y1 == NULL || size1 == 0 || y2 == NULL || size2 == 0 || a == NULL || size3 == 0){
		return;
	}

	uint32_t acc 			= 0;
	uint32_t tmp 			= 0;
	uint8_t  underflow 		= 0;
	uint64_t index 			= 0;
	uint64_t indexCounter 	= 0;

	for(uint64_t i = 0; i < size3; i++){
		if(i >= size1 || i >= size2){
			continue;
		}
	
		indexCounter = 1;	
		underflow = 0;

		acc = y1[i];
		tmp = acc;
		acc = acc - y2[i];
		if(tmp < acc){
			underflow++;
		}

		tmp = acc;
		acc = acc - a[i];
		if(tmp < acc){
			underflow++;
		}
		
		a[i] = acc;
		
		while(underflow > 0){
			index = i + indexCounter;
			if(index >= size3){
				break;
			}

			acc = a[index];
			tmp = acc;
			acc = acc - underflow;
			if(tmp < acc){
				underflow = 1;
			} else {
				underflow = 0;
			}
				
			a[index] = acc;
			indexCounter++;
		}

	}
}

uint64_t multiNAbsSize(multiNBuffer *y, uint64_t size){
	if(y == NULL || size == 0){
		return 0;
	}

	uint64_t absSize = size * F32_SIZE;
	uint8_t  bit     = 0;
	for(uint64_t i = 0; i < absSize; i++){
		bit = multiNGetBit(y, size, absSize - 1 - i);
		if(bit == 0x1){
			absSize = absSize - 1 - i;
			break;
		}
	}
	return absSize;
}

int8_t multiNCompare(multiNBuffer *y1, uint64_t size1, multiNBuffer *y2, uint64_t size2){
	 if(y1 == NULL || size1 == 0 || y2 == NULL || size2 == 0){
		return 0; 
	}

	uint64_t biggestSize;
	if(size1 > size2){
		biggestSize = size1;
	} else {
		biggestSize = size2;
	}

	uint64_t index = 0;
	uint32_t xa	   = 0;
	uint32_t xb	   = 0;

	for(uint64_t i = 0; i < biggestSize; i++){
		index = biggestSize - 1 - i;
		if(index >= size1){
			xa = 0;
		} else {
			xa = y1[index];
		}

		if(index >= size2){
			xb = 0;
		} else {
			xb = y2[index];
		}

		/*
		printf("xa: %08x\n", (uint32_t) xa);
		printf("xb: %08x\n", (uint32_t) xb);
		*/
		if(xa > xb){
			return 1;
		} else if (xa < xb){
			return -1;
		};
	}

	return 0;
}

uint8_t multiNIsZero(multiNBuffer *y, uint64_t size){
	if(y == NULL || size == 0){
		return 0;
	}

	for(uint64_t i = 0; i < size; i++){
		if(y[i] != 0){
			return 0;
		}
	}
	return 1;
}

void multiNCopy(multiNBuffer *x, uint64_t sizeX, multiNBuffer *y, uint64_t sizeY){
	if(x == NULL || sizeX == 0 || y == NULL || sizeY == 0){
		return;
	}
	
	multiNBlank(y, sizeY);
	for(uint64_t i = 0; i < sizeX; i++){
		if(i >= sizeX){
			continue;
		}

		if(i >= sizeY){
			continue;
		}
		
		y[i] = x[i];
	}
}

void multiNDiv(multiNBuffer *y1, uint64_t size1, multiNBuffer *y2, uint64_t size2, multiNBuffer *a, uint64_t size3, multiNBuffer *b, uint64_t size4, multiNBuffer *tmp1, multiNBuffer *tmp2){
	if(y1 == NULL || size1 == 0 || y2 == NULL || size2 == 0 || a == NULL || size3 == 0 || b == NULL || size4 == 0){
		return;
	}

	multiNBuffer *dividend 	= y1;
	multiNBuffer *divisor	= y2;
	multiNBuffer *quotient	= a;
	multiNBuffer *remainder = b;

	if(tmp1 != NULL){
#ifdef DEBUG_MODE
		printf("tmp1 is not NULL\n");
#endif
		multiNCopy(y1, size1, tmp1, size1);
		dividend = tmp1;
#ifdef DEBUG_MODE
		multiNPrintBinary(y1, size1);
		multiNPrintBinary(tmp1, size1);
#endif
	}

	if(tmp2 != NULL){
#ifdef DEBUG_MODE
		printf("tmp2 is not NULL\n");
#endif
		multiNCopy(y2, size2, tmp2, size2);
		divisor = tmp2;
#ifdef DEBUG_MODE
		multiNPrintBinary(y2, size1);
		multiNPrintBinary(tmp2, size2);
#endif
	}

	// Blank
	multiNBlank(quotient, size3);
	multiNBlank(remainder, size4);
	
	// Total size
	uint64_t y1_t = size1 * F32_SIZE;
	uint64_t y2_t = size2 * F32_SIZE;
	uint64_t  a_t = size3 * F32_SIZE;

	// Effective size (Where is the highest 1) 
	uint64_t y1_m = multiNAbsSize(dividend, size1);
	uint64_t y2_m = multiNAbsSize(divisor, size2);
	
	// Shift by
	int64_t shift1 = y1_t - y1_m - 1 - 1;
	int64_t shift2 = y2_t - y2_m - 1 - 1;

#ifdef DEBUG_MODE
	printf("shift1: %lu\n", shift1);
	printf("shift2: %lu\n", shift2);
#endif

	multiNLeftShift(dividend, size1, shift1);
	multiNLeftShift(divisor, size2, shift2);

#ifdef DEBUG_MODE
	printf("After shift\n");
	multiNPrintBinary(dividend, size1);
	multiNPrintBinary(divisor, size2);
#endif

	int8_t cmp = 0;

	for(uint64_t i = 0; i < a_t; i++){
		if(multiNIsZero(dividend, size1)){
			break;
		}

		cmp = multiNCompare(dividend, size1, divisor, size2);
#ifdef DEBUG_EXTRA_MODE
		printf("%li\t%i\n", i, cmp);
#endif
		if(cmp >= 0){
			multiNSetBit(quotient, size3, a_t - 1 - i, 1);
			multiNBlank(remainder, size4);
			multiNSub(dividend, size1, divisor, size2, remainder, size4);
			multiNCopy(remainder, size4, dividend, size1);
		}
#ifdef DEBUG_EXTRA_MODE
		multiNPrintBinary(y1, size1);
		multiNPrintBinary(y2, size2);
		multiNPrintBinary( b, size4);
#endif

		multiNLeftShift(dividend, size1, 1);
	}
#ifdef DEBUG_EXTRA_MODE
	printf("s1: %li\ts2: %li\ts: %li\n", shift1, shift2, shift1 - shift2);
#endif

#ifdef DEBUG_MODE
	multiNPrintBinary( a, size3);
#endif

	int64_t resultShift = F64_SIZE + shift1 - shift2 - 1;
	multiNRightShift(quotient, size3, resultShift);

#ifdef DEBUG_MODE
	multiNPrintBinary( a, size3);
#endif
}
