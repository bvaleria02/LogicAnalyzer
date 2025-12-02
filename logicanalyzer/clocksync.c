#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "liblogicanalyzer.h"

const char validClockNames[CLK_NAMES_COUNT][CLK_NAMES_LENGTH] = {	
	"CLK",
	"CP",
	"CL",
	"CK",
	"SCK",
	"I2C_SCK",
	"CLOCK",
	"SPI_SCK",
	"CC",
	"TIMER",
	"INTERRUPT",
	"RELOJ"
};

void LACopyText(char **dest, const gchar *src, size_t *l){
	if(src == NULL || dest == NULL){
		return;
	}

	size_t length = strlen(src);
	if(length == 0){
		(*dest) = NULL;
		return;
	}

	(*dest) = (char *)malloc(length + 1);
	if((*dest) == NULL){
		return;
	}

	strncpy((*dest), src, length);
	(*dest)[length] = '\0';
	if(l != NULL){
		(*l) = length; 
	}
}

void LAStrUpper(char *text, size_t length){
	if(text == NULL || length == 0){
		return;
	}

	char c;
	for(size_t i = 0; i < length; i++){
		c = text[i];
		if(c >= 'a' && c <= 'z'){
			text[i] = text[i] & 0xDF;
		}
	}
}

int8_t LAGetClockChannel(LAWindow *law){
	const gchar *originalName;
	char  *copyName;
	size_t length;

	for(uint8_t i = 0; i < MAX_CHANNEL_COUNT; i++){
		originalName = gtk_entry_get_text(GTK_ENTRY(law->channel[i].entry));
		if(originalName == NULL){
			continue;
		}

		LACopyText(&copyName, originalName, &length);
		if(copyName == NULL){
			continue;
		}

		LAStrUpper(copyName, length);
		// printf("a: %s\tb: %s\n", originalName, copyName);

		for(uint8_t j = 0; j < CLK_NAMES_COUNT; j++){
			if(!strcmp(copyName, validClockNames[j])){
				g_print("Found: %s\n", validClockNames[j]);
				return i;
			}	
		}

		if(copyName != NULL){
			free(copyName);
			copyName = NULL;
		}
	}

	return -1;
}

