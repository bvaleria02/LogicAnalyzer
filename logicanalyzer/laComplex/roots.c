#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "laComplex.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

LAErrorCode LAComplexDoubleRoots(LAComplexDouble *r1, LAComplexDouble *r2, double a, double b, double c){
	LA_HANDLE_NULLPTR(r1, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(r2, 	LA_PROPAGATE_ERROR);

	double x = -b / (double) (2*a);
	double d = b*b - (4*a*c);

	if(d < 0){
		// 2 complex roots
//		printf("Complex\n");
		r1->re = x;
		r2->re = x;
		r1->im = -(sqrt(fabs(d)) / (double) (2*a));
		r2->im =  (sqrt(fabs(d)) / (double) (2*a));
	} else if(d == 0){
		// 2 real roots, equal
		r1->re = x;
		r2->re = x;
		r1->im = 0;
		r2->im = 0;
	} else {
		// 2 real roots, different
		r1->re = x - (sqrt(d) / (double) (2*a));
		r2->re = x + (sqrt(d) / (double) (2*a));
		r1->im = 0;
		r2->im = 0;
	}

//	printf("r1\tre: %lf\t im: %lf\n", r1->re, r1->im);
//	printf("r2\tre: %lf\t im: %lf\n", r2->re, r2->im);
	return LA_NO_ERROR;
}

LAErrorCode LAComplexDoubleRootsArray(LAComplexDouble *r1, LAComplexDouble *r2, double *coef, const size_t size){
	LA_HANDLE_NULLPTR(r1, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(r2, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(coef, LA_PROPAGATE_ERROR);

	if(size != 3){
		LA_RAISE_ERROR(LA_ERROR_INCORRECTVALUE);
		return LA_ERROR_INCORRECTVALUE;
	}

	LAErrorCode code = LAComplexDoubleRoots(r1, r2, coef[0], coef[1], coef[2]);
	return code;
}
