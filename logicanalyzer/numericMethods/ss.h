#ifndef LA_NUMERIC_METHODS_STATUS_SYSTEM
#define LA_NUMERIC_METHODS_STATUS_SYSTEM

#include "../liblogicanalyzer.h"
#include <stdlib.h>

typedef LAErrorCode (*LASSCallback)(double *, double *, size_t, void *);
typedef LAErrorCode (*LASSCallbackIndex)(size_t, void *, void *);

LAErrorCode LAStateSystemSolver(double *a, double *b, double *c, double *d, double *dx, double *u, double *x, double *y, size_t s, size_t solver, LASolverCallback f, void *p, double h, double t0, LASSCallback fc, LASSCallbackIndex fi, void *de);

#endif //LA_NUMERIC_METHODS_STATUS_SYSTEM
