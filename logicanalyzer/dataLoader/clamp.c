#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include "../liblogicanalyzer.h"
#include "dataLoader.h"
#include "../numericMethods/dft.h"

#define CLAMP_PADDING 0.3

LADataLoaderClampType LAClampDetails[LA_CLAMP_TYPE_COUNT] = {
	{"No clamping", {
		{NULL,				-1,		0,		0,		0,		0,		0},
		{NULL,				1,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampNoClamping
	},
	{"Hard clip", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampHardClipping
	},
	{"Soft clip", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Intensity",		1,		-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampSoftClipping
	},
	{"Triangle folding", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Intensity",		1,		-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampHardWavefolding
	},
	{"Sine folding", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Intensity",		1,		-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampSoftWavefolding
	},
	{"Modulo", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Intensity",		1,		-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampModulo
	},
	{"Soft sign", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Intensity",		1,		-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampSoftSign
	},
	{"Cubic Clamping", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Intensity",		1,		-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampCubicClamping
	},
	{"Simple foldover", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Intensity",		1,		-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampSimpleFoldover
	},
	{"Double cusp fold", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Intensity",		1,		-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampDoubleCuspFold
	},
	{"Exponential fold", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Intensity",		1,		-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampExponentialFold
	},
	{"Partial wrap", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Intensity",		1,		-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampPartialWrap
	},
	{"Symmetrical Logistic", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Intensity",		1,		-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampSymmetricalLogistic
	},
	{"S curve", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Intensity",		1,		-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampSCurve
	},
	{"Exponential Gate", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Intensity",		1,		-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampExponentialGate
	},
	{"Soft knee", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Intensity",		1,		-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampSoftKnee
	},
	{"Saturated logistic map", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Intensity",		1,		-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_SLOW,
		LAClampSaturatedLogisticMap
	},
	{"Tent map", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"μ",				0.5,	-32768,	32767,	0.0001,	0.1,	4},
		{"y0",				0,   	-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_SLOW,
		LAClampTentMap
	},
	{"BJT-like saturation", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Intensity",		1,		-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampBJTSaturation
	},
	{"Slew rate saturation", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Slew Rate (v/sample)", 1,		-32768,	32767,	0.00001,	0.01,	5},
		{"Initial value",	0,		-32768,	32767,	0.0001,	0.1,	4},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_SLOW,
		LAClampSlewRateSaturation
	},
	{"Schmitt trigger", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Threshold", 		1,		 0,		1,		0.00001,0.01,	5},
		{"Initial value",	0,		 0,		1,	   	1,		1,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_SLOW,
		LAClampSchmittTrigger
	},
	{"RC Low pass", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"R", 				1000,	0,		1e12,	1,	    1e3,	3},
		{"C",				1e-9,	0,		1e3,	1e-12,	1e-6,	12},
		{"V_0",				0,		-32768,	32767,	0.01,	1,		2}},
		LA_CLAMP_FLAG_SLOW,
		LAClampRCLowPass
	},
	{"Symmetrical diode simulation", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"I_S", 			1e-9,	 1e-12,	32767,	1e-12,  1e-9,	12},
		{"k",				0.026,	 0,		32767,	1e-3,	1e-1,	3},
		{NULL,				0,		 0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampSymmetricalDiode
	},
	{"Unilateral diode simulation", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"I_S", 			1e-9,	 1e-12,	32767,	1e-12,  1e-9,	12},
		{"k",				0.026,	 0,		32767,	1e-3,	1e-1,	3},
		{NULL,				0,		 0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampUnilateralDiode
	},
	{"Differenciator", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"y0", 				0,	 	-32768, 32767,	1e-3,   1,	    3},
		{"h",				1,	 	 1e-6,	32767,	1e-6,	1e-3,	6},
		{NULL,				0,		 0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_SLOW,
		LAClampDifferenciator
	},
	{"Integrator", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"y0", 				0,	 	-32768, 32767,	1e-3,   1,	    3},
		{"h",				1,	 	 1e-6,	32767,	1e-6,	1e-3,	6},
		{NULL,				0,		 0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_SLOW,
		LAClampIntegrator
	},
	{"Stochastic", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{NULL,				0,		 0,		0,		0,		0,		0},
		{NULL,				0,		 0,		0,		0,		0,		0},
		{NULL,				0,		 0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_SLOW,
		LAClampStochastic
	},
	{"Even Power Limiter", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"k", 				1,	 	 1,     32767,	1,   	2,	    0},
		{NULL,				0,		 0,		0,		0,		0,		0},
		{NULL,				0,		 0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_SLOW,
		LAClampEvenPowerLimiter
	},
	{"Even Power Folder", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"k", 				1,	 	 1,     32767,	1,   	2,	    0},
		{NULL,				0,		 0,		0,		0,		0,		0},
		{NULL,				0,		 0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_SLOW,
		LAClampEvenPowerFolder
	},
	{"N-root limiter", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"k", 				3,	 	 0,     32767,	1e-3,   1,	    3},
		{NULL,				0,		 0,		0,		0,		0,		0},
		{NULL,				0,		 0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampNroot
	},
	{"Soft abs", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"k", 				1,	 	-32768, 32767,	1e-3,   1,	    3},
		{NULL,				0,		 0,		0,		0,		0,		0},
		{NULL,				0,		 0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampSoftAbs
	},
	{"Root sine foldover", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"k", 				1,	 	 0, 32767,	1,  	5,	    0},
		{NULL,				0,		 0,		0,		0,		0,		0},
		{NULL,				0,		 0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampRootSine
	},
	{"Gaussian hard clip 1", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"k", 				1,	 	-32768, 32767,	1e-3,   1,	    3},
		{NULL,				0,		 0,		0,		0,		0,		0},
		{NULL,				0,		 0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampHardGauss1
	},
	{"Gaussian hard clip 2", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"k", 				1,	 	-32768, 32767,	1e-3,   1,	    3},
		{NULL,				0,		 0,		0,		0,		0,		0},
		{NULL,				0,		 0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampHardGauss2
	},
	{"Bit crusher", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Bits", 			4,	 	 1, 	32767,	1,   	2,	    0},
		{NULL,				0,		 0,		0,		0,		0,		0},
		{NULL,				0,		 0,		0,		0,		0,		0}},
		LA_CLAMP_FLAG_FAST,
		LAClampBitCrush
	},
	{"Transient limiter", {
		{"Min",				-1,		-32768,	32767,	0.001,	1,		3},
		{"Max",				1,		-32768,	32767,	0.001,	1,		3},
		{"Intensity", 		1,	    -32768, 32767,	1e-3,   1e-1,	3},
		{"Start value (y0)",0,	    -32768, 32767,	1e-3,   1e-1,	3},
		{"h", 				1,	    -32768, 32767,	1e-6,   1e-3,	6}},
		LA_CLAMP_FLAG_SLOW,
		LAClampTransientLimiter
	},
};

LAErrorCode LADataLoaderComboBoxClampFiller(GtkWidget *widget){
	LA_HANDLE_NULLPTR(widget, LA_PROPAGATE_ERROR);

	for(size_t i = 0; i < LA_CLAMP_TYPE_COUNT; i++){
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), LAClampDetails[i].name);
	}

	gtk_combo_box_set_active(GTK_COMBO_BOX(widget), 0);

	return LA_NO_ERROR;
}

