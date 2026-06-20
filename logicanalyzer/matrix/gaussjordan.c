#include "../liblogicanalyzer.h"
#include "matrix.h"
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

LAErrorCode LAMatTriU(LAMat_t *a, LAMat_t *p){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(a))) 						return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(p)) && p != NULL) 		return LA_ERROR_MATRIX;

	if(!(LAMatIsSquare(a))) 					return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatIsSquare(p)) && p != NULL) 		return LA_ERROR_NONMATCHING_DIMENSION;

	if(!(LAMatAreSameDim(a, p)) && p != NULL) 	return LA_ERROR_NONMATCHING_DIMENSION;

	size_t w = LAMatGetWidth(a);
	if(w == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	size_t h = LAMatGetHeight(a);
	if(h == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	LAErrorCode code = LA_NO_ERROR;

	if(p != NULL){
		code = LAMatEye(p);
		if(code) return code;
	}

	size_t row = LA_MAT_INDEX_FAIL;
	double v1 = 0.0;
	double v2 = 0.0;

	for(size_t x = 0; x < w; x++){
		row = LA_MAT_INDEX_FAIL;

		code = LAMatGetHighestRowFrom(a, x, x, &row, true);
		if(code) return code;
		if(row == LA_MAT_INDEX_FAIL) return LA_ERROR_NONINVERTIBLE_MATRIX;
		
		code= LAMatRowSwap(a, x, row);
		if(code) return code;
		if(p != NULL){
			code= LAMatRowSwap(p, x, row);
			if(code) return code;
		}

		code = LAMatGet(a, x, x, &v1);
		if(code) return code;
		
		for(size_t y = x; y < w; y++){
			if(y == x) continue;

			code = LAMatGet(a, y, x, &v2);
			if(code) return code;

			code = LAMatSubRow(a, y, x, v2 / v1);
			if(code) return code;
			if(p != NULL){
				code = LAMatSubRow(p, y, x, v2 / v1);
				if(code) return code;
			}
		}
	}

	return LA_NO_ERROR;
}

LAErrorCode LAMatInverseGaussJordan(LAMat_t *a, LAMat_t *p){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(p, LA_PROPAGATE_ERROR);
	
	if(!(LAMatIsValid(a))) 			return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(p))) 			return LA_ERROR_MATRIX;

	if(!(LAMatIsSquare(a))) 		return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatIsSquare(p))) 		return LA_ERROR_NONMATCHING_DIMENSION;

	if(!(LAMatAreSameDim(a, p))) 	return LA_ERROR_NONMATCHING_DIMENSION;

	size_t w = LAMatGetWidth(a);
	if(w == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	size_t h = LAMatGetHeight(a);
	if(h == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	LAErrorCode code = LA_NO_ERROR;

	code = LAMatEye(p);
	if(code) return code;

	size_t row = LA_MAT_INDEX_FAIL;
	double v1 = 0.0;
	double v2 = 0.0;
	double eps = 1e-10;

	for(size_t n = 0; n < w; n++){
		// Gets nxn value
		code = LAMatGet(a, n, n, &v1);
		if(code) return code;

		// Pivot if diagonal value is 0.0
		if(fabs(v1) <= eps){
			// true means "get the highest absolute value, else, if all zero, INDEX_FAIL"
			code = LAMatGetHighestRowFrom(a, n, n, &row, true);
			if(code) return code;
			if(row == LA_MAT_INDEX_FAIL) return LA_ERROR_NONINVERTIBLE_MATRIX;

			//Swap rows both in A and P matrixes;
			code = LAMatRowSwap(a, n, row);
			if(code) return code;
			code = LAMatRowSwap(p, n, row);
			if(code) return code;

			// Now "value" holds the _value_ of the nxn cell
			code = LAMatGet(a, n, n, &v1);
			if(code) return code;
		}

		for(size_t k = n; k < h; k++){
			// Skip the first row and start below the diagonal
			if(n == k) continue;

			// Gets the kxn value from A (first column)
			code = LAMatGet(a, k, n, &v2);
			if(code) return code;

			// v2 holds the value scaling factor to make k-row 0
			// Divide pivot value by itself (to make it 1) and then multiply for the first column value
			v2 = v2 / v1;

			// mat, r1, r2, f
			// r1 = r1 - r2*f
			code = LAMatSubRow(a, k, n, v2);
			if(code) return code;
			code = LAMatSubRow(p, k, n, v2);
			if(code) return code;
		}
	}

	size_t x = 0;
	size_t y = 0;

	for(size_t n = 0; n < w; n++){
		x = w - n - 1;

		// Gets nxn value
		code = LAMatGet(a, x, x, &v1);
		if(code) return code;

		code = LAMatMulRow(a, x, 1 / v1);
		if(code) return code;
		code = LAMatMulRow(p, x, 1 / v1);
		if(code) return code;

		for(size_t k = n; k < (h-1); k++){
			y = h - k - 2;

			// Gets the x,y value from A (last column)
			code = LAMatGet(a, y, x, &v2);
			if(code) return code;

			// mat, r1, r2, f
			// r1 = r1 - r2*f
			code = LAMatSubRow(a, y, x, v2);
			if(code) return code;
			code = LAMatSubRow(p, y, x, v2);
			if(code) return code;
		}
	}

	return LA_NO_ERROR;
}
