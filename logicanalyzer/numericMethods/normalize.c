#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../liblogicanalyzer.h"
#include <math.h>
#include <stdbool.h>
#include "normalize.h"

LAErrorCode LANormalizeArrayRMS(double *x, size_t n){
	LA_HANDLE_NULLPTR(x, LA_PROPAGATE_ERROR);
	if(n == 0){
		LA_RAISE_ERROR(LA_ERROR_ZEROLENGTH);
		return LA_ERROR_ZEROLENGTH;
	};

	double acc = 0;
	for(size_t i = 0; i < n; i++){
		acc += x[i] * x[i];
	}

	// Avoid inf/NaN
	if(acc == 0) return LA_NO_ERROR;

	double rms = acc / (double) n;
	double norm = 1 / sqrt(rms);

	for(size_t i = 0; i < n; i++){
		x[i] *= norm;
	}

	return LA_NO_ERROR;
}

LAErrorCode LANormalizeArrayMinMax(double *x, size_t n){
	LA_HANDLE_NULLPTR(x, LA_PROPAGATE_ERROR);
	if(n == 0){
		LA_RAISE_ERROR(LA_ERROR_ZEROLENGTH);
		return LA_ERROR_ZEROLENGTH;
	};

	double minval 	= x[0];
	double maxval 	= x[0];
	double sum		= 0;

	for(size_t i = 0; i < n; i++){
		if(x[i] < minval) minval = x[i];
		if(x[i] > maxval) maxval = x[i];
		sum += x[i];
	}

	double mean 	= sum / (double) n;
	double weight 	= ((maxval - mean) > (mean - minval)) ? (maxval - mean) : (mean - minval);
	// Avoid inf/NaN
	if(weight == 0) return LA_NO_ERROR;

	for(size_t i = 0; i < n; i++){
		x[i] = (x[i] - mean) / weight; 
	}

	return LA_NO_ERROR;
}
