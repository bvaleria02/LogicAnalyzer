#include "../liblogicanalyzer.h"
#include "matrix.h"
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

LAErrorCode LAMatCopy(LAMat_t *src, LAMat_t *dest){
	LA_HANDLE_NULLPTR(src, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(dest, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(src))) 	return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(dest))) 	return LA_ERROR_MATRIX;

	if(!(LAMatAreSameDim(src, dest))) return LA_ERROR_NONMATCHING_DIMENSION;

	size_t w = LAMatGetWidth(src);
	if(w == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;
	size_t h = LAMatGetHeight(src);
	if(h == LA_MAT_INDEX_FAIL) return LA_ERROR_MATRIX;

	LAErrorCode code = LA_NO_ERROR;

	double value = 0;

	for(size_t y = 0; y < h; y++){
		for(size_t x = 0; x < w; x++){
			code = LAMatGet(src, y, x, &value);
			if(code) return code;
			code = LAMatSet(dest, y, x, value);
			if(code) return code;
		}
	}

	return LA_NO_ERROR;
}
