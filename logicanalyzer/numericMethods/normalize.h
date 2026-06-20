#ifndef LA_NUMERIC_METHODS_NORMALIZE
#define LA_NUMERIC_METHODS_NORMALIZE

#include "../liblogicanalyzer.h"
#include <stdlib.h>

LAErrorCode LANormalizeArrayRMS(double *x, size_t n);
LAErrorCode LANormalizeArrayMinMax(double *x, size_t n);

#endif //LA_NUMERIC_METHODS_NORMALIZE
