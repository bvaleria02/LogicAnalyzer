#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "liblogicanalyzer.h"
#include <string.h>
#include <time.h>
#include <pthread.h>

#define HEXVIEW_PAGE_SIZE 512
#define HEXVIEW_HEX_HEIGHT 576

void LATerminateHexViewWindow(GtkWidget *widget, LAHexView *lah){
	gtk_window_close(GTK_WINDOW(lah->window));
	gtk_main_quit();
}

LAErrorCode LACallbackHexViewSingleBucket(void *src, size_t srcSize, size_t offset, uint8_t *viewBuffer, size_t viewSize, size_t *bytesRead){
	LA_HANDLE_NULLPTR(src, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(viewBuffer, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(bytesRead, 	LA_PROPAGATE_ERROR);

	LABucket *bucket = (LABucket *)src;

	size_t k = 0;
	size_t offsetEnd = offset + viewSize;
	if(offsetEnd >= bucket->length)		offsetEnd = bucket->length;
	//g_print("Offset: %li\n", offsetEnd);
	size_t offsetStart = offset;
	if(offsetStart >= bucket->length && bucket->length > 0){
		offsetStart = bucket->length;
	} else if(bucket->length == 0){
		offsetStart = 0;
	}
	//g_print("Offset: %li\n", offsetStart);
	(*bytesRead) = offsetEnd - offsetStart;
	//g_print("Offset: %li\n", *bytesRead);

	for(size_t i = offsetStart; i < offsetEnd; i++){
		k = i - offsetStart;
		viewBuffer[k] = bucket->data[i];
	}

	return LA_NO_ERROR;
}

LAErrorCode LACallbackCircularBufferView(void *src, size_t srcSize, size_t offset, uint8_t *viewBuffer, size_t viewSize, size_t *bytesRead){
	LA_HANDLE_NULLPTR(src, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(viewBuffer, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(bytesRead, 	LA_PROPAGATE_ERROR);
	
	LAWindow *law   = (LAWindow *)src;
	uint8_t *buffer = (uint8_t *)law->dataBuffer;
	LA_HANDLE_NULLPTR(buffer, 		LA_PROPAGATE_ERROR);
	(*bytesRead) = 0;

	size_t bufferIndex = (law->rd.dataOffset + offset) % srcSize;
	size_t effectiveAddress = offset;

	for(size_t i = 0; i < viewSize; i++){
		if(bufferIndex >= srcSize){
			bufferIndex = 0;
		}

		if(effectiveAddress >= srcSize){
			break;
		}

		viewBuffer[i] = buffer[bufferIndex];
		bufferIndex += 1;
		effectiveAddress += 1;
		(*bytesRead) = (*bytesRead) + 1;
	}

	return LA_NO_ERROR;
}

LAErrorCode LACallbackNormalBuffer(void *src, size_t srcSize, size_t offset, uint8_t *viewBuffer, size_t viewSize, size_t *bytesRead){
	LA_HANDLE_NULLPTR(src, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(viewBuffer, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(bytesRead, 	LA_PROPAGATE_ERROR);

	size_t bufferIndex = offset;
	uint8_t *buffer = (uint8_t *)src;
	(*bytesRead) = 0;
	
	for(size_t i = 0; i < viewSize; i++){
		if(bufferIndex >= srcSize){
			break;
		}

		viewBuffer[i] = buffer[bufferIndex];
		bufferIndex += 1;
		(*bytesRead) = (*bytesRead) + 1;
	}

	return LA_NO_ERROR;
}

LAErrorCode LACallbackFileView(void *src, size_t srcSize, size_t offset, uint8_t *viewBuffer, size_t viewSize, size_t *bytesRead){
	LA_HANDLE_NULLPTR(src, 			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(viewBuffer, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(bytesRead, 	LA_PROPAGATE_ERROR);

	FILE *fp = (FILE *)src;
	fseek(fp, offset, SEEK_SET);
	size_t length = fread(viewBuffer, 1, viewSize, fp);
	//g_print("Length: %lu\n", length);
	(*bytesRead) = length;

	return LA_NO_ERROR;
}

LAErrorCode LAHexViewUpdateOffsetBuffer(LAHexView *lah, size_t bytesRead){
	// ((bytesRead // 16) + 1) * (16 {Address} + 1 {newline})
	size_t textOffsetSize = ((bytesRead >> 4) + 1) * (16 + 1) + 1;
	char *textOffset = malloc(textOffsetSize);
	if(textOffset == NULL){
		LA_RAISE_ERROR(LA_ERROR_MALLOC);
		return LA_ERROR_MALLOC;
	}

	g_print("Size: %li\n", textOffsetSize);
	for(uint32_t i = 0; i <= (bytesRead >> 4); i++){
		snprintf(&(textOffset[17 * i]), textOffsetSize - (17 * i), "%016lX\n", lah->offset + (i << 4));
	}

	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(lah->hexOffsetBuffer), textOffset, -1);
	free(textOffset);
	return LA_NO_ERROR;
}

LAErrorCode LAHexViewUpdateDataBuffer(LAHexView *lah, uint8_t *buffer, size_t bytesRead){
	size_t textDataSize = (bytesRead * 3) + 1;
	char *textData = malloc(textDataSize);
	if(textData == NULL){
		LA_RAISE_ERROR(LA_ERROR_MALLOC);
		return LA_ERROR_MALLOC;
	}

	memset(textData, '\0', textDataSize);
	size_t index = 0;
	g_print("Size: %li\n", textDataSize);
	for(uint32_t i = 0; i <= (bytesRead >> 4); i++){
		for(uint8_t j = 0; j < 0x10; j++){
			index = (i << 4) + j;
			if(index >= bytesRead)	break;
			//g_print("i: %i\t j: %i\n", i , j);
			if(j < 15){
				snprintf(&(textData[3 * index]), textDataSize - (3 * index), "%02X ", buffer[index]);
			} else {
				snprintf(&(textData[3 * index]), textDataSize - (3 * index), "%02X\n", buffer[index]);
			}
		}
	}

	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(lah->hexDataBuffer), textData, -1);
	free(textData);
	return LA_NO_ERROR;
}

LAErrorCode LAHexViewUpdateCharBuffer(LAHexView *lah, uint8_t *buffer, size_t bytesRead){
	size_t textCharSize = ((bytesRead >> 4) + 1) * (16 + 1) + 1;
	char *textChar = malloc(textCharSize);
	if(textChar == NULL){
		LA_RAISE_ERROR(LA_ERROR_MALLOC);
		return LA_ERROR_MALLOC;
	}

	memset(textChar, '\0', textCharSize);
	size_t index = 0;
	char   c	 = 0;
	g_print("Size: %li\n", textCharSize);
	for(uint32_t i = 0; i <= (bytesRead >> 4); i++){
		for(uint8_t j = 0; j < 0x10; j++){
			index = (i << 4) + j;
			if(index >= bytesRead)	break;
			
			c = buffer[index];
			if(c < 0x20 || c > 0x7E) c = '.';
	
			if(j < 15){
				snprintf(&(textChar[i + index]), textCharSize - (i + index), "%c", c);
			} else {
				snprintf(&(textChar[i + index]), textCharSize - (i + index), "%c\n", c);
			}
		}
	}

	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(lah->hexCharBuffer), textChar, -1);
	free(textChar);
	return LA_NO_ERROR;
}

LAErrorCode	LAHexViewUpdateInfo(LAHexView *lah, size_t bytesRead){
	char buffer[LA_SMALL_BUFFER_SIZE * 2];
	snprintf(buffer, LA_SMALL_BUFFER_SIZE * 2, "Offset: %016lX", lah->offset);
	gtk_label_set_text(GTK_LABEL(lah->infoOffset), buffer);
	if(bytesRead > 0){
		snprintf(buffer, LA_SMALL_BUFFER_SIZE * 2, "Range: %016lX - %016lX", lah->offset, lah->offset + bytesRead - 1);
	} else {
		snprintf(buffer, LA_SMALL_BUFFER_SIZE * 2, "Range: N/A");
	}
	gtk_label_set_text(GTK_LABEL(lah->infoRange), buffer);
	return LA_NO_ERROR;
}

LAErrorCode LAHexViewUpdateData(LAHexView *lah){
	LA_HANDLE_NULLPTR(lah, 	LA_PROPAGATE_ERROR);
	uint8_t hexViewBufferView[HEXVIEW_PAGE_SIZE];
	size_t bytesRead = 0;
	
	if(lah->callback == NULL){
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(lah->hexDataBuffer), "No callback function set.", -1);
		LA_RAISE_ERROR(LA_ERROR_NULLPTR);
		return LA_ERROR_NULLPTR;
	}

	if(lah->mutex != NULL){
		pthread_mutex_lock(lah->mutex);
	}

	LAErrorCode response = LA_NO_ERROR;
	response = lah->callback(
					lah->src,
					lah->srcSize,
					lah->offset,
					hexViewBufferView,
					HEXVIEW_PAGE_SIZE,
					&bytesRead
				);

	if(lah->mutex != NULL){
		pthread_mutex_unlock(lah->mutex);
	}

	if(response != LA_NO_ERROR){
		g_print("Response: %i\n", response);
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(lah->hexDataBuffer), "An error ocurred on callback function.", -1);
		return response;
	}

	response = LAHexViewUpdateOffsetBuffer(lah, bytesRead);
	if(response != LA_NO_ERROR) goto handle_error;

	response = LAHexViewUpdateDataBuffer(lah, hexViewBufferView, bytesRead);
	if(response != LA_NO_ERROR) goto handle_error;

	response = LAHexViewUpdateCharBuffer(lah, hexViewBufferView, bytesRead);
	if(response != LA_NO_ERROR) goto handle_error;

	response = LAHexViewUpdateInfo(lah, bytesRead);
	if(response != LA_NO_ERROR) goto handle_error;

	return LA_NO_ERROR;

handle_error:
	g_print("Response: %i\n", response);
	return response;
}

LAErrorCode LAHexViewHandleOffsetChange(GtkWidget *widget, LAHexView *lah){
	const gchar *entryText = gtk_entry_get_text(GTK_ENTRY(lah->offsetSpin));
	if(entryText == NULL){
		LA_RAISE_ERROR(LA_ERROR_NULLPTR);
		return LA_ERROR_MALLOC;
	}

	size_t offset;
	if(sscanf(entryText, "%lX", &offset) != 1){
		LADialogNumericEntryError(lawp);
		LA_RAISE_ERROR(LA_ERROR_VALUEREAD);
		return LA_ERROR_VALUEREAD;
	}

	if(offset >= lah->srcSize)		offset = lah->srcSize - 1;
	lah->offset = offset & ~(0xF);

	g_print("Offset read: %lu\n", offset);
	LAHexViewUpdateData(lah);
}

LAErrorCode LACreateHexView(LAWindow *law, void *buffer, size_t size, LAHexDumpCallback callback, pthread_mutex_t *mutex){
	LA_HANDLE_NULLPTR(law, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(buffer, 	LA_PROPAGATE_ERROR);
//	LA_HANDLE_NULLPTR(callback, LA_PROPAGATE_ERROR);

	LAHexView slah;
	LAHexView *lah 	= &slah;
	lah->src 		= buffer;
	lah->srcSize	= size;
	lah->callback	= callback;
	lah->mutex		= mutex;
	lah->offset		= 0;

	char tempCharBuffer[LA_SMALL_BUFFER_SIZE];

	lah->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(lah->window), "HexView");
	gtk_container_set_border_width(GTK_CONTAINER(lah->window), 8);

	lah->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(lah->window), GTK_WIDGET(lah->vbox));

	lah->offsetHbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lah->vbox), GTK_WIDGET(lah->offsetHbox));
	lah->hexHbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add(GTK_CONTAINER(lah->vbox), GTK_WIDGET(lah->hexHbox));
	lah->infoHbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
	gtk_container_add(GTK_CONTAINER(lah->vbox), GTK_WIDGET(lah->infoHbox));
	lah->buttonsHbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lah->vbox), GTK_WIDGET(lah->buttonsHbox));

	//GtkAdjustment *adjustment = gtk_adjustment_new(0, 0, size, 1, HEXVIEW_PAGE_SIZE, 0);

	lah->offsetLabel 		= gtk_label_new("Offset:");
	//lah->offsetSpin  		= gtk_spin_button_new(adjustment, 1, 0);
	lah->offsetSpin  		= gtk_entry_new();
	lah->offsetButton 		= gtk_button_new_with_label("View");

	lah->hexOffsetView		= gtk_text_view_new();
	lah->hexOffsetBuffer	= gtk_text_view_get_buffer(GTK_TEXT_VIEW(lah->hexOffsetView));
	gtk_widget_set_size_request(lah->hexOffsetView, 160, HEXVIEW_HEX_HEIGHT);
	gtk_text_view_set_monospace(GTK_TEXT_VIEW(lah->hexOffsetView), TRUE);
	lah->hexDataView		= gtk_text_view_new();
	lah->hexDataBuffer		= gtk_text_view_get_buffer(GTK_TEXT_VIEW(lah->hexDataView));
	gtk_widget_set_size_request(lah->hexDataView, 416, HEXVIEW_HEX_HEIGHT);
	gtk_text_view_set_monospace(GTK_TEXT_VIEW(lah->hexDataView), TRUE);
	lah->hexCharView		= gtk_text_view_new();
	lah->hexCharBuffer		= gtk_text_view_get_buffer(GTK_TEXT_VIEW(lah->hexCharView));
	gtk_widget_set_size_request(lah->hexCharView, 216, HEXVIEW_HEX_HEIGHT);
	gtk_text_view_set_monospace(GTK_TEXT_VIEW(lah->hexCharView), TRUE);

	lah->infoOffset 		= gtk_label_new("Offset: ");
	lah->infoRange 			= gtk_label_new("Range: ");
	snprintf(tempCharBuffer, LA_SMALL_BUFFER_SIZE, "Size: %016lX bytes", size);
	lah->infoSize 			= gtk_label_new(tempCharBuffer);

	lah->buttonExport		= gtk_button_new_with_label("Export");
	lah->buttonQuit 		= gtk_button_new_with_label("Quit");

	gtk_container_add(GTK_CONTAINER(lah->offsetHbox), lah->offsetLabel);
	gtk_container_add(GTK_CONTAINER(lah->offsetHbox), lah->offsetSpin);
	gtk_container_add(GTK_CONTAINER(lah->offsetHbox), lah->offsetButton);

	gtk_container_add(GTK_CONTAINER(lah->hexHbox), lah->hexOffsetView);
	gtk_container_add(GTK_CONTAINER(lah->hexHbox), lah->hexDataView);
	gtk_container_add(GTK_CONTAINER(lah->hexHbox), lah->hexCharView);

	gtk_container_add(GTK_CONTAINER(lah->infoHbox), lah->infoOffset);
	gtk_container_add(GTK_CONTAINER(lah->infoHbox), lah->infoRange);
	gtk_container_add(GTK_CONTAINER(lah->infoHbox), lah->infoSize);

	gtk_container_add(GTK_CONTAINER(lah->buttonsHbox), lah->buttonExport);
	gtk_container_add(GTK_CONTAINER(lah->buttonsHbox), lah->buttonQuit);

	g_signal_connect(lah->window, 		"destroy", G_CALLBACK(LATerminateHexViewWindow), lah);
	g_signal_connect(lah->buttonQuit, 	"clicked", G_CALLBACK(LACloseWindow), lah->window);
	g_signal_connect(lah->offsetButton, "clicked", G_CALLBACK(LAHexViewHandleOffsetChange), lah);
	g_signal_connect(lah->offsetSpin, 	"activate", G_CALLBACK(LAHexViewHandleOffsetChange), lah);

	LAHexViewUpdateData(lah);
	gtk_widget_show_all(lah->window);
	gtk_main();

	return LA_NO_ERROR;
}
