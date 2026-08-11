#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "../filter/windows.h"
#include "fft.h"

LAErrorCode LAFFT_core(double *a, double *b, const size_t n, const bool invert){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	if(n == 1) return LA_NO_ERROR;
	LAErrorCode code = LA_NO_ERROR;

	double *a0 = NULL;
	double *a1 = NULL;
	double *b0 = NULL;
	double *b1 = NULL;

	const size_t n2 = n / 2;

	LA_PROFILER({
		a0 = (double *)malloc(n2 * sizeof(double));
		if(a0 == NULL) goto malloc_error;
		a1 = (double *)malloc(n2 * sizeof(double));
		if(a1 == NULL) goto malloc_error;
		b0 = (double *)malloc(n2 * sizeof(double));
		if(b0 == NULL) goto malloc_error;
		b1 = (double *)malloc(n2 * sizeof(double));
		if(b1 == NULL) goto malloc_error;
	}, "FFT Malloc");

	for(size_t i = 0; (2 * i) < n; i++){
		a0[i] = a[2 * i];
		a1[i] = a[2 * i + 1];
		b0[i] = 0;
		b1[i] = 0;
	}

	code = LAFFT_core(a0, b0, n2, invert);
	if(code) goto cleanup;
	code = LAFFT_core(a1, b1, n2, invert);
	if(code) goto cleanup;

	double theta = (2 * M_PI / (double) n) * ((invert) ? -1.0 : 1.0);
	double omega_a = 1.0;
	double omega_b = 0.0;
	double omega_n_a = cos(theta);
	double omega_n_b = sin(theta);

	LA_PROFILER({
	for(size_t i = 0; (2 * i) < n; i++){
		double t_omega_a = (omega_a * a1[i]) - (omega_b * b1[i]);
		double t_omega_b = (omega_a * b1[i]) + (omega_b * a1[i]);
		a[i] 			 = a0[i] + t_omega_a;
		b[i] 			 = b0[i] + t_omega_b;
		a[i + n2] 		 = a0[i] - t_omega_a;
		b[i + n2] 		 = b0[i] - t_omega_b;
		if(invert){
			a[i] 		/= 2.0;
			b[i] 		/= 2.0;
			a[i + n2] 	/= 2.0;
			b[i + n2] 	/= 2.0;
		}
		omega_a = (omega_a * omega_n_a) - (omega_b * omega_n_b);
		omega_b = (omega_a * omega_n_b) + (omega_b * omega_n_a);
	}
	}, "FFT - Calculation");

	goto cleanup;

malloc_error:
	code = LA_ERROR_MALLOC;
	goto cleanup;

cleanup:
	LA_PROFILER({
	if(a0 != NULL) free(a0);
	if(a1 != NULL) free(a1);
	if(b0 != NULL) free(b0);
	if(b1 != NULL) free(b1);
	}, "FFT - Free");
	return code;
}

