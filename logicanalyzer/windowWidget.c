#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "liblogicanalyzer.h"
#include "enums.h"
#include "error.h"
#include "utils.h"
#include "types.h"
#include "gtk_funcs.h"
#include "filter/windows.h"
#include "filter/filter.h"
#include "filterwindow.h"
#include "compiler.h"
#include "fixedpoint/fixedpoint.h"
#include <math.h>
#include "numericMethods/dft.h"
#include "numericMethods/fft.h"
#include "draw/fftfreq.h"

gboolean LAOnDrawWindowWidgetGraph(GtkWidget *w, cairo_t *cr, LAFilterWindowWidget *widget){
	LA_HANDLE_NULLPTR(w, TRUE);
	LA_HANDLE_NULLPTR(cr, TRUE);
	LA_HANDLE_NULLPTR(widget, TRUE);
	LAErrorCode code = LA_NO_ERROR;

	code = LAFilterGraphDrawBG(cr, GRAPH_MINI_WIDTH, GRAPH_MINI_HEIGHT, FALSE);
	if(code) return TRUE;

	size_t size = 256;
	double buffer[256];

	code = LAFilterGenerateWindowArray(buffer, size, widget->windowType, widget->paramValue, LA_FIR_FILTER_PARAMS);
	if(code) goto cleanup;

	code = LAFilterGraphDrawArray(cr, buffer, size, GRAPH_MINI_WIDTH, GRAPH_MINI_HEIGHT, -1.0, 1.0, TRUE);
	if(code) goto cleanup;

cleanup:
	return TRUE;
}

gboolean LAOnDrawWindowWidgetGraph2(GtkWidget *w, cairo_t *cr, LAFilterWindowWidget *widget){
	LA_HANDLE_NULLPTR(w, TRUE);
	LA_HANDLE_NULLPTR(cr, TRUE);
	LA_HANDLE_NULLPTR(widget, TRUE);
	LAErrorCode code = LA_NO_ERROR;

	code = LAFilterGraphDrawBG(cr, GRAPH_MINI_WIDTH, GRAPH_MINI_HEIGHT, FALSE);
	if(code) return TRUE;

	size_t size = 1024;
	double buffer[1024];
	for(size_t i = 0; i < size; i++) buffer[i] = 1;
	buffer[size / 2] = 1;

	double *buffer2 = NULL;
	size_t size2 = 0x0;
	/*LA_PROFILER(
		code = LADiscreteFTFortran(buffer, size, &buffer2, &size2, widget->windowType, widget->paramValue);,
		"Window Widget - Bilateral DFT <Fortan>"
	);*/
	LA_PROFILER(
		code = LAFFTWindow(buffer, size, &buffer2, &size2, widget->windowType, widget->paramValue);,
		"Window Widget - FFT <C>"
	);
	if(code) goto cleanup;
/*
	double maxValue = 0;
	for(size_t i = 0; i < size2; i++){
		if(fabs(buffer2[i]) < LA_EPS){
			buffer2[i] = -1000;
		} else {
			buffer2[i] = log10(fabs(buffer2[i]));
		}
		if(buffer2[i] > maxValue) maxValue = buffer2[i];
	}
*/
	double limits = 4;
	cairo_set_source_rgb(cr, 0.2, 0.4, 0.9);
	//code = LAFilterGraphDrawArray(cr, buffer2, size2, GRAPH_MINI_WIDTH, GRAPH_MINI_HEIGHT, -8, maxValue, FALSE);
	code = LADrawFFTBilateral(cr, 0, 0, GRAPH_MINI_WIDTH, GRAPH_MINI_HEIGHT, buffer2, size2, -limits, limits, 1, false, true);
	if(code) goto cleanup;

cleanup:
	if(buffer2 != NULL) free(buffer2);
	return TRUE;
}

gboolean LAOnFilterWindowWidgetParamChange(GtkWidget *w, LAFilterWindowWidget *widget){
	for(size_t i = 0; i < LA_FIR_FILTER_PARAMS; i++){
		widget->paramValue[i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget->paramSb[i]));
	}

	gtk_widget_queue_draw(widget->graph);
	gtk_widget_queue_draw(widget->graph2);
	if(widget->externWidget != NULL) gtk_widget_queue_draw(widget->externWidget);
	(void) w;
	return FALSE;
}

