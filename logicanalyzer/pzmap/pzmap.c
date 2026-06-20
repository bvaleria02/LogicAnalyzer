#include "../liblogicanalyzer.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "stdint.h"
#include "pzmap.h"
#include "../laComplex/laComplex.h"
#include <math.h>

#define MARK_SIZE 8	
#define CIRCLE_COUNT 8
#define RADIUS_COUNT 8

LAErrorCode LAPZMapDraw(cairo_t *cr, LAComplex *zeros, const size_t zn, LAComplex *poles, const size_t pn, const size_t width, const size_t height, const size_t border){
	LA_HANDLE_NULLPTR(cr,		LA_PROPAGATE_ERROR);

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, 1);
	cairo_stroke(cr);

	double radius = ((width + height) / 4) - (border / 2);

	cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
	cairo_set_line_width(cr, 1);
	for(size_t i = 0; i < CIRCLE_COUNT; i++){
		cairo_arc(cr, width / 2, height / 2, ((i+1)*radius) /(double) (CIRCLE_COUNT + 1), 0, 2*M_PI);
		cairo_stroke(cr);
	}

	cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
	cairo_set_line_width(cr, 1);
	for(size_t i = 0; i < RADIUS_COUNT; i++){
		double theta = i * (M_PI / (double) RADIUS_COUNT);
		double x1 = (width/2)  + cos(theta) * radius;
		double y1 = (height/2) - sin(theta) * radius;
		double x2 = (width/2)  - cos(theta) * radius;
		double y2 = (height/2) + sin(theta) * radius;

		cairo_move_to(cr, x1, y1);
		cairo_line_to(cr, x2, y2);
		cairo_stroke(cr);
	}

	cairo_arc(cr, width / 2, height / 2, radius, 0, 2*M_PI);
	cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
	cairo_set_line_width(cr, 1);
	cairo_stroke(cr);

	double x = 0;
	double y = 0;

	cairo_set_source_rgb(cr, 0, 0, 1);
	cairo_set_line_width(cr, 1);
	for(size_t i = 0; i < zn; i++){
		x = (width / 2)  + (zeros[i].re * radius);
		y = (height / 2) - (zeros[i].im * radius);

		cairo_arc(cr, x, y, MARK_SIZE /(double) 2, 0, 2*M_PI);
		cairo_stroke(cr);
	}

	cairo_set_source_rgb(cr, 1, 0, 0);
	cairo_set_line_width(cr, 1);
	for(size_t i = 0; i < pn; i++){
		x = (width / 2)  + (poles[i].re * radius);
		y = (height / 2) - (poles[i].im * radius);

		cairo_move_to(cr, x - (MARK_SIZE /(double) 2), y - (MARK_SIZE /(double) 2));
		cairo_line_to(cr, x + (MARK_SIZE /(double) 2), y + (MARK_SIZE /(double) 2));
		cairo_stroke(cr);

		cairo_move_to(cr, x - (MARK_SIZE /(double) 2), y + (MARK_SIZE /(double) 2));
		cairo_line_to(cr, x + (MARK_SIZE /(double) 2), y - (MARK_SIZE /(double) 2));
		cairo_stroke(cr);
	}

	(void) zeros;
	(void) poles;
	return LA_NO_ERROR;
}