gboolean LAOnDataLoaderClampChanged(GtkWidget *widget, LADataLoaderWindow *lad){
	size_t index = gtk_combo_box_get_active(GTK_COMBO_BOX(lad->clampComboBox));
	//g_print("Index: %li\n", index);

	for(size_t i = 0; i < LA_CLAMP_PARAMETER_COUNT; i++){
		if(LAClampDetails[index].param[i].name != NULL){
			gtk_adjustment_configure(GTK_ADJUSTMENT(lad->clampParam[i].adjustment),
					LAClampDetails[index].param[i].value,
					LAClampDetails[index].param[i].min,
					LAClampDetails[index].param[i].max,
					LAClampDetails[index].param[i].stepIncrement,
					LAClampDetails[index].param[i].pageIncrement,
					0
				);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(lad->clampParam[i].spinButton),
					LAClampDetails[index].param[i].value
				);
			gtk_spin_button_set_digits(GTK_SPIN_BUTTON(lad->clampParam[i].spinButton),
					LAClampDetails[index].param[i].digits
				);
			gtk_label_set_text(GTK_LABEL(lad->clampParam[i].label), LAClampDetails[index].param[i].name);
			gtk_widget_set_visible(lad->clampParam[i].label, TRUE);
			gtk_widget_set_visible(lad->clampParam[i].spinButton, TRUE);

		} else {
			gtk_widget_set_visible(lad->clampParam[i].label, FALSE);
			gtk_widget_set_visible(lad->clampParam[i].spinButton, FALSE);
		}
	}

	gtk_widget_queue_draw(lad->clampGraph);
	gtk_widget_queue_draw(lad->clampGraphInfo);
	(void) widget;
	return TRUE;
}

