#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../liblogicanalyzer.h"
#include "dataLoader.h"
#include <time.h>
#include <pthread.h>
#include <math.h>


gboolean LAOnDataLoaderClampInstChanged(GtkWidget *widget, LADataLoaderWindow *lad){
	gtk_widget_queue_draw(lad->clampGraph);
	(void) widget;
	return TRUE;
}

LAErrorCode LADataLoaderComboBoxClampInstFiller(GtkWidget *widget){
	LA_HANDLE_NULLPTR(widget, LA_PROPAGATE_ERROR);

	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Ramp (Transfer curve)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Impulse");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Step");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Sine");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Pulse");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), "Noise");
	gtk_combo_box_set_active(GTK_COMBO_BOX(widget), 0);

	return LA_NO_ERROR;
}

LAErrorCode LAClampInstCreate(double *x, size_t size, double min, double max, size_t inst){
	LA_HANDLE_NULLPTR(x, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	switch(inst){
		case LA_CLAMP_INST_RAMP		: 	code = LALinspace(x, min, max, size);
										break;
		case LA_CLAMP_INST_IMPULSE	: 	code = LAClampInstImpulse(x, size, min, max);
										break;
		case LA_CLAMP_INST_STEP		: 	code = LAClampInstStep(x, size, min, max);
										break;
		case LA_CLAMP_INST_SINE		: 	code = LAClampInstSine(x, size, min, max);
										break;
		case LA_CLAMP_INST_PULSE	: 	code = LAClampInstPulse(x, size, min, max);
										break;
		case LA_CLAMP_INST_NOISE	: 	code = LAClampInstNoise(x, size, min, max);
										break;
		default						:	code = LALinspace(x, min, max, size);
										break;
	}

	return code;
}

static inline double midpoint(double a, double b){
	return (b + a) / (double) 2;
}

static inline double midsize(double a, double b){
	return (b - a) / (double) 2;
}

LAErrorCode LAClampInstImpulse(double *x, size_t size, double min, double max){
	LA_HANDLE_NULLPTR(x, LA_PROPAGATE_ERROR);

	//double p = midpoint(min, max);
	//double q = midsize(min, max);

	for(size_t i = 0; i < size; i++){
		if(i == (size / 2)){
			x[i] = max;
		} else {
			x[i] = min;
		}
	}

	return LA_NO_ERROR;
}

LAErrorCode LAClampInstStep(double *x, size_t size, double min, double max){
	LA_HANDLE_NULLPTR(x, LA_PROPAGATE_ERROR);

	//double p = midpoint(min, max);
	//double q = midsize(min, max);

	for(size_t i = 0; i < size; i++){
		if(i < (size / (double) 2)){
			x[i] = min;
		} else {
			x[i] = max;
		}
	}

	return LA_NO_ERROR;
}

LAErrorCode LAClampInstSine(double *x, size_t size, double min, double max){
	LA_HANDLE_NULLPTR(x, LA_PROPAGATE_ERROR);

	double p = midpoint(min, max);
	double q = midsize(min, max);

	for(size_t i = 0; i < size; i++){
		x[i] = q * sin(2 * M_PI * i / (double) size) + p;
	}

	return LA_NO_ERROR;
}

LAErrorCode LAClampInstPulse(double *x, size_t size, double min, double max){
	LA_HANDLE_NULLPTR(x, LA_PROPAGATE_ERROR);

	//double p = midpoint(min, max);
	//double q = midsize(min, max);
	double n = 0;

	for(size_t i = 0; i < size; i++){
		n = (i / (double) size);

		if((4*n - floor(4*n)) < 0.5){
			x[i] = min;
		} else {
			x[i] = max;
		}
	}

	return LA_NO_ERROR;
}


LAErrorCode LAClampInstNoise(double *x, size_t size, double min, double max){
	LA_HANDLE_NULLPTR(x, LA_PROPAGATE_ERROR);

	double p = midpoint(min, max);
	double q = midsize(min, max);
	
	double v = 0;

	for(size_t i = 0; i < size; i++){
		if((i & 0x7) == 0){
			v = q * ((2 * rand()) / (double) RAND_MAX) + p;
		}
		x[i] = v;
	}

	return LA_NO_ERROR;
}

