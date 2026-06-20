#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "liblogicanalyzer.h"
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <stdbool.h>

gboolean LAOnLabelSpinComboChange(GtkWidget *widget, LALabelSpinCombo *lal){
	lal->value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(lal->spinButton));

	if(lal->externalWidget != NULL) gtk_widget_queue_draw(lal->externalWidget);
	(void) widget;
	return TRUE;
}

LAErrorCode LALabelSpinComboSetMin(LALabelSpinCombo *lal, double min){
	LA_HANDLE_NULLPTR(lal, LA_PROPAGATE_ERROR);
	lal->min = min;
	gtk_adjustment_set_lower(GTK_ADJUSTMENT(lal->adjustment), min);
	return LA_NO_ERROR;
}

LAErrorCode LALabelSpinComboSetMax(LALabelSpinCombo *lal, double max){
	LA_HANDLE_NULLPTR(lal, LA_PROPAGATE_ERROR);
	lal->max = max;
	gtk_adjustment_set_upper(GTK_ADJUSTMENT(lal->adjustment), max);
	return LA_NO_ERROR;
}

LAErrorCode LALabelSpinComboSetVisibility(LALabelSpinCombo *lal, bool visibility){
	LA_HANDLE_NULLPTR(lal, LA_PROPAGATE_ERROR);

	gtk_widget_set_visible(lal->label, visibility);
	gtk_widget_set_visible(lal->spinButton, visibility);
	return LA_NO_ERROR;
}

LAErrorCode LALabelSpinComboInit(LALabelSpinCombo *lal, char *label, double value, double min, double max, double step, double page, size_t digits){
	LA_HANDLE_NULLPTR(lal,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(label,	LA_PROPAGATE_ERROR);

	lal->vbox 		= gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	lal->label 		= gtk_label_new(label);
	lal->adjustment = gtk_adjustment_new(value, min, max, step, page, 0);
	lal->spinButton = gtk_spin_button_new(GTK_ADJUSTMENT(lal->adjustment), 1, digits);

	gtk_container_add(GTK_CONTAINER(lal->vbox), GTK_WIDGET(lal->label));
	gtk_container_add(GTK_CONTAINER(lal->vbox), GTK_WIDGET(lal->spinButton));

	lal->value 	= value;
	lal->min	= min;
	lal->max	= max;
	lal->step	= step;
	lal->page	= page;
	lal->digits	= digits;
	lal->externalWidget = NULL;

	g_signal_connect(lal->spinButton, "value-changed", G_CALLBACK(LAOnLabelSpinComboChange), lal);
	
	return LA_NO_ERROR;
}

LAErrorCode LALabelSpinComboAdd(LALabelSpinCombo *lal, GtkWidget *container){
	LA_HANDLE_NULLPTR(lal,			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(container,	LA_PROPAGATE_ERROR);

	gtk_container_add(GTK_CONTAINER(container), GTK_WIDGET(lal->vbox));

	return LA_NO_ERROR;
}

LAErrorCode LALabelSpinComboConnect(LALabelSpinCombo *lal, GtkWidget *widget){
	LA_HANDLE_NULLPTR(lal,			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(widget,	LA_PROPAGATE_ERROR);

	lal->externalWidget = widget;

	return LA_NO_ERROR;
}
