#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "liblogicanalyzer.h"

void LAOkayWindow(GtkWidget *widget, LAZoomSetWindow *laz){
	double value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(laz->spin));
	lawp->rd.zoom = ((int) value);
	LACloseWindow(NULL, laz->window);
}

void LACreateZoomSetWindow(LAWindow *law, LAZoomSetWindow *laz){
	laz->isActive = 1;

	laz->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(laz->window), "Zoom Set");
	gtk_container_set_border_width(GTK_CONTAINER(laz->window), 8);

	laz->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(laz->window), GTK_WIDGET(laz->vbox));

	laz->label = gtk_label_new("Enter zoom (0 = default)");
	gtk_container_add(GTK_CONTAINER(laz->vbox), GTK_WIDGET(laz->label));
	
	GtkAdjustment *adjustment 	= gtk_adjustment_new(1, -128, 127, 1, 10, 0);
	laz->spin					= gtk_spin_button_new(adjustment, 1.0, 0);
	gtk_container_add(GTK_CONTAINER(laz->vbox), GTK_WIDGET(laz->spin));

	laz->hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(laz->vbox), GTK_WIDGET(laz->hbox));

	laz->okay 					= gtk_button_new_with_label("Apply");
	laz->cancel 				= gtk_button_new_with_label("Cancel");
	gtk_container_add(GTK_CONTAINER(laz->hbox), GTK_WIDGET(laz->okay));
	gtk_container_add(GTK_CONTAINER(laz->hbox), GTK_WIDGET(laz->cancel));

	g_signal_connect(laz->window, "destroy", G_CALLBACK(LATerminateZoomSetWindow), laz);
	g_signal_connect(laz->cancel, "clicked", G_CALLBACK(LACloseWindow), laz->window);
	g_signal_connect(laz->okay,   "clicked", G_CALLBACK(LAOkayWindow), laz);
	g_signal_connect(laz->spin,   "activate", G_CALLBACK(LAOkayWindow), laz);

	gtk_widget_show_all(laz->window);
	gtk_main();
}
