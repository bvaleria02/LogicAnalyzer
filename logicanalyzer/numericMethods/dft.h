#ifndef LA_NUMERIC_METHODS_DFT_H
#define LA_NUMERIC_METHODS_DFT_H

#include "../liblogicanalyzer.h"

// dft.c
LAErrorCode LAUnilateralDiscreteFT(double *x, size_t nx, double *w, double *p, size_t nw, LAFilterWindowType windowType, double *windowParams);
LAErrorCode LADiscreteFT(double *x, size_t nx, double *w, double *p, size_t nw, LAFilterWindowType windowType, double *windowParams);
LAErrorCode LAUnilateralDiscreteFTFortran(double *x, size_t nx, double *w, size_t nw, LAFilterWindowType windowType, double *windowParams);
LAErrorCode LADiscreteFTFortran(double *x, size_t nx, double *w, size_t nw, LAFilterWindowType windowType, double *windowParams);

// dft.f90
LAErrorCode LAUNILATERAL_DFT_NO_PHASE(double *x, size_t nx, double *y, size_t ny, double *w);
LAErrorCode LABILATERAL_DFT_NO_PHASE(double *x, size_t nx, double *y, size_t ny, double *w);
LAErrorCode LABILATERAL_DFT(double *x, size_t nx, double *y, double *p, size_t ny, double *w);

#endif // LA_NUMERIC_METHODS_DFT_H
