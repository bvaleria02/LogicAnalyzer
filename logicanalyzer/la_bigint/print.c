#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "laBigInt.h"

LAErrorCode LABigIntPrintHex(LABigInt_t *m, const char *label){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(m)))		return LA_ERROR_BIG_INT;

	if(label != NULL) printf("%s : ", label);
	printf("0x");

	for(size_t i = 0; i < m->limbCount; i++){
		size_t index = m->limbCount - 1 - i;
		printf("%08X ", m->limbs[index]);
	}

	printf("\n");

	return LA_NO_ERROR;
}

LAErrorCode LABigIntPrintBin(LABigInt_t *m, const char *label){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(m)))		return LA_ERROR_BIG_INT;

	if(label != NULL) printf("%s : ", label);
	printf("0b");

	for(size_t i = 0; i < m->limbCount; i++){
		size_t index = m->limbCount - 1 - i;
		LABigIntLimb_t limb = m->limbs[index];
		for(size_t j = 0; j < LA_BIG_INT_BIT_SIZE; j++){
			printf("%01X", (limb >> (LA_BIG_INT_BIT_SIZE - 1 - j)) & 0x1);
		}
		printf(" ");
	}

	printf("\n");

	return LA_NO_ERROR;
}

LAErrorCode LABigIntPrintDec(LABigInt_t *m, const char *label){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LABigIntIsValid(m)))		return LA_ERROR_BIG_INT;

	LAErrorCode code = LA_NO_ERROR;

	char *charBuffer = NULL;
	LABigInt_t b = {0}, c = {0}, d= {0};
	code = LABigIntCreateFromOther(&b, m, 0);
	if(code) goto cleanup;
	code = LABigIntCreateFromUInt(&c, 10);
	if(code) goto cleanup;
	code = LABigIntCreate(&d, m->limbCount);
	if(code) goto cleanup;

	if(label != NULL) printf("%s : ", label);
	if(m->sign == LA_BIG_INT_NEGATIVE) printf("-");

	size_t bufferSize = b.limbCount * 10 + 1;
	charBuffer = (char *)malloc(bufferSize * sizeof(char));
	if(charBuffer == NULL) return LA_ERROR_MALLOC;
	for(size_t i = 0; i < bufferSize; i++) charBuffer[i] = '\0';
	
	uint32_t rem = 0;
	size_t pos = 0;
	for(size_t i = 0; i < (bufferSize - 1); i++){
		size_t k = bufferSize - 1 - 1 - i;

		code = LABigIntDiv_non_restoring_backend(&b, &c, &d);
		if(code) goto cleanup;
		rem = d.limbs[0];

		charBuffer[k] = (rem % 10) + '0';

		if(LABigIntIsZero(&b)) {
			pos = k;
			break;
		}
	} 

	printf("%s\n", &(charBuffer[pos]));
	goto cleanup;

cleanup:
	if(code) printf("Error (%u)\n", code);
	LABigIntDestroy(&b);
	LABigIntDestroy(&c);
	LABigIntDestroy(&d);
	if(charBuffer != NULL) free(charBuffer);
	return code;
}
