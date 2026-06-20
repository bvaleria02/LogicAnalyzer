#ifndef LA_NUMERIC_METHODS_DERIVATIVE_H
#define LA_NUMERIC_METHODS_DERIVATIVE_H

double LAFirstDerivative(double *x, size_t l, size_t n, double h);
double LASecondDerivative(double *x, size_t l, size_t n, double h);
LAErrorCode LADerivateArray(double *y, size_t ny, double h, double y0);

#endif //LA_NUMERIC_METHODS_DERIVATIVE_H

