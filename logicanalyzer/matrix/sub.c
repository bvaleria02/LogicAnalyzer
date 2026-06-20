#include "../liblogicanalyzer.h"
#include "matrix.h"
#include <stdlib.h>
#include <stdbool.h>

LAErrorCode LAMatSub(LAMat_t *m1, LAMat_t *m2, LAMat_t *re){
	if(!(LAMatIsValid(m1)))	goto matrix_error;
	if(!(LAMatIsValid(m2)))	goto matrix_error;
	if(!(LAMatIsValid(re)))	goto matrix_error;

	if(!(LAMatAreSameDim(m1, m2)))	goto dim_error;
	if(!(LAMatAreSameDim(m1, re)))	goto dim_error;

	size_t width  = LAMatGetWidth(m1);
	size_t height = LAMatGetHeight(m1);

	double v1 = 0.0;
	double v2 = 0.0;
	double vr = 0.0;
	LAErrorCode code = LA_NO_ERROR;

	for(size_t y = 0; y < height; y++){
		for(size_t x = 0; x < width; x++){
			code = LAMatGet(m1, y, x, &v1);
			if(code) return code;
			code = LAMatGet(m2, y, x, &v2);
			if(code) return code;

			vr = v1 - v2;

			code = LAMatSet(re, y, x, vr);
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

LAErrorCode LAMatSubCol(LAMat_t *m, size_t c1, size_t c2, double f){
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

	/*
		c1 = c1 - c2 * f
	*/

	for(size_t y = 0; y < h; y++){
		code = LAMatGet(m, y, c1, &v1);
		if(code) return code;
		code = LAMatGet(m, y, c2, &v2);
		if(code) return code;
		
		v1 = v1 - v2 * f;

		code = LAMatSet(m, y, c1, v1);
		if(code) return code;
	}

	return LA_NO_ERROR;
}

LAErrorCode LAMatSubRow(LAMat_t *m, size_t r1, size_t r2, double f){
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

	/*
		r1 = r1 - r2 * f
	*/

	for(size_t x = 0; x < w; x++){
		code = LAMatGet(m, r1, x, &v1);
		if(code) return code;
		code = LAMatGet(m, r2, x, &v2);
		if(code) return code;
		
		v1 = v1 - v2 * f;

		code = LAMatSet(m, r1, x, v1);
		if(code) return code;
	}

	return LA_NO_ERROR;
}