void LACreateZoomClockSyncWindow(LAWindow *law, LAZoomClockSyncWindow *laz){
	if(law == NULL || laz == NULL){
		return;
	}

	laz->isActive = 1;
	laz->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(laz->window), "Zoom Clock Sync");
	gtk_container_set_border_width(GTK_CONTAINER(laz->window), 8);
	//gtk_widget_set_size_request(laz->window, 500, 300);

	laz->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(laz->window), GTK_WIDGET(laz->vbox));

	laz->channelSelect = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(laz->vbox), GTK_WIDGET(laz->channelSelect));

	laz->hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(laz->vbox), GTK_WIDGET(laz->hbox));

	laz->bufferSelect = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(laz->hbox), GTK_WIDGET(laz->bufferSelect));

	laz->optionsSelect = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(laz->hbox), GTK_WIDGET(laz->optionsSelect));

	laz->confirmHbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(laz->vbox), GTK_WIDGET(laz->confirmHbox));


	// Channel select
	laz->chSelLabel 	= gtk_label_new("Channel select");
	gtk_label_set_justify(GTK_LABEL(laz->chSelLabel), GTK_JUSTIFY_LEFT);
	laz->chSelDropdown 	= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laz->chSelDropdown), "Auto detect clock channel");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laz->chSelDropdown), "Channel 0");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laz->chSelDropdown), "Channel 1");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laz->chSelDropdown), "Channel 2");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laz->chSelDropdown), "Channel 3");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laz->chSelDropdown), "Channel 4");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laz->chSelDropdown), "Channel 5");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laz->chSelDropdown), "Channel 6");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laz->chSelDropdown), "Channel 7");
	gtk_combo_box_set_active(GTK_COMBO_BOX(laz->chSelDropdown), 0);

	gtk_container_add(GTK_CONTAINER(laz->channelSelect), GTK_WIDGET(laz->chSelLabel));
	gtk_container_add(GTK_CONTAINER(laz->channelSelect), GTK_WIDGET(laz->chSelDropdown));


	// Buffer select
	laz->buSelLabel 	= gtk_label_new("Buffer select");
	gtk_label_set_justify(GTK_LABEL(laz->buSelLabel), GTK_JUSTIFY_LEFT);
	laz->buSelDropdown 	= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laz->buSelDropdown), "Current view (1024 samples)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laz->buSelDropdown), "Full buffer (32768 samples)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laz->buSelDropdown), "Custom length");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laz->buSelDropdown), "Manual clock");
	gtk_combo_box_set_active(GTK_COMBO_BOX(laz->buSelDropdown), 0);
	laz->buSelLabel2	= gtk_label_new("Value");
	GtkAdjustment *adjustment = gtk_adjustment_new(1024, 1, 32768, 1, 1024, 0);
	laz->buSelEntry		= gtk_spin_button_new(adjustment, 1.0, 0);
	gtk_widget_set_sensitive(laz->buSelEntry, FALSE);
	laz->buSelDescView  	= gtk_text_view_new();
	laz->buSelDescBuffer  	= gtk_text_view_get_buffer(GTK_TEXT_VIEW(laz->buSelDescView));
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(laz->buSelDescBuffer), "Placeholder for explaining the buffer selection. This should be a long (but detailed) text about what 32768 means, and a reminder that the internal buffer is ant-sized.", -1);
	gtk_widget_set_sensitive(GTK_WIDGET(laz->buSelDescView), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(laz->buSelDescView), GTK_WRAP_WORD);

	gtk_container_add(GTK_CONTAINER(laz->bufferSelect), GTK_WIDGET(laz->buSelLabel));
	gtk_container_add(GTK_CONTAINER(laz->bufferSelect), GTK_WIDGET(laz->buSelDropdown));
	gtk_container_add(GTK_CONTAINER(laz->bufferSelect), GTK_WIDGET(laz->buSelLabel2));
	gtk_container_add(GTK_CONTAINER(laz->bufferSelect), GTK_WIDGET(laz->buSelEntry));
	gtk_container_add(GTK_CONTAINER(laz->bufferSelect), GTK_WIDGET(laz->buSelDescView));

	// Options
	GtkAdjustment *adjTimeLess  = gtk_adjustment_new(100, 10,  1000000, 1, 100, 0);
	GtkAdjustment *adjTimeMore  = gtk_adjustment_new(100, 10,  1000000, 1, 100, 0);
	GtkAdjustment *adjSigma 	= gtk_adjustment_new(3,   0.1, 10,      0.1, 1, 0);
	laz->opLabel 				= gtk_label_new("Options");
	laz->opFallingEdge 			= gtk_check_button_new_with_label("Use falling edge");
	laz->opRisingEdge 			= gtk_check_button_new_with_label("Use rising edge");
	laz->opDiscardOutliers 		= gtk_check_button_new_with_label("Discard outliers (std)");
	laz->opDiscardOutliersLabel = gtk_label_new("Sigma (σ)");
	laz->opDiscardOutliersSB	= gtk_spin_button_new(adjSigma, 0.1, 1);
	laz->opPeriodLess 			= gtk_check_button_new_with_label("Discard is T < ... μs");
	laz->opPeriodLessLabel 		= gtk_label_new("Min time (μs)");
	laz->opPeriodLessSB			= gtk_spin_button_new(adjTimeLess, 0.1, 0);
	laz->opPeriodMore 			= gtk_check_button_new_with_label("Discard is T > ... μs");
	laz->opPeriodMoreLabel 		= gtk_label_new("Max time (μs)");
	laz->opPeriodMoreSB			= gtk_spin_button_new(adjTimeMore, 0.1, 0);
	laz->opExperimentalLabel 	= gtk_label_new("Experimental");
	laz->opAutocorrelation 		= gtk_check_button_new_with_label("Compare with autocorrelation");

	laz->opDiscardOutliersHbox  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	laz->opPeriodLessHbox 		= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	laz->opPeriodMoreHbox 		= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);

	gtk_container_add(GTK_CONTAINER(laz->optionsSelect), GTK_WIDGET(laz->opLabel));
	gtk_container_add(GTK_CONTAINER(laz->optionsSelect), GTK_WIDGET(laz->opFallingEdge));
	gtk_container_add(GTK_CONTAINER(laz->optionsSelect), GTK_WIDGET(laz->opRisingEdge));

	gtk_container_add(GTK_CONTAINER(laz->optionsSelect), GTK_WIDGET(laz->opDiscardOutliers));
	gtk_container_add(GTK_CONTAINER(laz->optionsSelect), GTK_WIDGET(laz->opDiscardOutliersHbox));
	gtk_container_add(GTK_CONTAINER(laz->opDiscardOutliersHbox), GTK_WIDGET(laz->opDiscardOutliersLabel));
	gtk_container_add(GTK_CONTAINER(laz->opDiscardOutliersHbox), GTK_WIDGET(laz->opDiscardOutliersSB));

	gtk_container_add(GTK_CONTAINER(laz->optionsSelect), GTK_WIDGET(laz->opPeriodLess));
	gtk_container_add(GTK_CONTAINER(laz->optionsSelect), GTK_WIDGET(laz->opPeriodLessHbox));
	gtk_container_add(GTK_CONTAINER(laz->opPeriodLessHbox), GTK_WIDGET(laz->opPeriodLessLabel));
	gtk_container_add(GTK_CONTAINER(laz->opPeriodLessHbox), GTK_WIDGET(laz->opPeriodLessSB));

	gtk_container_add(GTK_CONTAINER(laz->optionsSelect), GTK_WIDGET(laz->opPeriodMore));
	gtk_container_add(GTK_CONTAINER(laz->optionsSelect), GTK_WIDGET(laz->opPeriodMoreHbox));
	gtk_container_add(GTK_CONTAINER(laz->opPeriodMoreHbox), GTK_WIDGET(laz->opPeriodMoreLabel));
	gtk_container_add(GTK_CONTAINER(laz->opPeriodMoreHbox), GTK_WIDGET(laz->opPeriodMoreSB));

	gtk_container_add(GTK_CONTAINER(laz->optionsSelect), GTK_WIDGET(laz->opExperimentalLabel));
	gtk_container_add(GTK_CONTAINER(laz->optionsSelect), GTK_WIDGET(laz->opAutocorrelation));
	// confirm
	laz->okay 					= gtk_button_new_with_label("Apply");
	laz->cancel 				= gtk_button_new_with_label("Cancel");
	gtk_container_add(GTK_CONTAINER(laz->confirmHbox), GTK_WIDGET(laz->okay));
	gtk_container_add(GTK_CONTAINER(laz->confirmHbox), GTK_WIDGET(laz->cancel));

	g_signal_connect(laz->window, "destroy", G_CALLBACK(LATerminateWindow), &(laz->window));
	g_signal_connect(laz->cancel, "clicked", G_CALLBACK(LACloseWindow), laz->window);

	gtk_widget_show_all(laz->window);
	gtk_main();
}




