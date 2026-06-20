#include "../liblogicanalyzer.h"
#include "matrix.h"
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

LAErrorCode LAMatKronecker(LAMat_t *a, LAMat_t *b, LAMat_t *c){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(a)))	return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(b)))	return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(c)))	return LA_ERROR_MATRIX;
	
	LAErrorCode code = LA_NO_ERROR;
	size_t aw,ah,bw,bh,cw,ch;
	code = LAMatShape(a, &ah, &aw);
	if(code) return code;
	code = LAMatShape(b, &bh, &bw);
	if(code) return code;
	code = LAMatShape(c, &ch, &cw);
	if(code) return code;

	if(!(LAMatMatchDim(c, ah*bh, aw*bw))) return LA_ERROR_NONMATCHING_DIMENSION;

	double a_yx = 0.0;
	double b_nm = 0.0;
	double c_pq = 0.0;
	
	size_t p = 0;
	size_t q = 0;

	for(size_t y = 0; y < ah; y++){
		for(size_t x = 0; x < aw; x++){

			code = LAMatGet(a, y, x, &a_yx);
			if(code) return code;

			for(size_t n = 0; n < bh; n++){
				for(size_t m = 0; m < bw; m++){

					code = LAMatGet(b, n, m, &b_nm);
					if(code) return code;

					c_pq = a_yx * b_nm;
					p = (y * bh) + n;
					q = (x * bw) + m;

					code = LAMatSet(c, p, q, c_pq);
					if(code) return code;

				}
			}
		}
	}

	return LA_NO_ERROR;
}
