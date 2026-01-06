#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "liblogicanalyzer.h"
#include <math.h>

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

const char *bufferDescription[3] = {
	"Use the visible buffer on the screen (affected by zoom).",
	"Use the entire circular buffer (not affected by zoom).",
	"Uses the amount of samples specified above."
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

		for(uint8_t j = 0; j < CLK_NAMES_COUNT; j++){
			if(!strcmp(copyName, validClockNames[j])){
				g_print("Found: %s\n", validClockNames[j]);
				if(copyName != NULL) free(copyName);
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


LAErrorCode LAGetFromCircularBuffer(LAWindow *law, uint8_t **buffer, uint32_t *bufSize, LABufferSelect bufSel, uint32_t bufTargetSize){
	LA_HANDLE_NULLPTR(law,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(buffer,	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(bufSize,	LA_PROPAGATE_ERROR);

	switch(bufSel){
		case LA_BUFFER_SELECT_VISIBLE	:	
											(*bufSize) = LA_BUFFER_SIZE / (double) LAGetZoomMultiplier(law);
											if((*bufSize) > LA_LARGE_BUFFER_SIZE) (*bufSize) = LA_LARGE_BUFFER_SIZE;
											break;
		case LA_BUFFER_SELECT_ALL		:
											(*bufSize) = LA_LARGE_BUFFER_SIZE;
											break;
		case LA_BUFFER_SELECT_CUSTOM	:
											(*bufSize) = bufTargetSize;
											if((*bufSize) > LA_LARGE_BUFFER_SIZE) (*bufSize) = LA_LARGE_BUFFER_SIZE;
											break;
		default							:
											LA_RAISE_ERROR(LA_ERROR_INVALIDVALUE);
											return LA_ERROR_INVALIDVALUE;
											break;
	}
	
	(*buffer)  = malloc((*bufSize));
	LA_HANDLE_NULLPTR((*buffer), LA_ERROR_MALLOC);

	int32_t indexSrc  = law->rd.scopeOffset - 1;
	int32_t indexDest = 0;

	pthread_mutex_lock(&(law->mutexes.dataBufferAccess));
	for(uint32_t i = 0; i < (*bufSize); i++){
		indexDest = (*bufSize) - 1 - i;
		indexSrc--;
		if(indexSrc < 0){
			indexSrc = LA_LARGE_BUFFER_SIZE - 1;
		}

		(*buffer)[indexDest] = law->dataBuffer[indexSrc];
	}
	pthread_mutex_unlock(&(law->mutexes.dataBufferAccess));

	return LA_NO_ERROR;
}

void LAClockSyncCastSettings(LAZoomClockSyncWindow *laz){
	if(laz == NULL) return;

	laz->channelSelectValue = gtk_combo_box_get_active(GTK_COMBO_BOX(laz->chSelDropdown));
	laz->bufferSelectValue  = gtk_combo_box_get_active(GTK_COMBO_BOX(laz->buSelDropdown));
	laz->bufferValue		= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(laz->buSelEntry));
	laz->sigmaValue			= gtk_spin_button_get_value(GTK_SPIN_BUTTON(laz->opDiscardOutliersSB));
	laz->lowerValue			= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(laz->opPeriodLessSB));
	laz->upperValue			= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(laz->opPeriodMoreSB));
	laz->useFallingEdge		= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(laz->opFallingEdge));
	laz->useRisingEdge		= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(laz->opRisingEdge));
	laz->discardOutliers	= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(laz->opDiscardOutliers));
	laz->discardLower		= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(laz->opPeriodLess));
	laz->discardUpper		= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(laz->opPeriodMore));
	laz->useCorrelation		= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(laz->opAutocorrelation));
}

LAErrorCode LAClockSyncCreateNode(LAClockSyncNode **node, uint8_t value, uint32_t duration){
	LA_HANDLE_NULLPTR(node, LA_PROPAGATE_ERROR);

	(*node) = (LAClockSyncNode *)malloc(sizeof(LAClockSyncNode));
	LA_HANDLE_NULLPTR((*node), LA_ERROR_MALLOC);

	(*node)->value = value;
	(*node)->duration = duration;
	(*node)->next = NULL;
	(*node)->prev = NULL;
	(*node)->ignore = 0;

	return LA_NO_ERROR;
}

