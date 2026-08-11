#include <stdlib.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "matrix.h"

LAErrorCode LAMatCreateFromArrayFlags(LAMat_t *mat, size_t row, size_t col, double *data, LAMatFlags flags){
	LA_HANDLE_NULLPTR(mat, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(data, LA_PROPAGATE_ERROR);

	if(row == 0) return LA_ERROR_ZEROLENGTH;
	if(col == 0) return LA_ERROR_ZEROLENGTH;

	LAErrorCode code = LAMatInit(mat);
	if(code) return code;

	mat->row = row;
	mat->col = col;
	mat->data = data;
	mat->flags = mat->flags | flags | LA_MAT_IS_SET;

	return LA_NO_ERROR;
}

LAErrorCode LAMatCreateFromArray(LAMat_t *mat, size_t row, size_t col, double *data){
	LA_HANDLE_NULLPTR(mat, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(data, LA_PROPAGATE_ERROR);

	LAMatFlags flags = LA_MAT_READ | LA_MAT_WRITE;

	LAErrorCode code = LAMatCreateFromArrayFlags(mat, row, col, data, flags);
	return code;
}

LAErrorCode LAMatCreateDynamicFlags(LAMat_t *mat, size_t row, size_t col, LAMatFlags flags){
	LA_HANDLE_NULLPTR(mat, LA_PROPAGATE_ERROR);

	if(row == 0) return LA_ERROR_ZEROLENGTH;
	if(col == 0) return LA_ERROR_ZEROLENGTH;

	double *data = (double *)malloc(sizeof(double) * row * col);
	if(data == NULL) return LA_ERROR_MALLOC;

	flags = flags | LA_MAT_IS_ALLOCATED;

	LAErrorCode code = LAMatCreateFromArrayFlags(mat, row, col, data, flags);
	return code;
}

LAErrorCode LAMatCreateDynamic(LAMat_t *mat, size_t row, size_t col){
	LA_HANDLE_NULLPTR(mat, LA_PROPAGATE_ERROR);

	LAMatFlags flags = LA_MAT_READ | LA_MAT_WRITE;

	LAErrorCode code = LAMatCreateDynamicFlags(mat, row, col, flags);
	return code;
}

LAErrorCode LAMatCreateViewFlags(LAMat_t *dest, LAMat_t *src, size_t r0, size_t c0, size_t rf, size_t cf, LAMatFlags flags){
	LA_HANDLE_NULLPTR(src, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(dest, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(src))) return LA_ERROR_MATRIX;
	
	size_t w = LAMatGetWidth(src);
	if(w == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	size_t h = LAMatGetHeight(src);
	if(h == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	
	if(r0 >= h) return LA_ERROR_NONMATCHING_DIMENSION;
	if(rf >= h) return LA_ERROR_NONMATCHING_DIMENSION;
	if(c0 >= w) return LA_ERROR_NONMATCHING_DIMENSION;
	if(cf >= w) return LA_ERROR_NONMATCHING_DIMENSION;

	size_t temp = 0;

	if(r0 > rf){
		temp = rf;
		r0   = rf;
		rf   = temp;
	}

	if(c0 > cf){
		temp = cf;
		c0   = cf;
		cf   = temp;
	}

	size_t viewWidth  = cf - c0 + 1;
	size_t viewHeight = rf - r0 + 1;

	dest->flags = src->flags  & ~(LA_MAT_IS_ALLOCATED);
	dest->flags = dest->flags & ~(LA_MAT_WRITE);
	dest->flags = dest->flags |   LA_MAT_IS_SET;
	dest->flags = dest->flags |   LA_MAT_IS_VIEW;
	dest->flags = dest->flags |   flags;

	if(LAMatIsTransposed(src)){
		dest->viewData.rowOffset = c0;
		dest->viewData.colOffset = r0;
		dest->viewData.virtualWidth = viewHeight;
		dest->viewData.virtualHeight = viewWidth;
	} else {
		dest->viewData.rowOffset = r0;
		dest->viewData.colOffset = c0;
		dest->viewData.virtualWidth = viewWidth;
		dest->viewData.virtualHeight = viewHeight;
	}

	dest->col = src->col;
	dest->row = src->row;

	dest->data = src->data;

	return LA_NO_ERROR;
}

LAErrorCode LAMatCreateView(LAMat_t *dest, LAMat_t *src, size_t r0, size_t c0, size_t rf, size_t cf){
	LA_HANDLE_NULLPTR(src, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(dest, LA_PROPAGATE_ERROR);

	LAMatFlags flags = 0x0;

	LAErrorCode code = LAMatCreateViewFlags(dest, src, r0, c0, rf, cf, flags);
	return code;
}
