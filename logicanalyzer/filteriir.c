#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "liblogicanalyzer.h"
#include "error.h"
#include "utils.h"
#include "filter/windows.h"
#include "filter/filter.h"

LAErrorCode LAFilterIIRCompile(double *x, size_t nx, double *y, size_t ny, double *a, size_t na, double *b, size_t nb){
	LA_HANDLE_NULLPTR(x,	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y,	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(a,	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b,	LA_PROPAGATE_ERROR);

	double a0 = 1;
	if(na > 0) a0 = a[0];
		
	double acc = 0;
	int64_t index = 0;
	for(size_t i = 0; i < ny; i++){
		
		acc = 0;

		if(na > 1){
			for(size_t j = 1; j < na; j++){
				index = i - j;
				if(index < 0) continue;
				acc -= a[j] * y[index];
			}
		}

		if(nb > 0){
			for(size_t k = 0; k < nb; k++){
				index = i - k;
				if(index < 0) continue;
				acc += b[k] * x[index];
			}
		}

		y[i] = (1 / (double) a0) * acc;
	}

	(void) nx;
	return LA_NO_ERROR;
}

LAErrorCode LAFilterMovingAverageCompile(double *x, size_t nx, double *y, size_t ny, size_t l){
	LA_HANDLE_NULLPTR(x, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	double acc = 0;

	for(size_t n = 0; n < ny; n++){
		acc = 0;
		for(size_t k = 0; k < l; k++){
			if((int64_t)(n-k) < 0) 	continue;
			if((n-k) >= nx)	break;
			acc += x[n-k];
		}
		y[n] = acc / (double) l;
	}

	return LA_NO_ERROR;
}
