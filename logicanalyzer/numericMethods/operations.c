#include "../liblogicanalyzer.h"
#include "operations.h"
#include <stdlib.h>
#include <stdint.h>

LAErrorCode LAMulArray(double *x, size_t nx, double f){
	LA_HANDLE_NULLPTR(x, LA_PROPAGATE_ERROR);

	for(size_t n = 0; n < nx; n++){
		x[n] *= f;
	}

	return LA_NO_ERROR;
}
