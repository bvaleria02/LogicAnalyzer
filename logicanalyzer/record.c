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

void LADestroyAllRecords(LAWindow *law){
	pthread_mutex_lock(&(law->mutexes.bucketAccess));

	LABucketDestroyAll(law->bd.bucketStart);
	law->bd.bucketStart = NULL;
	law->bd.bucketCurrent = NULL;
	law->bd.bucketWrite = 0;
	
	pthread_mutex_unlock(&(law->mutexes.bucketAccess));
} 


void LARecordStart(GtkWidget *widget, LAWindow *law){
	if(law->bd.bucketWrite){
		return;
	}

	if(law->bd.hasStopped){
		LADestroyAllRecords(law);
	}

	if(law->bd.bucketStart == NULL){
		law->bd.bucketStart = LACreateBucket();
		if(law->bd.bucketStart == NULL) return;
		law->bd.bucketCurrent = law->bd.bucketStart;
	}

	atomic_store(&(law->bd.bucketWrite), 1);
	atomic_store(&(law->bd.hasStopped), 0);
}

void LARecordStop(GtkWidget *widget, LAWindow *law){
	atomic_store(&(law->bd.bucketWrite), 0);
	atomic_store(&(law->bd.hasStopped), 1);
}

void LARecordPause(GtkWidget *widget, LAWindow *law){
	atomic_store(&(law->bd.bucketWrite), 0);
}

void LARecordDelete(GtkWidget *widget, LAWindow *law){
	LADestroyAllRecords(law);
	atomic_store(&(law->bd.hasStopped), 1);
}

void LARecordView(GtkWidget *widget, LAWindow *law){
	LABucketViewAll(law->bd.bucketStart);
}

void LARecordView2(GtkWidget *widget, LAWindow *law){
	size_t size = LABucketGetSizeAll(law->bd.bucketStart);
	g_print("Size calculated\n");
	LACreateHexView(
		law,
		law->bd.bucketStart,
		size,
		LACallbackHexViewBucketAll,
		&(law->mutexes.bucketAccess)
	);
}

void LAGetFileFromDialog(GtkWidget *widget, gchar **filename){
	if(filename == NULL){
		return;
	}

	(*filename) = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
	if((*filename) != NULL){
		g_print("filename: %s\n", (*filename));
	}

	gtk_widget_destroy(widget);
}

void LARecordSaveBin(GtkWidget *widget, LAWindow *law){
	if(law->bd.bucketStart == NULL){
		return;
	}
	
	gchar *filename = LADialogSaveFile(law, "Save binary file", "Hex file");
	if(filename == NULL){
		return;
	}

	pthread_mutex_lock(&(law->mutexes.bucketAccess));

	LAErrorCode response = LAWriteBucketsToFileBinary(law, filename);
	g_print("Response: %i\n", response);
	if(filename != NULL){
		g_free(filename);
	}

	pthread_mutex_unlock(&(law->mutexes.bucketAccess));
	return;
}

void LARecordSaveCSV(GtkWidget *widget, LAWindow *law){
	if(law->bd.bucketStart == NULL){
		return;
	}
	
	gchar *filename = LADialogSaveFile(law, "Save CSV File", "CSV file");
	if(filename == NULL){
		return;
	}

	pthread_mutex_lock(&(law->mutexes.bucketAccess));

	LAErrorCode response = LAWriteBucketsToFileCSV(law, filename);
	g_print("Response: %i\n", response);
	if(filename != NULL){
		g_free(filename);
	}

	pthread_mutex_unlock(&(law->mutexes.bucketAccess));
	return;
}