gboolean LAOnFilterWindowWidgetWindowChange(GtkWidget *w, LAFilterWindowWidget *widget){
	widget->windowType = gtk_combo_box_get_active(GTK_COMBO_BOX(widget->combobox));

	for(size_t i = 0; i < LA_FIR_FILTER_PARAMS; i++){
		if(LAWindowTypeDetails[widget->windowType].param[i].name != NULL){
			gtk_adjustment_configure(GTK_ADJUSTMENT(widget->paramAdj[i]),
					LAWindowTypeDetails[widget->windowType].param[i].value,
					LAWindowTypeDetails[widget->windowType].param[i].min,
					LAWindowTypeDetails[widget->windowType].param[i].max,
					LAWindowTypeDetails[widget->windowType].param[i].stepIncrement,
					LAWindowTypeDetails[widget->windowType].param[i].pageIncrement,
					0
				);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget->paramSb[i]),
					LAWindowTypeDetails[widget->windowType].param[i].value
				);
			gtk_spin_button_set_digits(GTK_SPIN_BUTTON(widget->paramSb[i]),
					LAWindowTypeDetails[widget->windowType].param[i].digits
				);
			gtk_label_set_text(GTK_LABEL(widget->paramLabel[i]), LAWindowTypeDetails[widget->windowType].param[i].name);
			gtk_widget_set_visible(widget->paramLabel[i], TRUE);
			gtk_widget_set_visible(widget->paramSb[i], TRUE);
		} else {
			gtk_widget_set_visible(widget->paramLabel[i], FALSE);
			gtk_widget_set_visible(widget->paramSb[i], FALSE);
		}
	}


	gtk_widget_queue_draw(widget->graph);
	gtk_widget_queue_draw(widget->graph2);
	if(widget->externWidget != NULL) gtk_widget_queue_draw(widget->externWidget);
	(void) w;
	return FALSE;
}

LAErrorCode LAWindowWidgetInit(LAFilterWindowWidget *widget){
	LA_HANDLE_NULLPTR(widget, 		LA_PROPAGATE_ERROR);

	widget->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);

	widget->label =  gtk_label_new("Window type:");
	LAFilterWindowComboBox(&(widget->combobox));
	widget->graph = gtk_drawing_area_new();
	gtk_widget_set_size_request(widget->graph, GRAPH_MINI_WIDTH, GRAPH_MINI_HEIGHT);
	widget->graph2 = gtk_drawing_area_new();
	gtk_widget_set_size_request(widget->graph2, GRAPH_MINI_WIDTH, GRAPH_MINI_HEIGHT);

	gtk_container_add(GTK_CONTAINER(widget->box), GTK_WIDGET(widget->label));
	gtk_container_add(GTK_CONTAINER(widget->box), GTK_WIDGET(widget->combobox));
	gtk_container_add(GTK_CONTAINER(widget->box), GTK_WIDGET(widget->graph));

	for(size_t i = 0; i < LA_FIR_FILTER_PARAMS; i++){
		widget->paramLabel[i]	= gtk_label_new("Parameter:");
		widget->paramAdj[i]		= gtk_adjustment_new(0, -32768, 32768, 0.01, 1, 0);
		widget->paramSb[i]		= gtk_spin_button_new(widget->paramAdj[i], 1, 2);
		gtk_container_add(GTK_CONTAINER(widget->box), GTK_WIDGET(widget->paramLabel[i]));
		gtk_container_add(GTK_CONTAINER(widget->box), GTK_WIDGET(widget->paramSb[i]));
	}

	gtk_container_add(GTK_CONTAINER(widget->box), GTK_WIDGET(widget->graph2));

	widget->windowType = 0;
	widget->externWidget = NULL;
	LAOnFilterWindowWidgetWindowChange(NULL, widget);

	return LA_NO_ERROR;
}

LAErrorCode LAWindowWidgetAdd(GtkWidget *container, LAFilterWindowWidget *widget){
	LA_HANDLE_NULLPTR(container, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(widget, 		LA_PROPAGATE_ERROR);

	gtk_container_add(GTK_CONTAINER(container), GTK_WIDGET(widget->box));

	return LA_NO_ERROR;
}

LAErrorCode LAWindowWidgetConnect(LAFilterWindowWidget *widget, GtkWidget *extWidget){
	LA_HANDLE_NULLPTR(widget, 		LA_PROPAGATE_ERROR);

	if(extWidget != NULL) widget->externWidget = extWidget;

	g_signal_connect(widget->graph,		"draw", 	G_CALLBACK(LAOnDrawWindowWidgetGraph), widget);
	g_signal_connect(widget->graph2,	"draw", 	G_CALLBACK(LAOnDrawWindowWidgetGraph2), widget);
	g_signal_connect(widget->combobox,	"changed", 	G_CALLBACK(LAOnFilterWindowWidgetWindowChange),	widget);

	for(size_t i = 0; i < LA_FIR_FILTER_PARAMS; i++){
		g_signal_connect(widget->paramSb[i],"value-changed", G_CALLBACK(LAOnFilterWindowWidgetParamChange), widget);
	}

	return LA_NO_ERROR;
}