LAErrorCode LAFFT_core_inplace(double *restrict a, double * restrict b, const size_t n, const bool invert){
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);

	for(size_t i=1, j=0; i < n; i++){
		size_t bit = n >> 1;
		for(; j & bit; bit >>= 1) j ^= bit;
		j ^= bit;
		if(i < j){
			double temp = a[j];
			a[j] = a[i];
			a[i] = temp;

			temp = b[j];
			b[j] = b[i];
			b[i] = temp;
		}
	}

	for(size_t len = 2; len <= n; len <<= 1){
		double theta = (2 * M_PI / (double) len) * ((invert) ? -1.0 : 1.0);
		double omega_len_a = cos(theta);
		double omega_len_b = sin(theta);
		for(size_t i = 0; i < n; i += len){
			double omega_a = 1.0;
			double omega_b = 0.0;
			for(size_t j = 0; j < (len / 2); j++){
				double mu_a = a[i+j];
				double mu_b = b[i+j];
				double nu_a = (a[i + j + (len/2)] * omega_a) - (b[i + j + (len/2)] * omega_b);
				double nu_b = (b[i + j + (len/2)] * omega_a) + (a[i + j + (len/2)] * omega_b);
				a[i + j] = mu_a + nu_a;
				b[i + j] = mu_b + nu_b;
				a[i + j + (len/2)] = mu_a - nu_a;
				b[i + j + (len/2)] = mu_b - nu_b;

				double temp_omega_a = omega_a;
				omega_a = (omega_a * omega_len_a) - (     omega_b * omega_len_b);
				omega_b = (omega_b * omega_len_a) + (temp_omega_a * omega_len_b);
			}
		}
	}

	if(invert){
		for(size_t i = 0; i < n; i++){
			a[i] /= (double) n;
			b[i] /= (double) n;
		}
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFFT(double *x, size_t n, double **w, size_t *nw){
	LA_HANDLE_NULLPTR(x,  LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(w,  LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(nw, LA_PROPAGATE_ERROR);

	if(n == 0) return LA_ERROR_ZEROLENGTH;
	LAErrorCode code = LA_NO_ERROR;
	bool isOmegaAAlloc = false;
	bool isOmegaBAlloc = false;
	double *wb = NULL;

	size_t n_padded = 0x1 << ((sizeof(size_t) * 8) - __builtin_clzl(n));

	(*w) = (double *)malloc(n_padded * sizeof(double));
	if((*w) == NULL) goto handle_malloc_error;
	wb   = (double *)malloc(n_padded * sizeof(double));
	if(wb == NULL) goto handle_malloc_error;

	// Padded copy
	memset(wb,   						0x0,	n_padded        * sizeof(double));
	memset((*w), 						0x0, 	(n_padded - n)  * sizeof(double));
	memcpy(&((*w)[n_padded - 1 - n]), 	x, 		n 				* sizeof(double));

	code = LAFFT_core_inplace((*w), wb, n_padded, false);
	if(code) goto handle_error;

	for(size_t i = 0; i < n_padded; i++){
		(*w)[i] = sqrt(pow((*w)[i], 2) + pow(wb[i], 2));
	}

	(*nw) = n_padded;
	// (*w) must be freed elsewhere
	if(wb != NULL) free(wb);
	return LA_NO_ERROR;

handle_malloc_error:
	code = LA_ERROR_MALLOC;
	goto handle_error;

handle_error:
	if(((*w) != NULL) && isOmegaAAlloc) free((*w));
	if((wb != NULL) && isOmegaBAlloc) free(wb);
	(*nw) = 0x0;
	return code;
}

LAErrorCode LAFFTWindow(double *x, size_t n, double **w, size_t *nw, LAFilterWindowType windowType, double *windowParams){
	LA_HANDLE_NULLPTR(x,  LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(w,  LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(nw, LA_PROPAGATE_ERROR);

	if(n == 0) return LA_ERROR_ZEROLENGTH;
	LAErrorCode code = LA_NO_ERROR;
	bool isOmegaAAlloc = false;
	bool isOmegaBAlloc = false;
	bool isWindowAlloc = false;
	double *wb = NULL;
	double *window = NULL;

	size_t n_padded = 0x1 << ((sizeof(size_t) * 8) - __builtin_clzl(n));

	(*w) = (double *)malloc(n_padded * sizeof(double));
	if((*w) == NULL) goto handle_malloc_error;
	wb   = (double *)malloc(n_padded * sizeof(double));
	if(wb == NULL) goto handle_malloc_error;
	window = (double *)malloc(n_padded * sizeof(double));
	if(window == NULL) goto handle_malloc_error;

	code = LAFilterGenerateWindowArray(window, n_padded, windowType, windowParams, LA_FIR_FILTER_PARAMS);
	if(code) goto handle_error;

	// Padded copy
	memset(wb,   						0x0,	n_padded        * sizeof(double));
	memset((*w), 						0x0, 	(n_padded - n)  * sizeof(double));
	memcpy(&((*w)[n_padded - 1 - n]), 	x, 		n 				* sizeof(double));

	// TODO: SIMD
	for(size_t i = 0; i < n_padded; i++){
		(*w)[i] *= window[i];
	}

	code = LAFFT_core_inplace((*w), wb, n_padded, false);
	if(code) goto handle_error;

	for(size_t i = 0; i < n_padded; i++){
		double a = (*w)[i];
		double b = wb[i];
		(*w)[i] = sqrt(a*a + b*b);
	//	wb[i] = atan2(b, a);
	}

	(*nw) = n_padded;
	// (*w) must be freed elsewhere
	if(wb != NULL) free(wb);
	if(window != NULL) free(window);
	return LA_NO_ERROR;

handle_malloc_error:
	code = LA_ERROR_MALLOC;
	goto handle_error;

handle_error:
	if(((*w) != NULL) && isOmegaAAlloc) free((*w));
	if((wb != NULL) && isOmegaBAlloc) free(wb);
	if((window != NULL) && isWindowAlloc) free(window);
	(*nw) = 0x0;
	return code;
}

LAErrorCode LAFFTWindow2(double *x, size_t n, double **w, double **p, size_t *nw, LAFilterWindowType windowType, double *windowParams){
	LA_HANDLE_NULLPTR(x,  LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(w,  LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(nw, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(p, LA_PROPAGATE_ERROR);

	if(n == 0) return LA_ERROR_ZEROLENGTH;
	LAErrorCode code = LA_NO_ERROR;
	bool isOmegaAAlloc = false;
	bool isOmegaBAlloc = false;
	bool isWindowAlloc = false;
	double *wb = NULL;
	double *window = NULL;

	size_t n_padded = 0x1 << ((sizeof(size_t) * 8) - __builtin_clzl(n));

	(*w) = (double *)malloc(n_padded * sizeof(double));
	if((*w) == NULL) goto handle_malloc_error;
	wb   = (double *)malloc(n_padded * sizeof(double));
	if(wb == NULL) goto handle_malloc_error;
	window = (double *)malloc(n_padded * sizeof(double));
	if(window == NULL) goto handle_malloc_error;

	code = LAFilterGenerateWindowArray(window, n_padded, windowType, windowParams, LA_FIR_FILTER_PARAMS);
	if(code) goto handle_error;

	// Padded copy
	memset(wb,   						0x0,	n_padded        * sizeof(double));
	memset((*w), 						0x0, 	(n_padded - n)  * sizeof(double));
	memcpy(&((*w)[n_padded - 1 - n]), 	x, 		n 				* sizeof(double));

	// TODO: SIMD
	for(size_t i = 0; i < n_padded; i++){
		(*w)[i] *= window[i];
	}

	code = LAFFT_core_inplace((*w), wb, n_padded, false);
	if(code) goto handle_error;

	for(size_t i = 0; i < n_padded; i++){
		double a = (*w)[i];
		double b = wb[i];
		(*w)[i] = sqrt(a*a + b*b);
		wb[i] = atan2(b, a);
	}

	(*nw) = n_padded;
	(*p)  = wb;
	// (*w) must be freed elsewhere
	if(window != NULL) free(window);
	return LA_NO_ERROR;

handle_malloc_error:
	code = LA_ERROR_MALLOC;
	goto handle_error;

handle_error:
	if(((*w) != NULL) && isOmegaAAlloc) free((*w));
	if((wb != NULL) && isOmegaBAlloc) free(wb);
	if((window != NULL) && isWindowAlloc) free(window);
	(*nw) = 0x0;
	return code;
}
