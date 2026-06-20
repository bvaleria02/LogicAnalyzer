#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include "liblogicanalyzer.h"

LAErrorCode LALinspace(double *x, double min, double max, size_t points){
	LA_HANDLE_NULLPTR(x, LA_PROPAGATE_ERROR);

	if(points == 0) return LA_NO_ERROR;

	if(points == 1){
		x[0] = min;
		return LA_NO_ERROR;
	}

	for(size_t i = 0; i < points; i++){
		x[i] = min + (max - min) * (i / (double) (points - 1));
	}

	return LA_NO_ERROR;
}
