#include <stdlib.h>
#include <stdbool.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "matrix.h"

LAErrorCode LAMatMul_backend(LAMat_t *m1, LAMat_t *m2, LAMat_t *m3, bool overwriteM3, double factor){
	if(!(LAMatIsValid(m1)))	goto matrix_error;
	if(!(LAMatIsValid(m2)))	goto matrix_error;
	if(!(LAMatIsValid(m3)))	goto matrix_error;

	if(!(LAMatAreMulDim(m1, m2, m3)))	goto dim_error;

	size_t w1 = LAMatGetWidth(m1);
	size_t w3 = LAMatGetWidth(m3);
	size_t h3 = LAMatGetHeight(m3);

	double v1 = 0.0;
	double v2 = 0.0;
	double vr = 0.0;
	LAErrorCode code = LA_NO_ERROR;

	for(size_t y = 0; y < h3; y++){
		for(size_t x = 0; x < w3; x++){

			if(overwriteM3){
				vr = 0.0;
			} else {
				code = LAMatGet(m3, y, x, &vr);
				if(code) return code;
			}

			for(size_t z = 0; z < w1; z++){
				code = LAMatGet(m1, y, z, &v1);
				if(code) return code;
				code = LAMatGet(m2, z, x, &v2);
				if(code) return code;

				vr += factor * (v1 * v2);
			}

			code = LAMatSet(m3, y, x, vr);
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

LAErrorCode LAMatMul(LAMat_t *m1, LAMat_t *m2, LAMat_t *m3){
	// Standard Multiplication
	// m3 = m1 * m2
	return LAMatMul_backend(m1, m2, m3, true, 1.0);
}

LAErrorCode LAMatMulCum(LAMat_t *m1, LAMat_t *m2, LAMat_t *m3){
	// Cumulative Multiplication
	// m3 = m3 + (m1 * m2)
	return LAMatMul_backend(m1, m2, m3, false, 1.0);
}

LAErrorCode LAMatMulCumFactor(LAMat_t *m1, LAMat_t *m2, LAMat_t *m3, double factor){
	// Cumulative Multiplication
	// m3 = m3 + factor * (m1 * m2)
	return LAMatMul_backend(m1, m2, m3, false, factor);
}

LAErrorCode LAMatMulCol(LAMat_t *m, size_t c, double f){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m))) return LA_ERROR_MATRIX;

	size_t w = LAMatGetWidth(m);
	if(w == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	size_t h = LAMatGetHeight(m);
	if(h == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	LAErrorCode code = LA_NO_ERROR;
	double v = 0.0;

	if(c >= w) return LA_ERROR_NONMATCHING_DIMENSION;

	for(size_t y = 0; y < h; y++){
		code = LAMatGet(m, y, c, &v);
		if(code) return code;
		
		v = v * f;

		code = LAMatSet(m, y, c, v);
		if(code) return code;
	}

	return LA_NO_ERROR;
}

LAErrorCode LAMatMulRow(LAMat_t *m, size_t r, double f){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m))) return LA_ERROR_MATRIX;

	size_t w = LAMatGetWidth(m);
	if(w == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	size_t h = LAMatGetHeight(m);
	if(h == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	LAErrorCode code = LA_NO_ERROR;
	double v = 0.0;

	if(r >= h) return LA_ERROR_NONMATCHING_DIMENSION;

	/*
		r1 = r1 + r2 * f
	*/

	for(size_t x = 0; x < w; x++){
		code = LAMatGet(m, r, x, &v);
		if(code) return code;
		
		v = v * f;

		code = LAMatSet(m, r, x, v);
		if(code) return code;
	}

	return LA_NO_ERROR;
}
