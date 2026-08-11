#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "matrix.h"

LAErrorCode LAMatFrobeniusNormBounded(LAMat_t *m, size_t r0, size_t c0, size_t rf, size_t cf, double *norm){
	LA_HANDLE_NULLPTR(m, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(norm, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m))) 					return LA_ERROR_MATRIX;

	LAErrorCode code = LA_NO_ERROR;

	size_t w,h;
	code = LAMatShape(m, &h, &w);
	if(code) return code;

	if(r0 >= h) return LA_ERROR_NONMATCHING_DIMENSION;
	if(c0 >= w) return LA_ERROR_NONMATCHING_DIMENSION;
	if(rf >= h) return LA_ERROR_NONMATCHING_DIMENSION;
	if(cf >= w) return LA_ERROR_NONMATCHING_DIMENSION;

	double value = 0.0;
	double acc   = 0.0;
	size_t temp  = 0;

	if(r0 > rf){
		temp = rf;
		rf   = r0;
		r0   = temp;
	}

	if(c0 > cf){
		temp = cf;
		cf   = c0;
		c0   = temp;
	}

	for(size_t y = r0; y <= rf; y++){
		for(size_t x = c0; x <= cf; x++){
			code = LAMatGet(m, y, x, &value);
			if(code) return code;

			acc += (value * value);
		}
	}
	
	(*norm) = sqrt(acc);

	return LA_NO_ERROR;
}

LAErrorCode LAMatFrobeniusNormShape(LAMat_t *m, size_t r0, size_t c0, size_t h, size_t w, double *norm){
	LA_HANDLE_NULLPTR(m, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(norm, LA_PROPAGATE_ERROR);

	size_t rf 		= r0 + h - 1;
	size_t cf 		= c0 + w - 1;
	size_t temp 	= 0;

	if(r0 > rf){
		temp = rf;
		rf   = r0;
		r0   = temp;
	}

	if(c0 > cf){
		temp = cf;
		cf   = c0;
		c0   = temp;
	}

	return LAMatFrobeniusNormBounded(m, r0, c0, rf, cf, norm);
}

LAErrorCode LAMatFrobeniusNorm(LAMat_t *m, double *norm){
	LA_HANDLE_NULLPTR(m, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(norm, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m))) 					return LA_ERROR_MATRIX;

	LAErrorCode code = LA_NO_ERROR;

	size_t w,h;
	code = LAMatShape(m, &h, &w);
	if(code) return code;

	size_t r0 = 0;
	size_t c0 = 0;
	size_t rf = h-1;
	size_t cf = w-1;

	return LAMatFrobeniusNormBounded(m, r0, c0, rf, cf, norm);
}

LAErrorCode LAMatFrobeniusNormRow(LAMat_t *m, size_t r, double *norm){
	LA_HANDLE_NULLPTR(m, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(norm, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m))) 					return LA_ERROR_MATRIX;

	LAErrorCode code = LA_NO_ERROR;

	size_t w,h;
	code = LAMatShape(m, &h, &w);
	if(code) return code;

	if(r >= h) return LA_ERROR_NONMATCHING_DIMENSION;

	size_t r0 = r;
	size_t c0 = 0;
	size_t rf = r;
	size_t cf = w-1;

	return LAMatFrobeniusNormBounded(m, r0, c0, rf, cf, norm);
}


LAErrorCode LAMatFrobeniusNormCol(LAMat_t *m, size_t c, double *norm){
	LA_HANDLE_NULLPTR(m, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(norm, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m))) 					return LA_ERROR_MATRIX;

	LAErrorCode code = LA_NO_ERROR;

	size_t w,h;
	code = LAMatShape(m, &h, &w);
	if(code) return code;

	if(c >= w) return LA_ERROR_NONMATCHING_DIMENSION;

	size_t r0 = 0;
	size_t c0 = c;
	size_t rf = h-1;
	size_t cf = c;

	return LAMatFrobeniusNormBounded(m, r0, c0, rf, cf, norm);
}
