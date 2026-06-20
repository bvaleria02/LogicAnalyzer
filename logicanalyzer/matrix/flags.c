#include "../liblogicanalyzer.h"
#include "matrix.h"
#include <stdlib.h>
#include <stdbool.h>

// if matrix is NULL, always return false 
// to avoid null-dereference further down
// LA_HANDLE_NULLPTR(mat, false);

bool LAMatIsReadable(LAMat_t *mat){
	LA_HANDLE_NULLPTR(mat, false);
	return (mat->flags & LA_MAT_READ);
}

bool LAMatIsWritable(LAMat_t *mat){
	LA_HANDLE_NULLPTR(mat, false);
	return (mat->flags & LA_MAT_WRITE);
}

bool LAMatIsView(LAMat_t *mat){
	LA_HANDLE_NULLPTR(mat, false);
	return (mat->flags & LA_MAT_IS_VIEW);
}

bool LAMatIsTransposed(LAMat_t *mat){
	LA_HANDLE_NULLPTR(mat, false);
	return (mat->flags & LA_MAT_TRANSPOSED);
}

bool LAMatIsSet(LAMat_t *mat){
	LA_HANDLE_NULLPTR(mat, false);
	return (mat->flags & LA_MAT_IS_SET);
}

bool LAMatIsAllocated(LAMat_t *mat){
	LA_HANDLE_NULLPTR(mat, false);
	return (mat->flags & LA_MAT_IS_ALLOCATED);
}

bool LAMatIsValid(LAMat_t *mat){
	LA_HANDLE_NULLPTR(mat, false);

	if(mat->data == NULL) 	return false;
	if(!(LAMatIsSet(mat))) 	return false;
	if(mat->col == 0)		return false;
	if(mat->row == 0)		return false;

	return true;
}

bool LAMatAreSameDim(LAMat_t *m1, LAMat_t *m2){
	LA_HANDLE_NULLPTR(m1, false);
	LA_HANDLE_NULLPTR(m2, false);

	size_t w1 = LAMatGetWidth(m1);
	if(w1 == LA_MAT_INDEX_FAIL) return false;
	size_t w2 = LAMatGetWidth(m2);
	if(w2 == LA_MAT_INDEX_FAIL) return false;
	size_t h1 = LAMatGetHeight(m1);
	if(h1 == LA_MAT_INDEX_FAIL) return false;
	size_t h2 = LAMatGetHeight(m2);
	if(h2 == LA_MAT_INDEX_FAIL) return false;

	if(w1 != w2) return false;
	if(h1 != h2) return false;

	return true;
}

bool LAMatAreMulDim(LAMat_t *m1, LAMat_t *m2, LAMat_t *m3){
	LA_HANDLE_NULLPTR(m1, false);
	LA_HANDLE_NULLPTR(m2, false);
	LA_HANDLE_NULLPTR(m3, false);

	size_t w1 = LAMatGetWidth(m1);
	if(w1 == LA_MAT_INDEX_FAIL) return false;
	size_t w2 = LAMatGetWidth(m2);
	if(w2 == LA_MAT_INDEX_FAIL) return false;
	size_t w3 = LAMatGetWidth(m3);
	if(w3 == LA_MAT_INDEX_FAIL) return false;
	size_t h1 = LAMatGetHeight(m1);
	if(h1 == LA_MAT_INDEX_FAIL) return false;
	size_t h2 = LAMatGetHeight(m2);
	if(h2 == LA_MAT_INDEX_FAIL) return false;
	size_t h3 = LAMatGetHeight(m3);
	if(h3 == LA_MAT_INDEX_FAIL) return false;

	if(w1 != h2) return false;
	if(w2 != w3) return false;
	if(h1 != h3) return false;

	return true;
}

bool LAMatIsSquare(LAMat_t *m1){
	LA_HANDLE_NULLPTR(m1, false);

	size_t w1 = LAMatGetWidth(m1);
	if(w1 == LA_MAT_INDEX_FAIL) return false;
	size_t h1 = LAMatGetHeight(m1);
	if(h1 == LA_MAT_INDEX_FAIL) return false;

	if(w1 != h1) return false;

	return true;
}

bool LAMatMatchDim(LAMat_t *m, size_t h, size_t w){
	LA_HANDLE_NULLPTR(m, false);
	
	size_t mh, mw;
	LAErrorCode code = LAMatShape(m, &mh, &mw);
	if(code) return false;

	if(h != mh) return false;
	if(w != mw) return false;

	return true;
}
