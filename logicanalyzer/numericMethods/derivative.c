#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "solver.h"
#include "ss.h"

double LAFirstDerivative(double *x, size_t l, size_t n, double h){
	LA_HANDLE_NULLPTR(x, 0.0);
	double x1 = 0;
	double x2 = 0;

	x1 = (n >= l) ? x[l-1] : x[n];
	
	if(n >= (l+1)){
		x2 = x[l-1];
	} else if (n > 0){
		x2 = x[n-1];
	}

	return (x1 - x2) / h;
}

double LASecondDerivative(double *x, size_t l, size_t n, double h){
	LA_HANDLE_NULLPTR(x, 0.0);
	double x1 = 0;
	double x2 = 0;
	double x3 = 0;

	x2 = (n >= l) ? x[l-1] : x[n];
	
	if(n >= (l+1)){
		x3 = x[l-1];
	} else if (n > 0){
		x3 = x[n-1];
	}

	if(n >= (l-1)){
		x1 = x[l-1];
	} else {
		x1 = x[n+1];
	}

	return (x1 - 2*x2 + x3) / (h*h);
}

LAErrorCode LADerivateArray(double *y, size_t ny, double h, double y0){
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	double y1 = 0.0;
	double y2 = y0;

	for(size_t n = 0; n < ny; n++){
		y1 = y[n];
		
		y[n] = (y1 - y2) / h;
		
		y2 = y1;
	}

	return LA_NO_ERROR;
}
