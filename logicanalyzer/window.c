#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "liblogicanalyzer.h"
#include <pthread.h>
#include <math.h>

void destroyWindow(GtkWidget *widget, gpointer *pointer){

	g_print("Closing device\n");
	LACloseDevice(lawp);
	g_print("Closing done\n");

	if(lawp->rd.renderSourceId != -1){
		g_source_remove(lawp->rd.renderSourceId);
	}

	gtk_main_quit();

	pthread_mutex_destroy(&(lawp->mutexes.bucketAccess));
	(void) widget;
	(void) pointer;
}

gboolean LAHandleMainKeyPress(GtkWidget *widget, GdkEventKey *event, LAWindow *law){
//	g_print("Key: %i\n", event->keyval);
	int response = FALSE;

	switch(event->keyval){
		case GDK_KEY_Left: 			LAMicroRewind(NULL, law);
									break;
		case GDK_KEY_Right: 		LAMicroAdvance(NULL, law);
									break;
		case GDK_KEY_Up: 			LAZoomMore(NULL, law);
									break;
		case GDK_KEY_KP_Subtract: 	LAZoomLess(NULL, law);
									break;
		case GDK_KEY_KP_Add: 		LAZoomMore(NULL, law);
									break;
		case GDK_KEY_Home: 			LAGoToStart(NULL, law);
									break;
		case GDK_KEY_End: 			LAGoToEnd(NULL, law);
									break;
		case GDK_KEY_Page_Down: 	LARewind(NULL, law);
									break;
		case GDK_KEY_Page_Up: 		LAAdvance(NULL, law);
									break;
		case GDK_KEY_F1: 			response = LACreateConnectWindow(NULL, law);
									break;
		case GDK_KEY_F2: 			response = LACreateCommandWindow(NULL, 0);
									break;
		case GDK_KEY_Control_L: 	response = FALSE;
									break;
	}

	return response;
}

void LAWindowCreate(LAWindow *law){
	law->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(law->window), "Logic Analyzer");
	g_signal_connect(law->window, "destroy", G_CALLBACK(destroyWindow), NULL);
	gtk_widget_set_size_request(law->window, 700, 200);
	gtk_container_set_border_width(GTK_CONTAINER(law->window), 0);

	law->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
	gtk_container_add(GTK_CONTAINER(law->window), GTK_WIDGET(law->vbox));

	//law->scrollableChannels = gtk_scrolled_window_new(NULL, NULL);
	//gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(law->scrollableChannels), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
	//gtk_widget_set_size_request(law->scrollableChannels, 800, 400);

	law->container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
	gtk_container_set_border_width(GTK_CONTAINER(law->container), 8);

	law->rd.dataOffset = 0;
	law->rd.scopeOffset = 0;
	law->rd.hasRenderFunctionAdded = 0;
	law->rd.renderSourceId = -1;
	law->rd.addDataOffset = 1;
	law->rd.showRelativeTime = 0;
	law->rd.showAbsoluteTime = 0;
	law->rd.showRelativeSample = 0;
	law->rd.showAbsoluteSample = 0;
	law->rd.showBufferEnd = 0;
	law->rd.showBufferRuler = 0;
	law->rd.showClockRuler = 0;
	law->rd.pollingTime = DEFAULT_POLLING_RATE;
	law->rd.virtualBufferSize = DEFAULT_VIRTUAL_BUFFER_SIZE;
	law->rd.sampleCounter = 0;
	law->connect.fd = -1;
	law->connect.device = NULL;
	law->connect.readThread = -1;
	law->connect.internalDeviceId = 0;
	law->windowUpdateThread = -1;

	law->bd.bucketStart = NULL;
	law->bd.bucketCurrent = NULL;
	law->bd.bucketWrite = 0;
	law->bd.hasStopped = 1;
	law->bd.dataMask = 0xFF;
	law->bd.dataMode = LA_RECORD_DATA_MODE_ALL;
	pthread_mutex_init(&(law->mutexes.bucketAccess), NULL);
	pthread_mutex_init(&(law->mutexes.dataBufferAccess), NULL);

	g_signal_connect(law->window, "key-press-event", G_CALLBACK(LAHandleMainKeyPress), law);
}

void LAWindowRun(LAWindow *law){
	gtk_widget_show_all(law->window);
	gtk_main();
}

void LATerminateWindow(GtkWidget *widget, GtkWidget **window){
	if(window == NULL){
		return;
	}

	if((*window) == NULL){
		g_print("Already terminated");
		return;
	}

	gtk_window_close(GTK_WINDOW((*window)));
	(*window) = NULL;

	gtk_main_quit();
}

void LATerminateZoomSetWindow(GtkWidget *widget, LAZoomSetWindow *laz){
	if(laz->window == NULL || laz->isActive == 0){
		g_print("Already closed\n");
		return;
	}

	g_print("IsActive = %i\n", laz->isActive);
	gtk_window_close(GTK_WINDOW(laz->window));
	gtk_main_quit();

	laz->window = NULL;
	laz->isActive = 0;
}

void LACloseWindow(GtkWidget *widget, GtkWidget *window){
	gtk_window_close(GTK_WINDOW(window));
}

double LAGetZoomMultiplier(LAWindow *law){
	return expf((law->rd.zoom) * 0.2);
}
