#include "../liblogicanalyzer.h"
#include "laComplex.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

double la_modulo(double a, double b){
	return sqrt((a*a) + (b*b));
}

double la_arg(double y, double x){
	return atan2(y, x);
}

LAErrorCode LAComplexDoubleModulo(LAComplexDouble *cmpx, double *res){
	LA_HANDLE_NULLPTR(cmpx, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(res, 	LA_PROPAGATE_ERROR);

	(*res) = la_modulo(cmpx->re, cmpx->im);

	return LA_NO_ERROR;
}

LAErrorCode LAComplexFloatModulo(LAComplexFloat *cmpx, float *res){
	LA_HANDLE_NULLPTR(cmpx, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(res, 	LA_PROPAGATE_ERROR);

	(*res) = la_modulo(cmpx->re, cmpx->im);
	return LA_NO_ERROR;
}

LAErrorCode LAComplexDoubleArg(LAComplexDouble *cmpx, double *res){
	LA_HANDLE_NULLPTR(cmpx, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(res, 	LA_PROPAGATE_ERROR);

	(*res) = la_arg(cmpx->im, cmpx->re);

	return LA_NO_ERROR;
}

LAErrorCode LAComplexFloatArg(LAComplexFloat *cmpx, float *res){
	LA_HANDLE_NULLPTR(cmpx, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(res, 	LA_PROPAGATE_ERROR);

	(*res) = la_arg(cmpx->im, cmpx->re);
	return LA_NO_ERROR;
}
