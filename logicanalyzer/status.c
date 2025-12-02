#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "liblogicanalyzer.h"

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <pthread.h>
#include <stdatomic.h>

void LAPlaceStatusBar(LAWindow *law){
	LAStatus *status = &(law->status);

	status->hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);

	status->pollingTime			= gtk_label_new("A");
	status->writeEnable			= gtk_label_new("A");
	status->virtualBufferSize	= gtk_label_new("A");
	status->capture				= gtk_label_new("A");
	status->device				= gtk_label_new("A");
	status->scopeWrite			= gtk_label_new("A");
	status->scopeScroll			= gtk_label_new("A");
	status->zoom				= gtk_label_new("A");


	gtk_container_add(GTK_CONTAINER(status->hbox), status->pollingTime);
	gtk_container_add(GTK_CONTAINER(status->hbox), status->writeEnable);
	gtk_container_add(GTK_CONTAINER(status->hbox), status->virtualBufferSize);
	gtk_container_add(GTK_CONTAINER(status->hbox), status->capture);
	gtk_container_add(GTK_CONTAINER(status->hbox), status->device);
	gtk_container_add(GTK_CONTAINER(status->hbox), status->scopeWrite);
	gtk_container_add(GTK_CONTAINER(status->hbox), status->scopeScroll);
	gtk_container_add(GTK_CONTAINER(status->hbox), status->zoom);

	gtk_container_add(GTK_CONTAINER(law->vbox), status->hbox);
	LAUpdateStatusBar(law);
}

void LAUpdateStatusBar(LAWindow *law){
	char buffer[LA_SMALL_BUFFER_SIZE];
	LAStatus *status = &(law->status);
	
	snprintf(buffer, LA_SMALL_BUFFER_SIZE, "Polling time: %i μs", law->rd.pollingTime);
	gtk_label_set_text(GTK_LABEL(status->pollingTime), buffer);
/*
	if(law->rd.readEn)	
	gtk_label_set_text(GTK_LABEL(status->pollingTime), buffer);
*/
	snprintf(buffer, LA_SMALL_BUFFER_SIZE, "Capture size: %i samples", law->rd.virtualBufferSize);
	gtk_label_set_text(GTK_LABEL(status->virtualBufferSize), buffer);

	if(law->bd.bucketWrite == 1){
		gtk_label_set_text(GTK_LABEL(status->capture), "Capture: RUNNING");
	} else if(law->bd.bucketWrite == 0 && law->bd.hasStopped == 0){
		gtk_label_set_text(GTK_LABEL(status->capture), "Capture: PAUSED");
	} else if(law->bd.bucketWrite == 0 && law->bd.hasStopped == 1){
		gtk_label_set_text(GTK_LABEL(status->capture), "Capture: IDLE");
	}

	if(law->connect.fd < 0){
		gtk_label_set_text(GTK_LABEL(status->device), "Device: Not set");
	} else {
		gtk_label_set_text(GTK_LABEL(status->device), "Device: Connected");
	}

	if(law->rd.dontWrite == 0){
		gtk_label_set_text(GTK_LABEL(status->scopeWrite), "Scope write: Enabled");
	} else {
		gtk_label_set_text(GTK_LABEL(status->scopeWrite), "Scope write: Disabled");
	}

	if(law->rd.addDataOffset == 0){
		gtk_label_set_text(GTK_LABEL(status->scopeScroll), "Scope scroll: Disabled");
	} else {
		gtk_label_set_text(GTK_LABEL(status->scopeScroll), "Scope scroll: Enabled");
	}

	double zoom = LAGetZoomMultiplier(law);
	snprintf(buffer, LA_SMALL_BUFFER_SIZE, "Zoom: %.2lf%%", zoom * 100);
	gtk_label_set_text(GTK_LABEL(status->zoom), buffer);
}
