#include "../liblogicanalyzer.h"
#include "matrix.h"
#include <stdlib.h>
#include <stdbool.h>


size_t LAMatGetIndex(LAMat_t *mat, size_t row, size_t col){
	if(mat == NULL){
		return LA_MAT_INDEX_FAIL;
	}

	if(!(LAMatIsValid(mat))){
		return LA_MAT_INDEX_FAIL;
	}

	size_t index = 0;

	size_t rowOffset = (LAMatIsView(mat)) ? mat->viewData.rowOffset : 0;
	size_t colOffset = (LAMatIsView(mat)) ? mat->viewData.colOffset : 0;

	if(LAMatIsTransposed(mat)){
		index = ((mat->row) * (col + rowOffset)) + (colOffset + row);
	} else {
		index = ((mat->col) * (row + rowOffset)) + (colOffset + col);
	}

	//printf("r: %lu\tc: %lu\ti: %lu\n", row, col, index);
	return index;
}

size_t LAMatGetWidth(LAMat_t *mat){
	if(mat == NULL){
		return LA_MAT_INDEX_FAIL;
	}

	if(!(LAMatIsValid(mat))){
		return LA_MAT_INDEX_FAIL;
	}

	size_t width = 0;

	if(LAMatIsTransposed(mat)){
		width = (LAMatIsView(mat)) ? mat->viewData.virtualHeight : mat->row;
	} else {
		width = (LAMatIsView(mat)) ? mat->viewData.virtualWidth  : mat->col;
	}

	return width;
}

size_t LAMatGetHeight(LAMat_t *mat){
	if(mat == NULL){
		return LA_MAT_INDEX_FAIL;
	}

	if(!(LAMatIsValid(mat))){
		return LA_MAT_INDEX_FAIL;
	}

	size_t height = 0;

	if(LAMatIsTransposed(mat)){
		height = (LAMatIsView(mat)) ? mat->viewData.virtualWidth  : mat->col;
	} else {
		height = (LAMatIsView(mat)) ? mat->viewData.virtualHeight : mat->row;
	}

	return height;
}

bool LAMatValidateCoordinates(LAMat_t *mat, size_t row, size_t col){
	if(mat == NULL)				goto matrix_fail;
	if(!(LAMatIsValid(mat))) 	goto matrix_fail;

	size_t width = LAMatGetWidth(mat);
	if(width == LA_MAT_INDEX_FAIL) goto matrix_fail;

	size_t height = LAMatGetHeight(mat);
	if(height == LA_MAT_INDEX_FAIL) goto matrix_fail;
	
	if(row >= height) goto out_of_bound;
	if(col >= width)  goto out_of_bound;
	
	return true;

matrix_fail:
	LA_RAISE_ERROR(LA_ERROR_MATRIX);
	return false;

out_of_bound:
	LA_RAISE_ERROR(LA_ERROR_OUTOFRANGE);
	return false;
}

LAErrorCode LAMatGet(LAMat_t *mat, size_t row, size_t col, double *value){
	LA_HANDLE_NULLPTR(mat, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value, LA_PROPAGATE_ERROR);

	// Check if row and col are outside the matrix
	if(!(LAMatValidateCoordinates(mat, row, col))) return LA_ERROR_MATRIX;

	// Check is READ flag is enabled
	if(!(LAMatIsReadable(mat))) return LA_ERROR_PERMISSIONS;

	size_t index = LAMatGetIndex(mat, row, col);
	if(index == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	(*value) = mat->data[index];

	return LA_NO_ERROR;
}

LAErrorCode LAMatSet(LAMat_t *mat, size_t row, size_t col, double value){
	LA_HANDLE_NULLPTR(mat, LA_PROPAGATE_ERROR);

	// Check if row and col are outside the matrix
	if(!(LAMatValidateCoordinates(mat, row, col))) return LA_ERROR_MATRIX;

	// Check is WRITE flag is enabled
	if(!(LAMatIsWritable(mat))) return LA_ERROR_PERMISSIONS;

	size_t index = LAMatGetIndex(mat, row, col);
	if(index == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	mat->data[index] = value;

	return LA_NO_ERROR;
}

LAErrorCode LAMatShape(LAMat_t *m, size_t *r, size_t *c){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(r, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);
	
	size_t mw = LAMatGetWidth(m);
	if(mw == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	size_t mh = LAMatGetHeight(m);
	if(mh == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	(*r) = mh;
	(*c) = mw;

	return LA_NO_ERROR;
}
