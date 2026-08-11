#include <stdlib.h>
#include <stdbool.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "matrix.h"

LAErrorCode LAMatFill(LAMat_t *m, double value){
	if(!(LAMatIsValid(m)))	goto matrix_error;

	size_t w = LAMatGetWidth(m);
	if(w == LA_MAT_INDEX_FAIL) goto matrix_error;
	size_t h = LAMatGetHeight(m);
	if(h == LA_MAT_INDEX_FAIL) goto matrix_error;

	LAErrorCode code = LA_NO_ERROR;

	for(size_t y = 0; y < h; y++){
		for(size_t x = 0; x < w; x++){
			code = LAMatSet(m, y, x, value);
			if(code) return code;
		}
	}

	return LA_NO_ERROR;

matrix_error:
	LA_RAISE_ERROR(LA_ERROR_MATRIX);
	return LA_ERROR_MATRIX;
}

LAErrorCode LAMatZeros(LAMat_t *m){
	return LAMatFill(m, 0.0);
}

LAErrorCode LAMatOnes(LAMat_t *m){
	return LAMatFill(m, 1.0);
}

LAErrorCode LAMatRandi(LAMat_t *m, double vmin, double vmax){
	if(!(LAMatIsValid(m)))	goto matrix_error;

	size_t w = LAMatGetWidth(m);
	if(w == LA_MAT_INDEX_FAIL) goto matrix_error;
	size_t h = LAMatGetHeight(m);
	if(h == LA_MAT_INDEX_FAIL) goto matrix_error;

	LAErrorCode code = LA_NO_ERROR;
	double value = 0.0;

	for(size_t y = 0; y < h; y++){
		for(size_t x = 0; x < w; x++){
			value = vmin + (vmax - vmin) * (rand() / (double) RAND_MAX);
			code = LAMatSet(m, y, x, value);
			if(code) return code;
		}
	}

	return LA_NO_ERROR;

matrix_error:
	LA_RAISE_ERROR(LA_ERROR_MATRIX);
	return LA_ERROR_MATRIX;
}

LAErrorCode LAMatRand(LAMat_t *m){
	return LAMatRandi(m, 0.0, 1.0);
}

LAErrorCode LAMatEye(LAMat_t *m){
	if(!(LAMatIsValid(m)))	goto matrix_error;
	if(!(LAMatIsSquare(m)))	goto dim_error;

	size_t w = LAMatGetWidth(m);
	if(w == LA_MAT_INDEX_FAIL) goto matrix_error;

	LAErrorCode code = LA_NO_ERROR;

	for(size_t y = 0; y < w; y++){
		for(size_t x = 0; x < w; x++){
			code = LAMatSet(m, y, x, (x == y) ? 1.0 : 0.0);
			if(code) return code;
		}
	}

	return LA_NO_ERROR;

dim_error:
	LA_RAISE_ERROR(LA_ERROR_NONMATCHING_DIMENSION);
	return LA_ERROR_NONMATCHING_DIMENSION;

matrix_error:
	LA_RAISE_ERROR(LA_ERROR_MATRIX);
	return LA_ERROR_MATRIX;
}
