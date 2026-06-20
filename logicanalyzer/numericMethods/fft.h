#ifndef LA_NUMERIC_METHODS_FFT_H
#define LA_NUMERIC_METHODS_FFT_H

#include "../liblogicanalyzer.h"
#include <stdlib.h>

LAErrorCode LAFFT(double *x, size_t n, double **w, size_t *nw);
LAErrorCode LAFFTWindow(double *x, size_t n, double **w, size_t *nw, LAFilterWindowType windowType, double *windowParams);
LAErrorCode LAFFTWindow2(double *x, size_t n, double **w, double **p, size_t *nw, LAFilterWindowType windowType, double *windowParams);

#endif //LA_NUMERIC_METHODS_FFT_H
