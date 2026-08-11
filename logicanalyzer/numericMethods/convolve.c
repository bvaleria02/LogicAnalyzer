#include <stdint.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "convolve.h"

LAErrorCode LAConvolveArray(double *x, size_t nx, double *h, size_t nh, double *y, size_t ny){
	LA_HANDLE_NULLPTR(x, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(h, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	double acc = 0;
	int64_t s = 0;

	for(size_t n = 0; n < ny; n++){
		acc = 0;
		for(size_t k = 0; k < nh; k++){
			s = n - k;

			if(s < 0)				continue;
			if(s >= (int64_t) nx) 	continue;

			acc += h[k] * x[s];
		}
		y[n] = acc;
	}
	
	return LA_NO_ERROR;
}

