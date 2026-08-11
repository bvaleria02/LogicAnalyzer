#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "fftfreq.h"

LAErrorCode LAFilterGraphDrawArray(cairo_t *cr, double *array, size_t size, double width, double height, double min, double max, bool changeColor){
	LA_HANDLE_NULLPTR(cr, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	if(min >= max){
		LA_RAISE_ERROR(LA_ERROR_INCORRECTVALUE);
		return LA_ERROR_INCORRECTVALUE;
	}
	
	double x = 0;
	double y = 0;
	if(changeColor) cairo_set_source_rgb(cr, 0.8, 0.4, 0.2);
	cairo_set_line_width(cr, 2);

	for(size_t i = 0; i < size; i++){
		if(size > 1){
			x = (i / (double) (size - 1)) * width;
		} else {
			x = width;
		}

		y = (array[i] - min) / (double) (max - min);
		y = height / (double) 2 - (y - 0.5) * (height * 0.9);

		if(i == 0){
			cairo_move_to(cr, 0, y);
		}
		cairo_line_to(cr, x, y);

	}

	cairo_stroke(cr);
	return LA_NO_ERROR;
}

LAErrorCode LADrawFFTBilateral(cairo_t *cr, const size_t x, const size_t y, const size_t w, const size_t h, double *data, const size_t n, const double vmin, const double vmax, const size_t downsampler, const bool logX, const bool logY){
	LA_HANDLE_NULLPTR(cr, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(data, LA_PROPAGATE_ERROR);

	size_t ds_size = n / (double) (2 * downsampler);
	double nLog10  = log10(ds_size + 1.0);
	double midpoint = (vmin + vmax) / (double) 2;
	double midrange =  (vmax - vmin) / (double) 2;

	cairo_move_to(cr, x, y + h);

	for(size_t i = 0; i < ds_size; i++){
		size_t k   = (n/2) + (i * downsampler);

		double acc = 0;
		for(size_t j = 0; j < downsampler; j++) acc += ((k+j) >= n) ? 0.0 : data[k + j];
		acc = fmax(0.0, acc / (double) downsampler);

		double x_rpos = (logX) ? (log10(ds_size - i) / nLog10) : (1.0 - (i / (double) ds_size));
		double y_rpos = (logY) ? ((fabs(acc) > 1e-12) ? log10(acc) : 0x0) : acc;
		y_rpos = fmax(vmin, fmin(vmax, y_rpos)) - midpoint;
		y_rpos = y_rpos / midrange;

		double x_apos = x + (1 - x_rpos) * (w / (double) 2);
		double y_apos = y + (1 - y_rpos) * (h / (double) 2);
		cairo_line_to(cr, x_apos, y_apos);
	}

	for(size_t i = 0; i < ds_size; i++){
		size_t k   = (i * downsampler);

		double acc = 0;
		for(size_t j = 0; j < downsampler; j++) acc += ((k+j) >= n) ? 0.0 : data[k + j];
		acc = fmax(0.0, acc / (double) downsampler);

		double x_rpos = (logX) ? (log10(i + 1) / nLog10) : (i / (double) ds_size);
		double y_rpos = (logY) ? ((fabs(acc) > 1e-12) ? log10(acc) : 0x0) : acc;
		y_rpos = fmax(vmin, fmin(vmax, y_rpos)) - midpoint;
		y_rpos = y_rpos / midrange;

		double x_apos = x + (1 + x_rpos) * (w / (double) 2);
		double y_apos = y + (1 - y_rpos) * (h / (double) 2);
		cairo_line_to(cr, x_apos, y_apos);
	}
	
	cairo_line_to(cr, x + w, y + h);
	cairo_line_to(cr, x,     y + h);
	cairo_fill_preserve(cr);

	return LA_NO_ERROR;
}
