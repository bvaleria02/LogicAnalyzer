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

gchar *LADialogSaveFile(LAWindow *law, const char *title, const char *filterName){
	if(law == NULL){
		return NULL;
	}

	GtkWidget *fileSelection = gtk_file_chooser_dialog_new(
										title,
										GTK_WINDOW(law->window),
										GTK_FILE_CHOOSER_ACTION_SAVE,
										"Cancel", GTK_RESPONSE_CANCEL,
										"Save", GTK_RESPONSE_ACCEPT,
										NULL
								);

	GtkFileFilter *fileFilter = gtk_file_filter_new();
	gtk_file_filter_set_name(fileFilter, filterName);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fileSelection), fileFilter);

	gchar *filename = NULL;
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(fileSelection), ".");

	gint response = gtk_dialog_run(GTK_DIALOG(fileSelection));

	if(response == GTK_RESPONSE_ACCEPT){
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fileSelection));
	}

	gtk_widget_destroy(fileSelection);
	return filename;
}

gchar *LADialogOpenFile(LAWindow *law, const char *title, const char *filterName){
	if(law == NULL){
		return NULL;
	}

	GtkWidget *fileSelection = gtk_file_chooser_dialog_new(
										title,
										GTK_WINDOW(law->window),
										GTK_FILE_CHOOSER_ACTION_OPEN,
										"Cancel", GTK_RESPONSE_CANCEL,
										"Load", GTK_RESPONSE_ACCEPT,
										NULL
								);
/*
	GtkFileFilter *fileFilter = gtk_file_filter_new();
	gtk_file_filter_set_name(fileFilter, filterName);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fileSelection), fileFilter);
*/
	gchar *filename = NULL;
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(fileSelection), ".");

	gint response = gtk_dialog_run(GTK_DIALOG(fileSelection));

	if(response == GTK_RESPONSE_ACCEPT){
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fileSelection));
	}

	gtk_widget_destroy(fileSelection);
	return filename;
}

void LADialogErrorGeneric(LAWindow *law, char *text){
	GtkWidget *dialog;
	GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL;

    dialog = gtk_message_dialog_new(GTK_WINDOW(law->window),
                                    flags,
                                    GTK_MESSAGE_ERROR,
                                    GTK_BUTTONS_CLOSE,
									"%s",
                                    text
								);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void LADialogNumericEntryError(LAWindow *law){
	char *text = "The input value is not numeric.";
	LADialogErrorGeneric(law, text);
}
