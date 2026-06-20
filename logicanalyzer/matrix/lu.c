#include "../liblogicanalyzer.h"
#include "matrix.h"
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

LAErrorCode LAMatLU(LAMat_t *a, LAMat_t *l, LAMat_t *u, LAMat_t *p){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(l, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(u, LA_PROPAGATE_ERROR);
	// p is Optional

	if(!(LAMatIsValid(a))) 					return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(l))) 					return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(u))) 					return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(p)) && (p != NULL)) 	return LA_ERROR_MATRIX;

	if(!(LAMatIsSquare(a))) 				return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatIsSquare(l))) 				return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatIsSquare(u))) 				return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatIsSquare(p)) && (p != NULL)) 	return LA_ERROR_NONMATCHING_DIMENSION;

	if(!(LAMatAreSameDim(a, l))) 				return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatAreSameDim(a, u))) 				return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatAreSameDim(a, p)) && (p != NULL)) return LA_ERROR_NONMATCHING_DIMENSION;

	size_t w = LAMatGetWidth(a);
	if(w == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	size_t h = LAMatGetHeight(a);
	if(h == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	
	LAErrorCode code = LA_NO_ERROR;

	code = LAMatEye(l);
	if(code) return code;
	code = LAMatCopy(a, u);
	if(code) return code;
	if(p != NULL){
		code = LAMatEye(p);
		if(code) return code;
	}

	double value = 0;
	size_t index = 0;
	double v1 = 0;
	double v2 = 0;
	double eps = 1e-10;

	for(size_t n = 0; n < w; n++){
		// Gets nxn value
		code = LAMatGet(u, n, n, &value);
		if(code) return code;

		// Pivot if diagonal value is 0.0
		if(fabs(value) <= eps){
			// true means "get the highest absolute value, else, if all zero, INDEX_FAIL"
			code = LAMatGetHighestRowFrom(u, n, n, &index, true);
			if(code) return code;
			if(index == LA_MAT_INDEX_FAIL) return LA_ERROR_NONINVERTIBLE_MATRIX;

			//Swap rows both U, L and P matrixes;
			code = LAMatRowSwap(u, n, index);
			if(code) return code;
			if(p == NULL){
				code = LAMatRowSwap(l, n, index);
				if(code) return code;
			} else {
				code = LAMatRowSwap(p, n, index);
				if(code) return code;
			}

			// Now "value" holds the _value_ of the nxn cell
			code = LAMatGet(u, n, n, &value);
			if(code) return code;
		}

		for(size_t k = n; k < h; k++){
			// Skip the first row and start below the diagonal
			if(n == k) continue;

			// Gets the kxn value from U (first column)
			code = LAMatGet(u, k, n, &v1);
			if(code) return code;

			// v2 holds the value scaling factor to make k-row 0
			// Divide pivot value by itself (to make it 1) and then multiply for the first column value
			v2 = v1 / value;

			// Stores into L the value of v2 (scaling factor)
			code = LAMatSet(l, k, n, v2);
			if(code) return code;

			// mat, r1, r2, f
			// r1 = r1 - r2*f
			code = LAMatSubRow(u, k, n, v2);
			if(code) return code;
		}
		
	}

	return LA_NO_ERROR;
}

LAErrorCode LAMatInverseFromLU(LAMat_t *l, LAMat_t *u, LAMat_t *z, LAMat_t *m){
	LA_HANDLE_NULLPTR(l, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(u, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(z, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(l))) 					return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(u))) 					return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(z))) 					return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(m))) 					return LA_ERROR_MATRIX;

	if(!(LAMatIsSquare(l))) 				return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatIsSquare(u))) 				return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatIsSquare(z))) 				return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatIsSquare(m))) 				return LA_ERROR_NONMATCHING_DIMENSION;

	if(!(LAMatAreSameDim(l, u))) 			return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatAreSameDim(l, z))) 			return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatAreSameDim(l, m))) 			return LA_ERROR_NONMATCHING_DIMENSION;

	size_t w = LAMatGetWidth(l);
	if(w == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	size_t h = LAMatGetHeight(l);
	if(h == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	LAErrorCode code = LA_NO_ERROR;

	// AM = I (M is the inverse)
	// why M? Originally, the formula uses H, but h is used for the Height of the matrix

	double l_yk = 0.0;
	double z_yx = 0.0;
	double z_kx = 0.0;
	double acc  = 0.0;
	double l_yy = 0.0;

	// Create Z matrix
	// LUM = I; 	Z = UM
	// LZ  = I
	for(size_t x = 0; x < w; x++){
		for(size_t y = 0; y < h; y++){

			acc = 0.0;

			for(size_t k = 0; k < y; k++){
				code = LAMatGet(l, y, k, &l_yk);
				if(code) return code;
				code = LAMatGet(z, k, x, &z_kx);
				if(code) return code;
				acc += l_yk * z_kx;
			}

			code = LAMatGet(l, y, y, &l_yy);
			if(code) return code;
			
			if(x > y){
				z_yx = 0.0;
			} else if (x == y){
				z_yx = (1 - acc) / l_yy;
			} else {
				z_yx = -acc / l_yy;
			}

			code = LAMatSet(z, y, x, z_yx);
			if(code) return code;
		}
	}

	double u_yy = 0.0;
	double u_yj = 0.0;
	double m_kx = 0.0;
	double m_yx = 0.0;
	// Create M matrix
	// UM = Z
	for(size_t n = 0; n < w; n++){
		// Invert index
		size_t x = w - n - 1;
		
		for(size_t k = 0; k < h; k++){
			// Invert inde
			size_t y = h - k - 1;

			code = LAMatGet(u, y, y, &u_yy);
			if(code) return code;
			code = LAMatGet(z, y, x, &z_yx);
			if(code) return code;
			acc = 0.0;
			
			for(size_t i = 0; i < k; i++){
				// Invert index
				size_t j = h - i - 1;

				code = LAMatGet(u, y, j, &u_yj);
				if(code) return code;
				code = LAMatGet(m, j, x, &m_kx);
				if(code) return code;
				acc += u_yj * m_kx;
			}

			if(k == 0){
				m_yx = z_yx / u_yy;	
			} else {
				m_yx = (z_yx - acc) / u_yy;
			}

			code = LAMatSet(m, y, x, m_yx);
			if(code) return code;
		}
	}
	
	return LA_NO_ERROR;
}

LAErrorCode LAMatInverseLU(LAMat_t *a, LAMat_t *l, LAMat_t *u, LAMat_t *p, LAMat_t *z, LAMat_t *m){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(l, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(u, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(p, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(z, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(a))) 					return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(l))) 					return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(u))) 					return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(p))) 					return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(z))) 					return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(m))) 					return LA_ERROR_MATRIX;

	if(!(LAMatIsSquare(a))) 				return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatIsSquare(l))) 				return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatIsSquare(u))) 				return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatIsSquare(p))) 				return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatIsSquare(z))) 				return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatIsSquare(m))) 				return LA_ERROR_NONMATCHING_DIMENSION;

	if(!(LAMatAreSameDim(a, l))) 			return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatAreSameDim(a, u))) 			return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatAreSameDim(a, p))) 			return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatAreSameDim(a, z))) 			return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatAreSameDim(a, m))) 			return LA_ERROR_NONMATCHING_DIMENSION;

	LAErrorCode code = LA_NO_ERROR;

	// LU decomposition, permutate P to make L lower triangle
	// A = LU
	code = LAMatLU(a, l, u, p);
	if(code) return code;

	// Compute the inverse (M)
	code = LAMatInverseFromLU(l, u, z, m);
	if(code) return code;

	return LA_NO_ERROR;
}

