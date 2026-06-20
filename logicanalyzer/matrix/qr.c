#include "../liblogicanalyzer.h"
#include "matrix.h"
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

LAErrorCode LAMatQR(LAMat_t *a, LAMat_t *q, LAMat_t *r){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(q, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(r, LA_PROPAGATE_ERROR);

	return LA_NO_ERROR;
}

LAErrorCode LAMatHMatrix(LAMat_t *hMat, LAMat_t *uVec){
	LA_HANDLE_NULLPTR(hMat, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(uVec, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(hMat))) 					return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(uVec))) 					return LA_ERROR_MATRIX;
	
	LAErrorCode code = LA_NO_ERROR;

	if(!(LAMatIsSquare(hMat))) 					return LA_ERROR_NONMATCHING_DIMENSION;

	size_t hw,hh;
	code = LAMatShape(hMat, &hh, &hw);
	if(code) return code;

	size_t uw,uh;
	code = LAMatShape(uVec, &uh, &uw);
	if(code) return code;

	if(uw !=  1)	return LA_ERROR_NONMATCHING_DIMENSION;
	if(uh != hh)	return LA_ERROR_NONMATCHING_DIMENSION;
	if(hw != hh)	return LA_ERROR_NONMATCHING_DIMENSION;

	code = LAMatEye(hMat);
	if(code) return code;

	double hValue 	= 0.0;
	double uValueA 	= 0.0;
	double uValueB 	= 0.0;

	for(size_t y = 0; y < hh; y++){

		code = LAMatGet(uVec, y, 0, &uValueA);
		if(code) return code;

		for(size_t x = 0; x < hw; x++){

			code = LAMatGet(uVec, x, 0, &uValueB);
			if(code) return code;
		
			code = LAMatGet(hMat, y, x, &hValue);
			if(code) return code;

			hValue = hValue - 2*(uValueA * uValueB);

			code = LAMatSet(hMat, y, x, hValue);
			if(code) return code;

		}
	}

	return LA_NO_ERROR;
}

LAErrorCode LAMatHMatrixAndNormalize(LAMat_t *hMat, LAMat_t *uVec){
	LA_HANDLE_NULLPTR(hMat, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(uVec, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	double norm = 0.0;
	code = LAMatFrobeniusNormCol(uVec, 0, &norm);
	if(code) return code;
	// Placeholder error code
	if(fabs(norm) < LA_EPS) return LA_ERROR_INCORRECTVALUE;

	code = LAMatMulCol(uVec, 0, 1 / norm);
	if(code) return code;

	return LAMatHMatrix(hMat, uVec);
}

LAErrorCode LAMatUVector(LAMat_t *a, LAMat_t *uVec, size_t col){
	LA_HANDLE_NULLPTR(a, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(uVec, LA_PROPAGATE_ERROR);
	
	if(!(LAMatIsValid(a))) 						return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(uVec))) 					return LA_ERROR_MATRIX;

	LAErrorCode code = LA_NO_ERROR;

	size_t aw,ah;
	code = LAMatShape(a, &ah, &aw);
	if(code) return code;

	size_t uw,uh;
	code = LAMatShape(uVec, &uh, &uw);
	if(code) return code;

	if(uw  !=  1)	return LA_ERROR_NONMATCHING_DIMENSION;
	if(uh  != ah)	return LA_ERROR_NONMATCHING_DIMENSION;
	if(col >= aw)	return LA_ERROR_NONMATCHING_DIMENSION;

	double aValue = 0.0;
	double aNorm  = 0.0;
	code = LAMatFrobeniusNormCol(a, col, &aNorm);
	if(code) return code;

	// Computes  u' = a_col - ||a_col|| * e_col
	// e_0 = [1 0 ... 0]T
	// e_1 = [0 1 ... 0]T
	for(size_t y = 0; y < ah; y++){
		code = LAMatGet(a, y, col, &aValue);
		if(code) return code;

		if(y == col){
			// a_col - 1 * ||a_col||
			aValue = aValue - aNorm;
		} else {
			// a_col - 0 * ||a_col||
		}

		code = LAMatSet(uVec, y, 0, aValue);
		if(code) return code;
	}

	double uNorm = 0.0;
	code = LAMatFrobeniusNormCol(uVec, 0, &uNorm);
	if(code) return code;
	// Placeholder error code
	if(fabs(uNorm) < LA_EPS) return LA_ERROR_INCORRECTVALUE;

	code = LAMatMulCol(uVec, 0, 1 / uNorm);
	if(code) return code;

	return LA_NO_ERROR;
}
