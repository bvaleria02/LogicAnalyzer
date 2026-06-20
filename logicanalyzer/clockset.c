#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "liblogicanalyzer.h"

void LAOkayZoomSetWindow(GtkWidget *widget, LAZoomSetWindow *laz){
	double value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(laz->spin));
	lawp->rd.zoom = ((int) value);
	(void) widget;
}

void LACreateZoomSetWindow(LAWindow *law, LAZoomSetWindow *laz){
	laz->isActive = 1;

	laz->window = gtk_dialog_new_with_buttons(
					"Zoom set",
					GTK_WINDOW(law->window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					"_Apply", GTK_RESPONSE_ACCEPT,
					"_Quit", GTK_RESPONSE_CANCEL,
					NULL
				);

	gtk_window_set_modal(GTK_WINDOW(laz->window), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(laz->window), 8);
	laz->content = gtk_dialog_get_content_area(GTK_DIALOG(laz->window));

	laz->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(laz->content), GTK_WIDGET(laz->vbox));

	laz->label = gtk_label_new("Enter zoom (0 = default)");
	gtk_container_add(GTK_CONTAINER(laz->vbox), GTK_WIDGET(laz->label));
	
	GtkAdjustment *adjustment 	= gtk_adjustment_new(0, -128, 127, 1, 10, 0);
	laz->spin					= gtk_spin_button_new(adjustment, 1.0, 0);
	gtk_container_add(GTK_CONTAINER(laz->vbox), GTK_WIDGET(laz->spin));

	g_signal_connect(laz->spin,   "activate", G_CALLBACK(LAOkayZoomSetWindow), laz);

	gtk_widget_show_all(laz->window);

	int response = gtk_dialog_run(GTK_DIALOG(laz->window));
	if(response == GTK_RESPONSE_ACCEPT){
		LAOkayZoomSetWindow(NULL, laz);
	}

	gtk_widget_destroy(laz->window);
}
