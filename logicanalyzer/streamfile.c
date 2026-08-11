#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <stdatomic.h>

#include "liblogicanalyzer.h"
#include "error.h"
#include "utils.h"
#include "types.h"
#include "compiler.h"
#include "serial.h"
#include "gtk_funcs.h"
#include "dialog.h"

typedef struct {
	GtkWidget *window;
	GtkWidget *content;
	GtkWidget *vbox;
	GtkWidget *hboxTitle;
	GtkWidget *titleTitle;
	GtkWidget *titleFilename;

	GtkWidget *hboxControls;
	GtkWidget *controlsPlay;
	GtkWidget *controlsPause;
	GtkWidget *controlsSendOne;
	GtkWidget *controlsRewind;

	pthread_t sendThread;
	uint8_t closeThread;
	LAMappedFile *file;
	uint32_t offset;
	uint8_t playback;
} LAFileStreamer;


void *LAThreadFileSender(void *vlas){
	LAFileStreamer *las = (LAFileStreamer *)vlas;

	const uint8_t scale = 1;
	struct timespec ts = {0};
	ts.tv_nsec = 1000000000 / (double) (scale * 32258.06452 / (double) (256 * 4));

	uint8_t buffer[256];
	uint32_t size = 0;
	LASerialV2Protocol p;

	LAPrepareProtocolV2Basic(&p, LA_COMMAND_TEST_DAC_MODE, 2);
	LASendSerialV2(lawp, &p);

	LAPrepareProtocolV2Basic(&p, LA_COMMAND_TEST_DAC_SPEED, 31 / scale);
	LASendSerialV2(lawp, &p);

	while(1){
		if(atomic_load(&(las->closeThread)) != 0) break;

		g_print("Hello %i\n", las->offset);
		clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);

		if(atomic_load(&(las->playback)) == 0) continue;
		
		size = 0;
		for(uint16_t i = 0; i < 256; i++){
			if((las->offset+i) >= las->file->length) break;
			buffer[i] = las->file->data[las->offset+i];
			size++;
		}

		atomic_fetch_add(&(las->offset), size);

		if(size != 0){
			LAPrepareProtocolV2Stream(&p, buffer, size);
			LASendSerialV2(lawp, &p);
		}
	}

	g_print("Closing thread\n");
	return NULL;
}

void LAFileSenderRewind(GtkWidget *widget, LAFileStreamer *las){
	atomic_store(&(las->offset), 0);
	(void) widget;
}

void LAFileSenderPlay(GtkWidget *widget, LAFileStreamer *las){
	LASerialV2Protocol p;
	LAPrepareProtocolV2Basic(&p, LA_COMMAND_TEST_DAC_MUTE, 0);
	LASendSerialV2(lawp, &p);
	atomic_store(&(las->playback), 1);
	(void) widget;
}

void LAFileSenderPause(GtkWidget *widget, LAFileStreamer *las){
	LASerialV2Protocol p;
	LAPrepareProtocolV2Basic(&p, LA_COMMAND_TEST_DAC_MUTE, 1);
	LASendSerialV2(lawp, &p);
	atomic_store(&(las->playback), 0);
	(void) widget;
}

void LAOpenFileStreamer(GtkWidget *widget, LAWindow *law){
	LAFileStreamer  lasv;
	LAFileStreamer *las  = &lasv;

	gchar *filename = LADialogOpenFile(law, "Open file", NULL);
	if(filename == NULL){
		LADialogErrorGeneric(law, "An error ocurred");
		return;
	}

	LAMappedFile file;
	LAErrorCode code = LAMemoryMapFile(filename, &file);
	if(code) return;

	las->offset = 0;
	las->playback = 0;
	las->closeThread = 0;
	las->file = &file;

	las->window = gtk_dialog_new_with_buttons(
					"File Streamer",
					GTK_WINDOW(law->window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					"_Quit", GTK_RESPONSE_CANCEL,
					NULL
				);

	gtk_window_set_modal(GTK_WINDOW(las->window), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(las->window), 8);
	las->content = gtk_dialog_get_content_area(GTK_DIALOG(las->window));
	
	las->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(las->content), GTK_WIDGET(las->vbox));

	las->hboxTitle = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(las->vbox), GTK_WIDGET(las->hboxTitle));

	las->hboxControls = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(las->vbox), GTK_WIDGET(las->hboxControls));

	las->titleTitle		= gtk_label_new("Title:");
	las->titleFilename	= gtk_label_new(filename);

	las->controlsPlay		= gtk_button_new_with_label("Play");
	las->controlsPause		= gtk_button_new_with_label("Pause");
	las->controlsSendOne	= gtk_button_new_with_label("Step");
	las->controlsRewind		= gtk_button_new_with_label("Rewind");

	gtk_container_add(GTK_CONTAINER(las->hboxTitle), GTK_WIDGET(las->titleTitle));
	gtk_container_add(GTK_CONTAINER(las->hboxTitle), GTK_WIDGET(las->titleFilename));

	gtk_container_add(GTK_CONTAINER(las->hboxControls), GTK_WIDGET(las->controlsPlay));
	gtk_container_add(GTK_CONTAINER(las->hboxControls), GTK_WIDGET(las->controlsPause));
	gtk_container_add(GTK_CONTAINER(las->hboxControls), GTK_WIDGET(las->controlsSendOne));
	gtk_container_add(GTK_CONTAINER(las->hboxControls), GTK_WIDGET(las->controlsRewind));

	g_signal_connect(las->controlsPlay,		"clicked", G_CALLBACK(LAFileSenderPlay), las);
	g_signal_connect(las->controlsPause,	"clicked", G_CALLBACK(LAFileSenderPause), las);
	g_signal_connect(las->controlsRewind,	"clicked", G_CALLBACK(LAFileSenderRewind), las);

	pthread_create(&(las->sendThread), NULL, LAThreadFileSender, las);

	gtk_widget_show_all(las->window);
	gtk_dialog_run(GTK_DIALOG(las->window));

	atomic_store(&(las->closeThread), 1);
	pthread_join(las->sendThread, NULL);
	gtk_widget_destroy(las->window);

	code = LAMemoryUnmapFile(&file);
	if(code) return;
	(void) widget;
}
