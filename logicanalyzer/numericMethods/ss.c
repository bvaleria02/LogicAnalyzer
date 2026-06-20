#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../liblogicanalyzer.h"
#include <math.h>
#include <stdbool.h>
#include "solver.h"
#include "ss.h"

/*
dot(x) = Ax + Bu
    y  = Cx + Du
*/



LAErrorCode LAStateSystemSolver(double *a, double *b, double *c, double *d, double *dx, double *u, double *x, double *y, size_t s, size_t solver, LASolverCallback f, void *p, double h, double t0, LASSCallback fc, LASSCallbackIndex fi, void *de){
	LA_HANDLE_NULLPTR(a, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(d, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(dx, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(u, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(f, 	LA_PROPAGATE_ERROR);
	if(s == 0){
		LA_RAISE_ERROR(LA_ERROR_INVALIDVALUE);
		return LA_ERROR_INVALIDVALUE;
	}

	LAErrorCode code = LA_NO_ERROR;

	for(size_t i = 0; i < s; i++){
		dx[i] = 0;
		for(size_t j = 0; j < s; j++){
			//printf("i: %li\ta: %li\tx: %li\n", i, LA_MATRIX_INDEX(j,i,s), j);
			dx[i] += a[LA_MATRIX_INDEX(j, i, s)] * x[j];
			dx[i] += b[LA_MATRIX_INDEX(j, i, s)] * u[j];
		}
	}

	// Callback, usually to store dx inside p
	code = LA_NO_ERROR;
	if(fc != NULL) code = fc(dx, x, s, p);
	if(code) return code;

	for(size_t i = 0; i < s; i++){

		// Callback, to switch the index
		code = LA_NO_ERROR;
		if(fc != NULL) code = fi(i, p, de);
		if(code) return code;

		x[i] = LASolver(x[i], t0, h, f, p, solver, de);
	}

	for(size_t i = 0; i < s; i++){
		y[i] = 0;
		for(size_t j = 0; j < s; j++){
			y[i] += c[LA_MATRIX_INDEX(j, i, s)] * x[j];
			y[i] += d[LA_MATRIX_INDEX(j, i, s)] * u[j];
		}
	}

	return LA_NO_ERROR;
}
