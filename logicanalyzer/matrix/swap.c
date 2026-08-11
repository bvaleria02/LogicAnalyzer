#include <stdlib.h>
#include <stdbool.h>
#include "../liblogicanalyzer.h"
#include "matrix.h"
#include "../error.h"
#include "../utils.h"

LAErrorCode LAMatRowSwap(LAMat_t *m, size_t r1, size_t r2){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m))) return LA_ERROR_MATRIX;

	size_t w = LAMatGetWidth(m);
	if(w == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	size_t h = LAMatGetHeight(m);
	if(h == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	LAErrorCode code = LA_NO_ERROR;
	double v1 = 0.0;
	double v2 = 0.0;

	if(r1 >= h) return LA_ERROR_NONMATCHING_DIMENSION;
	if(r2 >= h) return LA_ERROR_NONMATCHING_DIMENSION;

	for(size_t x = 0; x < w; x++){
		code = LAMatGet(m, r1, x, &v1);
		if(code) return code;
		code = LAMatGet(m, r2, x, &v2);
		if(code) return code;

		code = LAMatSet(m, r1, x, v2);
		if(code) return code;
		code = LAMatSet(m, r2, x, v1);
		if(code) return code;
	}

	return LA_NO_ERROR;
}

LAErrorCode LAMatColSwap(LAMat_t *m, size_t c1, size_t c2){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m))) return LA_ERROR_MATRIX;

	size_t w = LAMatGetWidth(m);
	if(w == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	size_t h = LAMatGetHeight(m);
	if(h == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	LAErrorCode code = LA_NO_ERROR;
	double v1 = 0.0;
	double v2 = 0.0;

	if(c1 >= w) return LA_ERROR_NONMATCHING_DIMENSION;
	if(c2 >= w) return LA_ERROR_NONMATCHING_DIMENSION;

	for(size_t y = 0; y < h; y++){
		code = LAMatGet(m, y, c1, &v1);
		if(code) return code;
		code = LAMatGet(m, y, c2, &v2);
		if(code) return code;

		code = LAMatSet(m, y, c1, v2);
		if(code) return code;
		code = LAMatSet(m, y, c2, v1);
		if(code) return code;
	}

	return LA_NO_ERROR;
}

LAErrorCode LAMatValueSwap(LAMat_t *m, size_t r1, size_t c1, size_t r2, size_t c2){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m))) return LA_ERROR_MATRIX;

	size_t w = LAMatGetWidth(m);
	if(w == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	size_t h = LAMatGetHeight(m);
	if(h == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	LAErrorCode code = LA_NO_ERROR;
	double v1 = 0.0;
	double v2 = 0.0;

	if(r1 >= h) return LA_ERROR_NONMATCHING_DIMENSION;
	if(r2 >= h) return LA_ERROR_NONMATCHING_DIMENSION;
	if(c1 >= w) return LA_ERROR_NONMATCHING_DIMENSION;
	if(c2 >= w) return LA_ERROR_NONMATCHING_DIMENSION;

	code = LAMatGet(m, r1, c1, &v1);
	if(code) return code;
	code = LAMatGet(m, r2, c2, &v2);
	if(code) return code;

	code = LAMatSet(m, r1, c1, v2);
	if(code) return code;
	code = LAMatSet(m, r2, c2, v1);
	if(code) return code;

	return LA_NO_ERROR;
}
