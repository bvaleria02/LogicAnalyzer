#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <stdbool.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "../filter/windows.h"
#include "dft.h"

LAErrorCode LAUnilateralDiscreteFT(double *x, size_t nx, double *w, double *p, size_t nw, LAFilterWindowType windowType, double *windowParams){
	LA_HANDLE_NULLPTR(x, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(w, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(windowParams, LA_PROPAGATE_ERROR);
	
	bool isWindowAllocated = FALSE;
	LAErrorCode code = LA_NO_ERROR;

	double *window = (double *)malloc(sizeof(double) * nx);
	if(window == NULL) goto handle_error;
	isWindowAllocated = TRUE;

	code = LAFilterGenerateWindowArray(window, nx, windowType, windowParams, LA_FIR_FILTER_PARAMS);
	if(code) goto handle_error;

	double re = 0;
	double im = 0;
	double omega = 0;

	for(size_t f = 0; f < nw; f++){
		re = 0;
		im = 0;
		omega = 2 * M_PI * f/ (double) (nx);

		for(size_t n = 0; n < nx; n++){
			re += x[n] * window[n] * cos(omega * n);
			im -= x[n] * window[n] * sin(omega * n);
		}

		w[f] = sqrt((re * re) + (im * im));
		if(p != NULL){
			p[f] = atan2(im, re);
		}
	}

	goto cleanup;

handle_error:
	if(!code) LA_RAISE_ERROR(LA_ERROR_NULLPTR);
    code = LA_ERROR_NULLPTR;

cleanup:
	if(isWindowAllocated && window != NULL) free(window);

	return code;
}

LAErrorCode LADiscreteFT(double *x, size_t nx, double *w, double *p, size_t nw, LAFilterWindowType windowType, double *windowParams){
	LA_HANDLE_NULLPTR(x, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(w, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(windowParams, LA_PROPAGATE_ERROR);
	
	bool isWindowAllocated = FALSE;
	LAErrorCode code = LA_NO_ERROR;

	double *window = (double *)malloc(sizeof(double) * nx);
	if(window == NULL) goto handle_error;
	isWindowAllocated = TRUE;

	code = LAFilterGenerateWindowArray(window, nx, windowType, windowParams, LA_FIR_FILTER_PARAMS);
	if(code) goto handle_error;

	double re = 0;
	double im = 0;
	double omega = 0;

	for(size_t f = 0; f < nw; f++){
		re = 0;
		im = 0;
		omega = (2 * M_PI / (double) nx) * (f - (nw / (double) 2));

		for(size_t n = 0; n < nx; n++){
			re += x[n] * window[n] * cos(omega * n);
			im -= x[n] * window[n] * sin(omega * n);
		}

		w[f] = sqrt((re * re) + (im * im));
		if(p != NULL){
			p[f] = atan2(im, re);
		}
	}

	goto cleanup;

handle_error:
	if(!code) LA_RAISE_ERROR(LA_ERROR_NULLPTR);
    code = LA_ERROR_NULLPTR;

cleanup:
	if(isWindowAllocated && window != NULL) free(window);

	return code;
}

LAErrorCode LAUnilateralDiscreteFTFortran(double *x, size_t nx, double *y, size_t ny, LAFilterWindowType windowType, double *windowParams){
	LA_HANDLE_NULLPTR(x, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(windowParams, LA_PROPAGATE_ERROR);
	
	bool isWindowAllocated = FALSE;
	LAErrorCode code = LA_NO_ERROR;

	double *window = (double *)malloc(sizeof(double) * nx);
	if(window == NULL) goto handle_error;
	isWindowAllocated = TRUE;

	code = LAFilterGenerateWindowArray(window, nx, windowType, windowParams, LA_FIR_FILTER_PARAMS);
	if(code) goto handle_error;

	LAUNILATERAL_DFT_NO_PHASE(x, nx, y, ny, window);

	goto cleanup;

handle_error:
	if(!code) LA_RAISE_ERROR(LA_ERROR_NULLPTR);
    code = LA_ERROR_NULLPTR;

cleanup:
	if(isWindowAllocated && window != NULL) free(window);

	return code;
}

LAErrorCode LADiscreteFTFortran(double *x, size_t nx, double *y, size_t ny, LAFilterWindowType windowType, double *windowParams){
	LA_HANDLE_NULLPTR(x, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(windowParams, LA_PROPAGATE_ERROR);
	
	bool isWindowAllocated = FALSE;
	LAErrorCode code = LA_NO_ERROR;

	double *window = (double *)malloc(sizeof(double) * nx);
	if(window == NULL) goto handle_error;
	isWindowAllocated = TRUE;

	code = LAFilterGenerateWindowArray(window, nx, windowType, windowParams, LA_FIR_FILTER_PARAMS);
	if(code) goto handle_error;

	LABILATERAL_DFT_NO_PHASE(x, nx, y, ny, window);

	goto cleanup;

handle_error:
	if(!code) LA_RAISE_ERROR(LA_ERROR_NULLPTR);
    code = LA_ERROR_NULLPTR;

cleanup:
	if(isWindowAllocated && window != NULL) free(window);
	return code;
}

LAErrorCode LADiscreteFTFortranPhase(double *x, size_t nx, double *y, double *p, size_t ny, LAFilterWindowType windowType, double *windowParams){
	LA_HANDLE_NULLPTR(x, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(p, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(windowParams, LA_PROPAGATE_ERROR);
	
	bool isWindowAllocated = FALSE;
	LAErrorCode code = LA_NO_ERROR;

	double *window = (double *)malloc(sizeof(double) * nx);
	if(window == NULL) goto handle_error;
	isWindowAllocated = TRUE;

	code = LAFilterGenerateWindowArray(window, nx, windowType, windowParams, LA_FIR_FILTER_PARAMS);
	if(code) goto handle_error;

	LABILATERAL_DFT(x, nx, y, p, ny, window);

	goto cleanup;

handle_error:
	if(!code) LA_RAISE_ERROR(LA_ERROR_NULLPTR);
    code = LA_ERROR_NULLPTR;

cleanup:
	if(isWindowAllocated && window != NULL) free(window);
	return code;
}