LAErrorCode LAClockSyncDeleteNode(LAClockSyncNode *node){
	LA_HANDLE_NULLPTR(node, LA_PROPAGATE_ERROR);

	if(node->prev != NULL){
		(node->prev)->next = node->next;
	}

	if(node->next != NULL){
		(node->next)->prev = node->prev;
	}

	free(node);
	return LA_NO_ERROR;
}

LAErrorCode LAClockSyncDeleteAllNodes(LAClockSyncNode **nodeStart){
	LA_HANDLE_NULLPTR(nodeStart, LA_PROPAGATE_ERROR);

	LAClockSyncNode *node = (*nodeStart);
	LAClockSyncNode *next = NULL;

	while(node != NULL){
		next = node->next;
		free(node);
		node = next;
	}

	(*nodeStart) = NULL;
	return LA_NO_ERROR;
}

LAErrorCode LAClockSyncSplitInsertNode(LAClockSyncNode **nodeStart, LAClockSyncNode **node, LAClockSyncNode **newNode, int8_t *value, uint32_t *duration, uint8_t sample){
	LA_HANDLE_NULLPTR(nodeStart,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(node,				LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(newNode,			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value,			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(duration,			LA_PROPAGATE_ERROR);

	LAErrorCode code = LAClockSyncCreateNode(newNode, (*value), (*duration));
	if(code) return code;

	if((*node) == NULL){
		(*nodeStart) = (*newNode);
		(*node) = (*newNode);
	} else {
		(*node)->next = (*newNode);
		(*newNode)->prev = (*node);
		(*node) = (*newNode);
	}

	(*value) = sample;
	(*duration) = 1;
	return LA_NO_ERROR;
}

LAErrorCode LAClockSyncSplitBuffer(LAZoomClockSyncWindow *laz, uint8_t *buffer, uint32_t bufSize, LAClockSyncNode **nodeStart, uint8_t channelSelect){
	LA_HANDLE_NULLPTR(buffer, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(nodeStart, 	LA_PROPAGATE_ERROR);

	int8_t channel = (channelSelect > 1) ? channelSelect - 1 : LAGetClockChannel(lawp);
	if(channel < 0) channel = 0;
	//g_print("Channel: %i\n", channel);

	int8_t value = -1;
	uint32_t duration = 0;
	uint8_t sample = 0;

	LAClockSyncNode *node 		= (*nodeStart);
	LAClockSyncNode *newNode 	= NULL;
	LAErrorCode code = LA_NO_ERROR;

	if(laz->useFallingEdge == 0 && laz->useRisingEdge == 0){
		return LA_NO_ERROR;
	}

	for(uint32_t i = 0; i < bufSize; i++){
		sample = (buffer[i] >> channel) & 0x1;
	//	g_print("i: %i\tvalue: %i\n", i, sample);
		
		if(value == -1){
			value = sample;
			duration++;
			continue;
		}
		
		if((laz->useFallingEdge == 1) && (laz->useRisingEdge == 0) && sample == 0 && value == 1){
			code = LAClockSyncSplitInsertNode(nodeStart, &(node), &(newNode), &value, &duration, sample);
			if(code) return code;
		} else if((laz->useFallingEdge == 0) && (laz->useRisingEdge == 1) && sample == 1 && value == 0){
			code = LAClockSyncSplitInsertNode(nodeStart, &(node), &(newNode), &value, &duration, sample);
			if(code) return code;
		} else if(sample != value && value != -1 && laz->useFallingEdge && laz->useRisingEdge){
			code = LAClockSyncSplitInsertNode(nodeStart, &(node), &(newNode), &value, &duration, sample);
			if(code) return code;
		} else {
			if(laz->useFallingEdge == 0 || laz->useRisingEdge == 0){
				value = sample;
			}
			duration++;
		}
	}

	code = LAClockSyncCreateNode(&(newNode), value, duration);
	if(code) return code;

	if(node == NULL){
		(*nodeStart) = newNode;
		node = newNode;
	} else {
		node->next = newNode;
		newNode->prev = node;
		node = newNode;
	}

	return LA_NO_ERROR;
}

uint32_t LAConvertSamplesToUs(uint32_t pollingTime, uint32_t samples){
	return (pollingTime * samples);
}

double LAConvertSamplesToUsF(uint32_t pollingTime, double samples){
	return (pollingTime * samples);
}

LAErrorCode LAClockSyncGetMean(LAZoomClockSyncWindow *laz, LAClockSyncNode *nodeStart, double *mean, uint8_t useIgnore){
	LA_HANDLE_NULLPTR(laz,			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(nodeStart,	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(mean,			LA_PROPAGATE_ERROR);

	LAClockSyncNode *node 	= nodeStart;

	uint32_t sampleLength = 0;

	uint32_t accumulator = 0;
	uint32_t counter = 0;

	uint32_t timeUs = 0;

	while(node != NULL){
		sampleLength = node->duration;
		timeUs = LAConvertSamplesToUs(lawp->rd.pollingTime, sampleLength);
		
		if(useIgnore == 1 && node->ignore == 1){
			// Left blank
		} else if(laz->discardLower && timeUs < laz->lowerValue){
			// Left blank
		} else if (laz->discardUpper && timeUs > laz->upperValue){
			// Left blank
		} else {
			accumulator += sampleLength;
			counter++;
		}

		node = node->next;
	}

	if(counter > 0){
		(*mean) = accumulator / (double) counter;
	} else {
		(*mean) = 0;
	}

	if(laz->useFallingEdge && laz->useRisingEdge){
		(*mean) = 2 * (*mean);
	}

	return LA_NO_ERROR;
}

LAErrorCode LAClockSyncGetStd(LAZoomClockSyncWindow *laz, LAClockSyncNode *nodeStart, double mean, double *std){
	LA_HANDLE_NULLPTR(laz,			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(nodeStart,	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(std,			LA_PROPAGATE_ERROR);

	LAClockSyncNode *node 	= nodeStart;

	uint32_t sampleLength = 0;

	double accumulator = 0;
	uint32_t counter = 0;

	uint32_t timeUs = 0;
	if(laz->useFallingEdge && laz->useRisingEdge){
		mean = mean / (double) 2;
	}

	while(node != NULL){
		sampleLength = node->duration;
		timeUs = LAConvertSamplesToUs(lawp->rd.pollingTime, sampleLength);

		if(laz->discardLower && timeUs < laz->lowerValue){
			// Left blank
		} else if (laz->discardUpper && timeUs > laz->upperValue){
			// Left blank
		} else {
			accumulator += powf(sampleLength - mean, 2);
			counter++;
		}

		node = node->next;
	}

	if(counter > 0){
		(*std) = accumulator / (double) counter;
		(*std) = sqrtf((*std));
	} else {
		(*std) = 0;
	}

	return LA_NO_ERROR;
}

LAErrorCode LAClockSyncDropOutliers(LAZoomClockSyncWindow *laz, LAClockSyncNode *nodeStart, double mean, double std){
	LA_HANDLE_NULLPTR(laz,			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(nodeStart,	LA_PROPAGATE_ERROR);

	if(laz->discardOutliers == 0){
		g_print("No outliers\n");
		return LA_NO_ERROR;
	}

	if(std == 0){
		g_print("std = 0\n");
		return LA_NO_ERROR;
	}
	
	double value = 0;
	LAClockSyncNode *node 	= nodeStart;
	if(laz->useFallingEdge && laz->useRisingEdge){
		mean = mean / (double) 2;
	}

	while(node != NULL){
		value = fabs(node->duration - mean);
		value = value / (double) std;

		g_print("Value: %lf\tSigma: %lf\n", value, std);
		if(value > laz->sigmaValue){
			g_print("Discarded\n");
			node->ignore = 1;
		}

		node = node->next;
	}

	return LA_NO_ERROR;
}

void LAClockSyncApply(GtkWidget *widget, LAZoomClockSyncWindow *laz){
	LAClockSyncCastSettings(laz);

	LAErrorCode code = LAGetFromCircularBuffer(lawp, &(laz->buffer), &(laz->bufferSize), laz->bufferSelectValue, laz->bufferValue);
	if(code) return;

/*
	g_print("Buffer size: %i\n", laz->bufferSize);
	LACreateHexView(lawp, laz->buffer, laz->bufferSize, LACallbackNormalBuffer, NULL);
*/
	code = LAClockSyncSplitBuffer(laz, laz->buffer, laz->bufferSize, &(laz->splitNodeStart), laz->channelSelectValue);
	
	code = LAClockSyncGetMean(laz, laz->splitNodeStart, &(laz->mean), 0);
	g_print("Mean: %lf us\n", LAConvertSamplesToUsF(lawp->rd.pollingTime, laz->mean));

	code = LAClockSyncGetStd(laz, laz->splitNodeStart, laz->mean, &(laz->std));
	g_print("Std: %lf us\n", laz->std);

	code = LAClockSyncDropOutliers(laz, laz->splitNodeStart, laz->mean, laz->std);

	code = LAClockSyncGetMean(laz, laz->splitNodeStart, &(laz->mean), 1);
	g_print("Mean: %lf us\n", LAConvertSamplesToUsF(lawp->rd.pollingTime, laz->mean));
/*
	LAClockSyncNode *node = laz->splitNodeStart;
	uint32_t i = 0;
	while(node != NULL){
		g_print("Node: %i\tValue: %i\tDuration: %i\n", i, node->value, node->duration);
		i++;
		node = node->next;
	}
*/

	char bufferValue[LA_SMALL_BUFFER_SIZE];
	snprintf(bufferValue, LA_SMALL_BUFFER_SIZE, "Time: %lf μs", LAConvertSamplesToUsF(lawp->rd.pollingTime, laz->mean));
	gtk_label_set_text(GTK_LABEL(laz->buSelResultValue), bufferValue);

	if(laz->mean != 0){
		snprintf(bufferValue, LA_SMALL_BUFFER_SIZE, "Freq: %lf Hz", 1000000 / (double) LAConvertSamplesToUsF(lawp->rd.pollingTime, laz->mean));
	} else {
		snprintf(bufferValue, LA_SMALL_BUFFER_SIZE, "Freq: -- Hz");
	}
	gtk_label_set_text(GTK_LABEL(laz->buSelResultValue2), bufferValue);

	LAClockSyncDeleteAllNodes(&(laz->splitNodeStart));
	if(laz->buffer != NULL) free(laz->buffer);
}

void LAClockSyncBufferSelectChanged(GtkWidget *widget, LAZoomClockSyncWindow *laz){
	uint32_t index = gtk_combo_box_get_active(GTK_COMBO_BOX(laz->buSelDropdown));

	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(laz->buSelDescBuffer), bufferDescription[index], -1);

	if(index == LA_BUFFER_SELECT_CUSTOM){
		gtk_widget_set_sensitive(laz->buSelEntry, TRUE);
	} else {
		gtk_widget_set_sensitive(laz->buSelEntry, FALSE);
	}
}

void LAClockSyncClickedDiscardOutliers(GtkWidget *widget, LAZoomClockSyncWindow *laz){
	uint8_t value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(laz->opDiscardOutliers));

	if(value == 1){
		gtk_widget_set_sensitive(laz->opDiscardOutliersSB, TRUE);
	} else {
		gtk_widget_set_sensitive(laz->opDiscardOutliersSB, FALSE);
	}
}

void LAClockSyncClickedPeriodLess(GtkWidget *widget, LAZoomClockSyncWindow *laz){
	uint8_t value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(laz->opPeriodLess));

	if(value == 1){
		gtk_widget_set_sensitive(laz->opPeriodLessSB, TRUE);
	} else {
		gtk_widget_set_sensitive(laz->opPeriodLessSB, FALSE);
	}
}

void LAClockSyncClickedPeriodMore(GtkWidget *widget, LAZoomClockSyncWindow *laz){
	uint8_t value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(laz->opPeriodMore));

	if(value == 1){
		gtk_widget_set_sensitive(laz->opPeriodMoreSB, TRUE);
	} else {
		gtk_widget_set_sensitive(laz->opPeriodMoreSB, FALSE);
	}
}

void LAClockSyncPreset(LAZoomClockSyncWindow *laz){
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(laz->opFallingEdge), TRUE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(laz->opRisingEdge), TRUE);
}

void LACreateZoomClockSyncWindow(LAWindow *law, LAZoomClockSyncWindow *laz, const char *title){
	if(law == NULL || laz == NULL){
		return;
	}

	laz->isActive = 1;
	laz->pollingTime = law->rd.pollingTime;
	laz->buffer = NULL;
	laz->bufferSize = 0;
	laz->splitNodeStart = NULL;
	laz->mean = 0;
	laz->std = 0;

//	laz->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//	gtk_window_set_transient_for(GTK_WINDOW(laz->window), GTK_WINDOW(law->window));
//	gtk_window_set_destroy_with_parent(GTK_WINDOW(laz->window), TRUE);

	laz->window = gtk_dialog_new_with_buttons(
					(title != NULL) ? title : "Zoom Clock Sync",
					GTK_WINDOW(law->window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					"_Apply", GTK_RESPONSE_ACCEPT,
					"_Quit", GTK_RESPONSE_CANCEL,
					NULL
				);

	//gtk_window_set_transient_for(GTK_WINDOW(laz->window), GTK_WINDOW(law->window));
	gtk_window_set_modal(GTK_WINDOW(laz->window), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(laz->window), 8);
	laz->content = gtk_dialog_get_content_area(GTK_DIALOG(laz->window));

	//gtk_widget_set_size_request(laz->window, 500, 300);

	laz->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(laz->content), GTK_WIDGET(laz->vbox));

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
	gtk_combo_box_set_active(GTK_COMBO_BOX(laz->buSelDropdown), 0);
	laz->buSelLabel2	= gtk_label_new("Value");
	GtkAdjustment *adjustment = gtk_adjustment_new(1024, 1, 32768, 1, 1024, 0);
	laz->buSelEntry		= gtk_spin_button_new(adjustment, 1.0, 0);
	gtk_widget_set_sensitive(laz->buSelEntry, FALSE);
	laz->buSelDescView  	= gtk_text_view_new();
	laz->buSelDescBuffer  	= gtk_text_view_get_buffer(GTK_TEXT_VIEW(laz->buSelDescView));
	gtk_widget_set_sensitive(GTK_WIDGET(laz->buSelDescView), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(laz->buSelDescView), GTK_WRAP_WORD);
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(laz->buSelDescBuffer), bufferDescription[0], -1);
	laz->buSelSep			= gtk_label_new(" ");
	laz->buSelResultTile	= gtk_label_new("Last result:");
	laz->buSelResultValue	= gtk_label_new("(Not run)");
	laz->buSelResultValue2  = gtk_label_new(" ");
	laz->buSelSep2			= gtk_label_new(" ");
	laz->buSelResultTip		= gtk_label_new("If the last result is zero,\ncheck if your settings are too\nstrict.");
	gtk_label_set_line_wrap(GTK_LABEL(laz->buSelResultTip), TRUE);

	gtk_container_add(GTK_CONTAINER(laz->bufferSelect), GTK_WIDGET(laz->buSelLabel));
	gtk_container_add(GTK_CONTAINER(laz->bufferSelect), GTK_WIDGET(laz->buSelDropdown));
	gtk_container_add(GTK_CONTAINER(laz->bufferSelect), GTK_WIDGET(laz->buSelLabel2));
	gtk_container_add(GTK_CONTAINER(laz->bufferSelect), GTK_WIDGET(laz->buSelEntry));
	gtk_container_add(GTK_CONTAINER(laz->bufferSelect), GTK_WIDGET(laz->buSelDescView));
	gtk_container_add(GTK_CONTAINER(laz->bufferSelect), GTK_WIDGET(laz->buSelSep));
	gtk_container_add(GTK_CONTAINER(laz->bufferSelect), GTK_WIDGET(laz->buSelResultTile));
	gtk_container_add(GTK_CONTAINER(laz->bufferSelect), GTK_WIDGET(laz->buSelResultValue));
	gtk_container_add(GTK_CONTAINER(laz->bufferSelect), GTK_WIDGET(laz->buSelResultValue2));
	gtk_container_add(GTK_CONTAINER(laz->bufferSelect), GTK_WIDGET(laz->buSelSep2));
	gtk_container_add(GTK_CONTAINER(laz->bufferSelect), GTK_WIDGET(laz->buSelResultTip));

	// Options
	GtkAdjustment *adjTimeLess  = gtk_adjustment_new(100, 10,  1000000, 1, 100, 0);
	GtkAdjustment *adjTimeMore  = gtk_adjustment_new(100, 10,  1000000, 1, 100, 0);
	GtkAdjustment *adjSigma 	= gtk_adjustment_new(3,   0.01, 10,      0.01, 1, 0);
	laz->opLabel 				= gtk_label_new("Options");
	laz->opFallingEdge 			= gtk_check_button_new_with_label("Use falling edge");
	laz->opRisingEdge 			= gtk_check_button_new_with_label("Use rising edge");
	laz->opDiscardOutliers 		= gtk_check_button_new_with_label("Discard outliers (std)");
	laz->opDiscardOutliersLabel = gtk_label_new("Sigma (σ)");
	laz->opDiscardOutliersSB	= gtk_spin_button_new(adjSigma, 0.1, 2);
	gtk_widget_set_sensitive(laz->opDiscardOutliersSB, FALSE);
	laz->opPeriodLess 			= gtk_check_button_new_with_label("Discard is T < ... μs");
	laz->opPeriodLessLabel 		= gtk_label_new("Min time (μs)");
	laz->opPeriodLessSB			= gtk_spin_button_new(adjTimeLess, 0.1, 0);
	gtk_widget_set_sensitive(laz->opPeriodLessSB, FALSE);
	laz->opPeriodMore 			= gtk_check_button_new_with_label("Discard is T > ... μs");
	laz->opPeriodMoreLabel 		= gtk_label_new("Max time (μs)");
	laz->opPeriodMoreSB			= gtk_spin_button_new(adjTimeMore, 0.1, 0);
	gtk_widget_set_sensitive(laz->opPeriodMoreSB, FALSE);
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
	laz->okay 					= gtk_button_new_with_mnemonic("_Calculate");
	gtk_container_add(GTK_CONTAINER(laz->confirmHbox), GTK_WIDGET(laz->okay));

	//g_signal_connect(laz->window, 			"destroy", G_CALLBACK(LATerminateWindow), &(laz->window));
	g_signal_connect(laz->buSelDropdown, 	"changed", G_CALLBACK(LAClockSyncBufferSelectChanged), laz);
	g_signal_connect(laz->okay,   			"clicked", G_CALLBACK(LAClockSyncApply), laz);
	g_signal_connect(laz->opDiscardOutliers,"clicked", G_CALLBACK(LAClockSyncClickedDiscardOutliers), laz);
	g_signal_connect(laz->opPeriodLess,     "clicked", G_CALLBACK(LAClockSyncClickedPeriodLess), laz);
	g_signal_connect(laz->opPeriodMore,     "clicked", G_CALLBACK(LAClockSyncClickedPeriodMore), laz);
	
	LAClockSyncPreset(laz);

	gtk_widget_show_all(laz->window);
	laz->response = gtk_dialog_run(GTK_DIALOG(laz->window));
	gtk_widget_destroy(laz->window);
}

void LAOpenClockFreqAnalyzer(GtkWidget *widget, LAWindow *law){
	LAZoomClockSyncWindow lazv;
	LAZoomClockSyncWindow *laz = &lazv;

	LACreateZoomClockSyncWindow(law, laz, "Clock frequency analyzer");
}
