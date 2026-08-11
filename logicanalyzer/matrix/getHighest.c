#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "matrix.h"

LAErrorCode LAMatGetHighestRow(LAMat_t *m, size_t c, size_t *r, bool useFabs){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(r, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m))) return LA_ERROR_MATRIX;

	size_t w = LAMatGetWidth(m);
	if(w == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	size_t h = LAMatGetHeight(m);
	if(h == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	if(c >= w) return LA_ERROR_NONMATCHING_DIMENSION;
	
	LAErrorCode code = LA_NO_ERROR;
	double value = 0.0;
	double maxValue = 0.0;
	size_t index = (useFabs) ? LA_MAT_INDEX_FAIL : 0;
	if(!useFabs){
		code = LAMatGet(m, 0, c, &maxValue);
		if(code) return code;
	}
	
	for(size_t y = 0; y < h; y++){
		code = LAMatGet(m, y, c, &value);
		if(code) return code;

		if(useFabs) value = fabs(value);

		if(value > maxValue){
			maxValue = value;
			index = y;
		}
	}

	(*r) = (maxValue == 0.0 && !(useFabs)) ? LA_MAT_INDEX_FAIL : index;

	return LA_NO_ERROR;
}

LAErrorCode LAMatGetHighestCol(LAMat_t *m, size_t r, size_t *c, bool useFabs){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m))) return LA_ERROR_MATRIX;

	size_t w = LAMatGetWidth(m);
	if(w == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	size_t h = LAMatGetHeight(m);
	if(h == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	if(r >= h) return LA_ERROR_NONMATCHING_DIMENSION;
	
	LAErrorCode code = LA_NO_ERROR;
	double value = 0.0;
	double maxValue = 0.0;
	size_t index = (useFabs) ? LA_MAT_INDEX_FAIL : 0;
	if(!useFabs){
		code = LAMatGet(m, r, 0, &maxValue);
		if(code) return code;
	}
	
	for(size_t x = 0; x < w; x++){
		code = LAMatGet(m, r, x, &value);
		if(code) return code;

		if(useFabs) value = fabs(value);

		if(value > maxValue){
			maxValue = value;
			index = x;
		}
	}

	(*c) = (maxValue == 0.0 && !(useFabs)) ? LA_MAT_INDEX_FAIL : index;

	return LA_NO_ERROR;
}

LAErrorCode LAMatGetHighestRowFrom(LAMat_t *m, size_t c, size_t r0, size_t *r, bool useFabs){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(r, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m))) return LA_ERROR_MATRIX;

	size_t w = LAMatGetWidth(m);
	if(w == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	size_t h = LAMatGetHeight(m);
	if(h == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	if(c  >= w) return LA_ERROR_NONMATCHING_DIMENSION;
	if(r0 >= w) return LA_ERROR_NONMATCHING_DIMENSION;
	
	LAErrorCode code = LA_NO_ERROR;
	double value = 0.0;
	double maxValue = 0.0;
	size_t index = (useFabs) ? LA_MAT_INDEX_FAIL : r0;
	if(!useFabs){
		code = LAMatGet(m, r0, c, &maxValue);
		if(code) return code;
	}
	
	for(size_t y = r0; y < h; y++){
		code = LAMatGet(m, y, c, &value);
		if(code) return code;

		if(useFabs) value = fabs(value);

		if(value > maxValue){
			maxValue = value;
			index = y;
		}
	}

	(*r) = (maxValue == 0.0 && !(useFabs)) ? LA_MAT_INDEX_FAIL : index;

	return LA_NO_ERROR;
}

LAErrorCode LAMatGetHighestColFrom(LAMat_t *m, size_t r, size_t c0, size_t *c, bool useFabs){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m))) return LA_ERROR_MATRIX;

	size_t w = LAMatGetWidth(m);
	if(w == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	size_t h = LAMatGetHeight(m);
	if(h == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	if(r  >= h) return LA_ERROR_NONMATCHING_DIMENSION;
	if(c0 >= h) return LA_ERROR_NONMATCHING_DIMENSION;
	
	LAErrorCode code = LA_NO_ERROR;
	double value = 0.0;
	double maxValue = 0.0;
	size_t index = (useFabs) ? LA_MAT_INDEX_FAIL : c0;
	if(!useFabs){
		code = LAMatGet(m, r, c0, &maxValue);
		if(code) return code;
	}
	
	for(size_t x = c0; x < w; x++){
		code = LAMatGet(m, r, x, &value);
		if(code) return code;

		if(useFabs) value = fabs(value);

		if(value > maxValue){
			maxValue = value;
			index = x;
		}
	}

	(*c) = (maxValue == 0.0 && !(useFabs)) ? LA_MAT_INDEX_FAIL : index;
	return LA_NO_ERROR;
}
