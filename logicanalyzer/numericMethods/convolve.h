#ifndef LA_NUMERIC_METHODS_CONVOLVE
#define LA_NUMERIC_METHODS_CONVOLVE

#include "../liblogicanalyzer.h"
#include <stdlib.h>

LAErrorCode LAConvolveArray(double *x, size_t nx, double *h, size_t nh, double *y, size_t ny);

#endif //LA_NUMERIC_METHODS_CONVOLVE