LAErrorCode LADataLoaderClampDrawBg(cairo_t *cr){
	LA_HANDLE_NULLPTR(cr, LA_PROPAGATE_ERROR);

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);
	cairo_stroke(cr);

	cairo_set_line_width(cr, 1);
	cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);
	cairo_move_to(cr, CLAMP_GRAPH_WIDTH / 2, 0);
	cairo_line_to(cr, CLAMP_GRAPH_WIDTH / 2, CLAMP_GRAPH_HEIGHT);
	cairo_stroke(cr);
	cairo_move_to(cr, 0, CLAMP_GRAPH_HEIGHT / 2);
	cairo_line_to(cr, CLAMP_GRAPH_WIDTH, CLAMP_GRAPH_HEIGHT / 2);
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_rectangle(cr, 0, 0, CLAMP_GRAPH_WIDTH, CLAMP_GRAPH_HEIGHT);
	cairo_stroke(cr);

	return LA_NO_ERROR;
}

LAErrorCode LADataLoaderClampDFTDrawBg(cairo_t *cr){
	LA_HANDLE_NULLPTR(cr, LA_PROPAGATE_ERROR);

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_rectangle(cr, 0, 0, CLAMP_GRAPH_WIDTH, CLAMP_GRAPH_HEIGHT);
	cairo_stroke(cr);

	return LA_NO_ERROR;
}

LAErrorCode LAClampEvaluate(double *x, size_t sizeX, double *y, size_t sizeY, size_t clampType, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params,	LA_PROPAGATE_ERROR);

	if(clampType >= LA_CLAMP_TYPE_COUNT){
		// If invalid, pass
	} else {
		LAClampDetails[clampType].callable(x, sizeX, y, sizeY, params, paramCount);
	}

	return LA_NO_ERROR;
}

LAErrorCode LAClampDrawGraph(cairo_t *cr, double x1, double x2, double *x, size_t size){
	LA_HANDLE_NULLPTR(cr, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(x,  LA_PROPAGATE_ERROR);

	double limitY = (fabs(x2) > fabs(x1)) ? fabs(x2) : fabs(x1);

	double x0 = 0;
	double y0 = 0;
	cairo_set_line_width(cr, 2);

	for(size_t i = 0; i < size; i++){
		x0 = CLAMP_GRAPH_WIDTH * (i / (double) (size - 1));
		y0 = (CLAMP_GRAPH_HEIGHT / (double) 2) - CLAMP_GRAPH_HEIGHT * CLAMP_PADDING * (x[i] / (double) limitY);
		
		if(i == 0)	cairo_move_to(cr, x0, y0);
		else 		cairo_line_to(cr, x0, y0);
	}

	cairo_stroke(cr);

	return LA_NO_ERROR;
}

LAErrorCode LAClampDrawLimits(cairo_t *cr, double x1, double x2, double y1, double y2){
	LA_HANDLE_NULLPTR(cr, LA_PROPAGATE_ERROR);

	double limitY = (fabs(x2) > fabs(x1)) ? fabs(x2) : fabs(x1);
	double yl1 = (CLAMP_GRAPH_HEIGHT / (double) 2) - CLAMP_GRAPH_HEIGHT * CLAMP_PADDING * (y1 / (double) limitY);
	double yl2 = (CLAMP_GRAPH_HEIGHT / (double) 2) - CLAMP_GRAPH_HEIGHT * CLAMP_PADDING * (y2 / (double) limitY);

	cairo_set_line_width(cr, 2);

	cairo_set_source_rgb(cr, 0.9, 0.9, 0.99);
	cairo_rectangle(cr, 0, 0, CLAMP_GRAPH_WIDTH, yl2);
	cairo_fill(cr);
	cairo_stroke(cr);
	cairo_set_source_rgb(cr, 0.75, 0.75, 0.9);
	cairo_move_to(cr, 0, yl2);
	cairo_line_to(cr, CLAMP_GRAPH_WIDTH, yl2);
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 0.9, 0.9, 0.99);
	cairo_rectangle(cr, 0, yl1, CLAMP_GRAPH_WIDTH, CLAMP_GRAPH_HEIGHT - yl1);
	cairo_fill(cr);
	cairo_stroke(cr);
	cairo_set_source_rgb(cr, 0.75, 0.75, 0.9);
	cairo_move_to(cr, 0, yl1);
	cairo_line_to(cr, CLAMP_GRAPH_WIDTH, yl1);
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_rectangle(cr, 0, 0, CLAMP_GRAPH_WIDTH, CLAMP_GRAPH_HEIGHT);
	cairo_stroke(cr);
	
	return LA_NO_ERROR;
}

gboolean LAOnDataLoaderClampDraw(GtkWidget *widget, cairo_t *cr, LADataLoaderWindow *lad){
	LAErrorCode code = LA_NO_ERROR;

	code = LADataLoaderClampDrawBg(cr);
	if(code) return TRUE;

	double x[256];
	double y[256];
	size_t size = 256;
	double minValue = lad->clampParam[0].value;
	double maxValue = lad->clampParam[1].value;
	double limits = (fabs(maxValue) > fabs(minValue)) ? fabs(maxValue) : fabs(minValue);
	limits *= lad->clampGain.value;

	int clampInstValue = gtk_combo_box_get_active(GTK_COMBO_BOX(lad->clampInstComboBox));
	code = LAClampInstCreate(x, size, -limits, limits, clampInstValue);
	if(code) return TRUE;

	code = LAClampDrawLimits(cr, -limits, limits, minValue, maxValue);
	if(code) return TRUE;

	//code = LALinspace(x, -limits, limits, size);
	cairo_set_source_rgb(cr, 0.2, 0.6, 0.9);
	code = LAClampDrawGraph(cr, -limits, limits, x, size);
	if(code) return TRUE;

	double params[LA_CLAMP_PARAMETER_COUNT];
	for(size_t i = 0; i < LA_CLAMP_PARAMETER_COUNT; i++) params[i] = lad->clampParam[i].value;
	size_t clampType = gtk_combo_box_get_active(GTK_COMBO_BOX(lad->clampComboBox));

	code = LAClampEvaluate(x, size, y, size, clampType, params, LA_CLAMP_PARAMETER_COUNT);
	if(code) return TRUE;

	cairo_set_source_rgb(cr, 0.9, 0.7, 0.3);
	code = LAClampDrawGraph(cr, -limits, limits, y, size);
	if(code) return TRUE;

	gtk_widget_queue_draw(lad->clampGraphDFT);
	(void) widget;
	return TRUE;
}

LAErrorCode LAClampDFTDrawGraph(cairo_t *cr, double *w, size_t size){
	LA_HANDLE_NULLPTR(cr,	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(w,	LA_PROPAGATE_ERROR);

	double max = 0;
	double min = 0;
	for(size_t i = 0; i < size; i++){
		if(w[i] > max) max = w[i];
		if(w[i] < min) min = w[i];
	}

	double x = 0;
	double y = 0;

	cairo_move_to(cr, 0, CLAMP_GRAPH_HEIGHT);
	for(size_t i = 0; i < size; i++){
		x = CLAMP_GRAPH_WIDTH * (i / (double) size);
		y = CLAMP_GRAPH_HEIGHT * (1 - ((w[i] - min) / (double) (max - min)));
		cairo_line_to(cr, x, y);
	}
	cairo_line_to(cr, CLAMP_GRAPH_WIDTH, CLAMP_GRAPH_HEIGHT);
	cairo_line_to(cr, 0, CLAMP_GRAPH_HEIGHT);
	cairo_fill(cr);
	cairo_stroke(cr);

	return LA_NO_ERROR;
}

gboolean LAOnDataLoaderClampDFTDraw(GtkWidget *widget, cairo_t *cr, LADataLoaderWindow *lad){
	LAErrorCode code = LA_NO_ERROR;

	code = LADataLoaderClampDFTDrawBg(cr);
	if(code) return TRUE;

	double x[256];
	double y[256];
	size_t size = 256;
	double minValue = lad->clampParam[0].value;
	double maxValue = lad->clampParam[1].value;
	double limits = (fabs(maxValue) > fabs(minValue)) ? fabs(maxValue) : fabs(minValue);
	limits *= lad->clampGain.value;

	code = LAClampInstCreate(x, size, -limits, limits, LA_CLAMP_INST_SINE);
	if(code) return TRUE;

	double params[LA_CLAMP_PARAMETER_COUNT];
	for(size_t i = 0; i < LA_CLAMP_PARAMETER_COUNT; i++) params[i] = lad->clampParam[i].value;
	size_t clampType = gtk_combo_box_get_active(GTK_COMBO_BOX(lad->clampComboBox));

	code = LAClampEvaluate(x, size, y, size, clampType, params, LA_CLAMP_PARAMETER_COUNT);
	if(code) return TRUE;

	double y2[128];
	size_t size2 = 128;
	double params2[LA_FIR_FILTER_PARAMS] = {0, 0, 0};
	LA_PROFILER(
		//code = LAUnilateralDiscreteFT(y, size, y2, NULL, size2, LA_FILTER_WINDOW_RECTANGULAR, params2);,
		code = LAUnilateralDiscreteFTFortran(y, size, y2, size2, LA_FILTER_WINDOW_RECTANGULAR, params2);,
		"Data loader - Unilateral DFT <C>"
	);
	if(code) return TRUE;

	for(size_t i = 0; i < size2; i++){
		y2[i] = (y2[i] > LA_EPS) ? 20 * log10(y2[i]) : 20 * log10(LA_EPS);
	//	printf("i: %li\tw: %lf\n", i, y2[i]);
	}

	cairo_set_source_rgb(cr, 0.2, 0.7, 0.8);
	code = LAClampDFTDrawGraph(cr, y2, size2);
	if(code) return TRUE;

	(void) widget;
	return TRUE;
}

LAErrorCode LADataLoaderClampInfoSlow(LADataLoaderWindow *lad, cairo_t *cr){
	LA_HANDLE_NULLPTR(lad, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(cr, 	LA_PROPAGATE_ERROR);

	cairo_set_source_rgb(cr, 0.982, 0.646, 0.646);
	cairo_paint(cr);

	cairo_set_source_rgb(cr, 0.896, 0.253, 0.253);

	cairo_set_font_size(cr, 16);
	cairo_move_to(cr, 12, 20);
	cairo_show_text(cr, "Caution: sequential");

	cairo_set_font_size(cr, 10);
	cairo_move_to(cr, 12, 40);
	cairo_show_text(cr, "This clamp is not vectorized");
	cairo_move_to(cr, 12, 50);
	cairo_show_text(cr, "Expect it to be 4x slower due to sequential loop computation");

	cairo_rectangle(cr, 0, 0, CLAMP_INFO_GRAPH_WIDTH, CLAMP_INFO_GRAPH_HEIGHT);
	cairo_stroke(cr);

	return LA_NO_ERROR;
}

LAErrorCode LADataLoaderClampInfoFast(LADataLoaderWindow *lad, cairo_t *cr){
	LA_HANDLE_NULLPTR(lad, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(cr, 	LA_PROPAGATE_ERROR);

	cairo_set_source_rgb(cr, 0.646, 0.982, 0.646);
	cairo_paint(cr);

	cairo_set_source_rgb(cr, 0.216, 0.656, 0.216);

	cairo_set_font_size(cr, 16);
	cairo_move_to(cr, 12, 20);
	cairo_show_text(cr, "This function is vectorized");

	cairo_set_font_size(cr, 10);
	cairo_move_to(cr, 12, 40);
	cairo_show_text(cr, "This clamp uses AVX/SSE instructions if available.");
	cairo_move_to(cr, 12, 50);
	cairo_show_text(cr, "This offer up to 4x speed improvement, compared to sequential");

	cairo_rectangle(cr, 0, 0, CLAMP_INFO_GRAPH_WIDTH, CLAMP_INFO_GRAPH_HEIGHT);
	cairo_stroke(cr);

	return LA_NO_ERROR;
}

gboolean LAOnDataLoaderClampInfoDraw(GtkWidget *widget, cairo_t *cr, LADataLoaderWindow *lad){
	LAErrorCode code = LA_NO_ERROR;

	size_t clampType = gtk_combo_box_get_active(GTK_COMBO_BOX(lad->clampComboBox));
	if((LAClampDetails[clampType].flags & 0x1) == LA_CLAMP_FLAG_FAST) {
		code = LADataLoaderClampInfoFast(lad, cr);
	} else {
		code = LADataLoaderClampInfoSlow(lad, cr);
	}
	if(code) return TRUE;

	(void) widget;
	return TRUE;
}
