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
#include "preset.h"
#include "dialog.h"
#include "gtk_funcs.h"
#include "compiler.h"
#include "compiler/file.h"

#define DEFAULT_HEADER_HEIGHT 	7
#define DEFAULT_HEADER_WIDTH 	5

LAErrorCode LACreatePresetDataNode(LAPFDataNode **node, uint16_t length){
	LA_HANDLE_NULLPTR(node, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	uint8_t isNodeAllocated = 0;
	uint8_t isValueAllocated = 0;

	(*node) = (LAPFDataNode *)malloc(sizeof(LAPFDataNode));
	if((*node) == NULL){
		code = LA_ERROR_MALLOC;
		goto handle_error;
	}
	isNodeAllocated = 1;

	if(length > 0){
		(*node)->value = malloc(length);
		if((*node) == NULL){
			code = LA_ERROR_MALLOC;
			goto handle_error;
		}
		isValueAllocated = 1;
	} else {
		(*node)->value = NULL;
	}

	(*node)->tag = 0;
	(*node)->length = length;
	memset((*node)->value, 0, length);
	(*node)->prev = NULL;
	(*node)->next = NULL;

	return LA_NO_ERROR;

handle_error:
	if(isValueAllocated && isNodeAllocated){
		free((*node)->value);
		(*node)->value = NULL;
	}

	if(isNodeAllocated){
		free((*node));
		(*node) = NULL;
	}

	LA_RAISE_ERROR(code);
	return code;
}

LAErrorCode LADeletePresetDataNodes(LAPFDataNode **start){
	LA_HANDLE_NULLPTR(start, LA_PROPAGATE_ERROR);

	LAPFDataNode *node = (*start);
	LAPFDataNode *next = NULL;

	while(node != NULL){
		if(node->value != NULL){
			free(node->value);
			node->value = NULL;
		}
		next = node->next;
		free(node);
		node = next;
	}

	return LA_NO_ERROR;
}

const uint8_t LAPFMagicValue[LA_PF_MAGIC_LENGTH] = {
	'L', 'A', 'P', 'F'
};

uint8_t LAPresetCheckMagic(LAPresetFile *file){
	if(file == NULL) return 0;

	for(uint8_t i = 0; i < LA_PF_MAGIC_LENGTH; i++){
		if(file->header.magic[i] != LAPFMagicValue[i]) return 0;
	}

	return 1;
}

uint32_t LAPresetArrayToI32(uint8_t *data){
	if(data == NULL) return 0;

	uint32_t value = 0;
	value |= (data[0] << 0);
	value |= (data[1] << 8);
	value |= (data[2] << 16);
	value |= (data[3] << 24);

	return value;
}

uint16_t LAPresetArrayToI16(uint8_t *data){
	if(data == NULL) return 0;

	uint16_t value = 0;
	value |= (data[0] << 0);
	value |= (data[1] << 8);

	return value;
}

LAErrorCode LAPresetInsertDataNodeLast(LAPresetFile *preset, LAPFDataNode *node){
	LA_HANDLE_NULLPTR(preset,	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(node,		LA_PROPAGATE_ERROR);

	if(preset->dataStart == NULL){
		// First node
		preset->dataStart = node;
		preset->dataLast  = node;
	} else if (preset->dataLast != NULL){
		// n > 1 node
		preset->dataLast->next = node;
		preset->dataLast = node;
	} else {
		// Failsafe if there is some kind of corruption in dataLast
		preset->dataLast = preset->dataStart;
		while(preset->dataLast->next != NULL){
			preset->dataLast = preset->dataLast->next;
		}
		preset->dataLast->next = node;
		preset->dataLast = node;
	}

	return LA_NO_ERROR;
}

size_t LAPresetFileGetPadding(LAMappedFile *file){
	LA_HANDLE_NULLPTR(file, 0);

	size_t rawOffset = file->readOffset & 0x3;

	if(rawOffset == 0) return 0;

	return (4 - rawOffset);
}

#define LA_TLV_PREMATURE_ABORT 0x1
#define LA_TLV_NO_EXPLICIT_END 0x2

LAErrorCode LAPresetTLVParserFromFile(LAPresetFile *preset, LAMappedFile *file, uint8_t *flags){
	/*
		tag : 2 bytes
		length: 2 bytes
		value: 0 - 65535 bytes

		tag:
			0x0000 = NOP
			0xFFFF = END

		If can't read value, aborts prematurely

		length == 0 is supported, as NOP and END tags are zero-length
	*/

	LA_HANDLE_NULLPTR(preset, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(file, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(flags, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	LAPFDataNode *node;
	uint16_t tag 	= 0;
	uint16_t length = 0;

	// Set flag, can be unset if parser detect tag=0xFFFF (END)
	(*flags) = (*flags) | LA_TLV_NO_EXPLICIT_END;

	while(LAMappedFileCanRead(file, 4)){
		if(file->readOffset >= file->length) break;

		code = LAMappedFileReadI16(file, &tag);
		if(code) return code;
		code = LAMappedFileReadI16(file, &length);
		if(code) return code;

		// If file is shorter than offset+length, abort TLV parser
		// premature end in supported, but the last TLV is not copied
		// The parses tries its best effort to parse
		if(!LAMappedFileCanRead(file, length)){
			(*flags) = (*flags) | LA_TLV_PREMATURE_ABORT;
			break;
		}
		
		// Create new node, if there is an error, propagate
		code	= LACreatePresetDataNode(&node, length);
		if(code) return code;

		// Node is valid, copy tag and length
		node->tag = tag;
		node->length = length;
		code = LAMappedFileReadBytes(file, node->value, length, NULL, TRUE);
		if(code) return code;

		code = LAPresetInsertDataNodeLast(preset, node);
		if(code) return code;

		// Tag is explicit "END", so exit parser
		if(tag == LA_TAG_END) {
			(*flags) = (*flags) & ~(LA_TLV_NO_EXPLICIT_END);
			break;
		}
	}


	return LA_NO_ERROR;
}

LAErrorCode LAPresetParseFile(LAPresetFile *preset, LAMappedFile *file, uint8_t *flags){
	LA_HANDLE_NULLPTR(preset, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(file, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(flags, 	LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	if(!LAMappedFileCanRead(file, 4)) goto size_error;
	code = LAMappedFileReadBytes(file, preset->header.magic, 4, NULL, TRUE);
	if(code) goto handle_error;
	uint8_t isLAPF = LAPresetCheckMagic(preset);
	if(isLAPF == 0) goto invalid_file;

	if(!LAMappedFileCanRead(file, 10)) goto size_error;
	code = LAMappedFileReadI32(file, &(preset->header.version));
	if(code) goto handle_error;
	code = LAMappedFileReadI32(file, &(preset->header.pluginId));
	if(code) goto handle_error;
	code = LAMappedFileReadI16(file, &(preset->header.nameLength));
	if(code) goto handle_error;

	uint16_t nameLengthAdj  	= (preset->header.nameLength >= LA_PF_NAME_LENGTH) ? LA_PF_NAME_LENGTH : preset->header.nameLength;
	if(!LAMappedFileCanRead(file, nameLengthAdj)) goto size_error;
	memset(preset->header.name, '\0', LA_PF_NAME_LENGTH);
	code = LAMappedFileReadBytes(file, preset->header.name, nameLengthAdj, NULL, TRUE);
	if(code) goto handle_error;
	preset->header.name[LA_PF_NAME_LENGTH - 1] = '\0';
	
	if(!LAMappedFileCanRead(file, 8)) goto size_error;
	code = LAMappedFileReadI32(file, &(preset->header.crc32Header));
	if(code) goto handle_error;
	code = LAMappedFileReadI32(file, &(preset->header.crc32Data));
	if(code) goto handle_error;

	size_t padding = LAPresetFileGetPadding(file);
	code = LAMappedFileSeekBytes(file, padding, TRUE);
	if(code) goto handle_error;

	(*flags) = 0;
	code = LAPresetTLVParserFromFile(preset, file, flags);
	if(code) goto tlv_error;

	if((*flags) & LA_TLV_PREMATURE_ABORT)	LADialogWarningGeneric(lawp, "TLV Parser: File is shorter than expected. Maybe it is truncated?");
	if((*flags) & LA_TLV_NO_EXPLICIT_END)	LADialogWarningGeneric(lawp, "TLV Parser: No \"END\" (0xFFFF) tag found.");

	return LA_NO_ERROR;

tlv_error:
	size_t size = LA_BUFFER_SIZE;
	char buffer[LA_BUFFER_SIZE];
	snprintf(buffer, size, "TLV Parser: An error ocurred (key: %i)", code);
	LADialogErrorGeneric(lawp, buffer);	
	return code;

handle_error:
	return code;

size_error:
	LADialogErrorGeneric(lawp, "File size is invalid for parse. Maybe it is truncated?");
	LA_RAISE_ERROR(LA_ERROR_OUTOFRANGE);
	return LA_ERROR_OUTOFRANGE;

invalid_file:
	LADialogErrorGeneric(lawp, "FIle selected is not Preset file format.");
	LA_RAISE_ERROR(LA_ERROR_INCORRECTVALUE);
	return LA_ERROR_INCORRECTVALUE;
}

void LAOnPresetFileOpen(GtkWidget *widget, LAPresetEditor *lap){
	gchar *filename = LADialogOpenFile(lawp, "Open LA Preset File", ".lapf");
	if(filename == NULL) return;

	LAErrorCode code = LAMemoryMapFile(filename, &(lap->file));
	if(code) goto handle_free;

	lap->isFileOpen = 1;
	gtk_label_set_text(GTK_LABEL(lap->fileOpenFilename), filename);

	uint8_t flags = 0;
	code = LAPresetParseFile(&(lap->presetFile), &(lap->file), &flags);
	if(code != LA_NO_ERROR){
		g_free(filename);
		LAMemoryUnmapFile(&(lap->file));
		lap->isFileOpen = 0;
		return;
	}

	LAPresetUpdateUI(lap);
	(void) widget;

handle_free:
	g_free(filename);
}

void LAOnPresetDestroy(GtkWidget *widget, LAPresetEditor *lap){
	if (lap->isFileOpen == 0) return;

	LAMemoryUnmapFile(&(lap->file));
	LADeletePresetDataNodes(&(lap->presetFile.dataStart));
	(void) widget;
}

LAErrorCode LAPresetFileInit(LAPresetFile *file){
	LA_HANDLE_NULLPTR(file, LA_PROPAGATE_ERROR);

	memset(file->header.magic, ' ', LA_PF_MAGIC_LENGTH);
	file->header.version = 0;
	file->header.nameLength = 0;
	memset(file->header.name, 0, LA_PF_NAME_LENGTH);
	file->header.crc32Header = 0;
	file->header.crc32Data = 0;

	file->dataLength = 0;
	file->dataStart = NULL;
	file->dataLast = NULL;

	return LA_NO_ERROR;
}

LAErrorCode LAArrayToHexString(uint8_t *array, size_t arrayLength, char *output, size_t outputLength){
	LA_HANDLE_NULLPTR(array, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(output, 	LA_PROPAGATE_ERROR);

	size_t byte = 0;
	uint8_t nibble = 0;

	// Aborts if length is zero, ensures output is '\0' if arrayLength == 0
	memset(output, '\0', outputLength);
	if(arrayLength == 0) return LA_NO_ERROR;
	if(outputLength == 0) return LA_NO_ERROR;


	for(size_t i = 0; i < outputLength; i++){
		if(byte >= arrayLength) break;
		switch(i % 3){
			case 0:	nibble = array[byte] >> 4;
					output[i] = (nibble > 9) ? (nibble - 10 + 'A') : (nibble + '0');
					break;
			case 1:	nibble = array[byte] & 0xF;
					output[i] = (nibble > 9) ? (nibble - 10 + 'A') : (nibble + '0');
					break;
			case 2:	output[i] = (byte < (arrayLength - 1)) ? ' ' : '\0';
					byte++;
					break;
		}
	}

	output[outputLength - 1] = '\0';

	return LA_NO_ERROR;
}

LAErrorCode LAPresetStoreClear(LAPresetEditor *lap){
	LA_HANDLE_NULLPTR(lap, LA_PROPAGATE_ERROR);

	gtk_list_store_clear(GTK_LIST_STORE(lap->dataStore));

	return LA_NO_ERROR;
}

LAErrorCode LAPresetStoreAppendNode(LAPresetEditor *lap, LAPFDataNode *node, size_t index){
	LA_HANDLE_NULLPTR(lap, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(node, LA_PROPAGATE_ERROR);

	char buffer[LA_BUFFER_SIZE];
	LAErrorCode code = LAArrayToHexString(node->value, node->length, buffer, LA_BUFFER_SIZE);
	if(code) return code;

	gtk_list_store_append(lap->dataStore, &(lap->dataIter));
 	gtk_list_store_set(lap->dataStore, &(lap->dataIter),
                     0, (guint) index,
                     1, (guint) node->tag,
                     2, (gchar *) "Undef",	 /* TODO: Name handler */
                     3, (guint) node->length,
                     4,  (gchar *) buffer,
                     -1);

	return LA_NO_ERROR;
}

LAErrorCode LAPresetUpdateUI(LAPresetEditor *lap){
	LA_HANDLE_NULLPTR(lap, 	LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	char buffer[LA_BUFFER_SIZE];
	LAPresetFile file = lap->presetFile;

	snprintf(buffer, LA_BUFFER_SIZE, "'%c', '%c', '%c', '%c'", file.header.magic[0], file.header.magic[1], file.header.magic[2], file.header.magic[3]);
	gtk_label_set_text(GTK_LABEL(lap->headerMagicValue), buffer);

	snprintf(buffer, LA_BUFFER_SIZE, "v%i.%i.%i", file.header.version >> 24, (file.header.version >> 16) & 0xFF, (file.header.version & 0xFFFF));
	gtk_label_set_text(GTK_LABEL(lap->headerVersionValue), buffer);

	snprintf(buffer, LA_BUFFER_SIZE, "%i bytes", file.header.nameLength);
	gtk_label_set_text(GTK_LABEL(lap->headerNameLengthValue), buffer);

	size_t nameLength = (file.header.nameLength >= LA_PF_NAME_LENGTH) ? (LA_PF_NAME_LENGTH - 1) : file.header.nameLength;
	memcpy(buffer, file.header.name, nameLength);
	buffer[nameLength] = '\0';
	gtk_entry_set_text(GTK_ENTRY(lap->headerNameValue), buffer);

	snprintf(buffer, LA_BUFFER_SIZE, "0x%08X" , file.header.crc32Header);
	gtk_label_set_text(GTK_LABEL(lap->headerCRC32AValue), buffer);

	snprintf(buffer, LA_BUFFER_SIZE, "0x%08X" , file.header.crc32Data);
	gtk_label_set_text(GTK_LABEL(lap->headerCRC32BValue), buffer);

	snprintf(buffer, LA_BUFFER_SIZE, "%i bytes" , file.dataLength);
	gtk_label_set_text(GTK_LABEL(lap->dataLengthValue), buffer);

	LAPresetStoreClear(lap);

	// No TLV found, still a valid file if there is no nodes
	if(file.dataStart == NULL) return LA_NO_ERROR;
	
	LAPFDataNode *node = file.dataStart;
	uint32_t counter = 1;
	while(node != NULL){
		code = LAPresetStoreAppendNode(lap, node, counter);
		if(code) return code;

		counter++;
		node = node->next;
	}

	return LA_NO_ERROR;
}


LAErrorCode LAOpenPresetEditorWindow(LAPresetEditor *lap, LAWindow *law){

	lap->window = gtk_dialog_new_with_buttons(
					"LAPF Preset Editor",
					GTK_WINDOW(law->window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					"_Quit", GTK_RESPONSE_CANCEL,
					NULL
				);

	lap->isFileOpen = 0;
	LAPresetFileInit(&(lap->presetFile));

	gtk_window_set_modal(GTK_WINDOW(lap->window), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(lap->window), 8);
	lap->content = gtk_dialog_get_content_area(GTK_DIALOG(lap->window));

	lap->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(lap->content), GTK_WIDGET(lap->vbox));

	lap->frameFile = gtk_frame_new("File:");
	gtk_container_add(GTK_CONTAINER(lap->vbox), GTK_WIDGET(lap->frameFile));
	lap->hboxFile = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lap->frameFile), GTK_WIDGET(lap->hboxFile));
	gtk_container_set_border_width(GTK_CONTAINER(lap->hboxFile), 8);

	lap->frameHeader = gtk_frame_new("Header:");
	gtk_container_add(GTK_CONTAINER(lap->vbox), GTK_WIDGET(lap->frameHeader));
	lap->gridHeader = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(lap->frameHeader), GTK_WIDGET(lap->gridHeader));
	gtk_container_set_border_width(GTK_CONTAINER(lap->gridHeader), 8);

	lap->frameData = gtk_frame_new("Data:");
	gtk_container_add(GTK_CONTAINER(lap->vbox), GTK_WIDGET(lap->frameData));
	lap->vboxData = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(lap->frameData), GTK_WIDGET(lap->vboxData));
	gtk_container_set_border_width(GTK_CONTAINER(lap->vboxData), 8);

	lap->fileOpenButton		= gtk_button_new_with_label("Open file");
	lap->fileOpenFilename	= gtk_label_new("No file selected");
	gtk_container_add(GTK_CONTAINER(lap->hboxFile), GTK_WIDGET(lap->fileOpenButton));
	gtk_container_add(GTK_CONTAINER(lap->hboxFile), GTK_WIDGET(lap->fileOpenFilename));

	lap->headerMagicLabel		= gtk_label_new("Magic number:");
	gtk_label_set_xalign(GTK_LABEL(lap->headerMagicLabel), 0.0);
	lap->headerMagicValue		= gtk_label_new(" ");
	gtk_label_set_xalign(GTK_LABEL(lap->headerMagicValue), 0.0);
	lap->headerVersionLabel		= gtk_label_new("Version:");
	gtk_label_set_xalign(GTK_LABEL(lap->headerVersionLabel), 0.0);
	lap->headerVersionValue		= gtk_label_new(" ");
	gtk_label_set_xalign(GTK_LABEL(lap->headerVersionValue), 0.0);
	lap->headerPluginIdLabel	= gtk_label_new("Plugin Id:");
	gtk_label_set_xalign(GTK_LABEL(lap->headerPluginIdLabel), 0.0);
	lap->headerPluginIdValue 	= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lap->headerPluginIdValue), "Filter editor");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lap->headerPluginIdValue), "Waveform editor");
	gtk_combo_box_set_active(GTK_COMBO_BOX(lap->headerPluginIdValue), 0);
	lap->headerNameLabel		= gtk_label_new("Name:");
	gtk_label_set_xalign(GTK_LABEL(lap->headerNameLabel), 0.0);
	lap->headerNameValue		= gtk_entry_new();
	lap->headerNameApply		= gtk_button_new_with_label("Change");
	lap->headerNameLengthLabel	= gtk_label_new("Name Length:");
	gtk_label_set_xalign(GTK_LABEL(lap->headerNameLengthLabel), 0.0);
	lap->headerNameLengthValue	= gtk_label_new(" ");
	gtk_label_set_xalign(GTK_LABEL(lap->headerNameLengthValue), 0.0);
	lap->headerCRC32ALabel		= gtk_label_new("Header CRC32:");
	gtk_label_set_xalign(GTK_LABEL(lap->headerCRC32ALabel), 0.0);
	lap->headerCRC32AValue		= gtk_label_new(" ");
	gtk_label_set_xalign(GTK_LABEL(lap->headerCRC32AValue), 0.0);
	lap->headerCRC32BLabel		= gtk_label_new("Data CRC32:");
	gtk_label_set_xalign(GTK_LABEL(lap->headerCRC32BLabel), 0.0);
	lap->headerCRC32BValue		= gtk_label_new(" ");
	gtk_label_set_xalign(GTK_LABEL(lap->headerCRC32BValue), 0.0);

	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerMagicLabel), 		0, 0, 3, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerMagicValue), 		3, 0, 4, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerVersionLabel), 	0, 1, 3, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerVersionValue),		3, 1, 4, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerPluginIdLabel), 	0, 2, 3, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerPluginIdValue), 	3, 2, 4, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerNameLabel), 		0, 3, 3, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerNameValue), 		3, 3, 8, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerNameApply), 		11,3, 2, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerNameLengthLabel), 	0, 4, 3, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerNameLengthValue),	3, 4, 4, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerCRC32ALabel), 		0, 5, 3, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerCRC32AValue), 		3, 5, 4, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerCRC32BLabel), 		0, 6, 3, 1);
	gtk_grid_attach(GTK_GRID(lap->gridHeader), GTK_WIDGET(lap->headerCRC32BValue), 		3, 6, 4, 1);

	lap->dataLengthHbox 	= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lap->vboxData), GTK_WIDGET(lap->dataLengthHbox));
	lap->dataLengthLabel	= gtk_label_new("Data length:");
	gtk_label_set_xalign(GTK_LABEL(lap->dataLengthLabel), 0.0);
	lap->dataLengthValue	= gtk_label_new(" ");
	gtk_label_set_xalign(GTK_LABEL(lap->dataLengthValue), 0.0);
	gtk_container_add(GTK_CONTAINER(lap->dataLengthHbox), GTK_WIDGET(lap->dataLengthLabel));
	gtk_container_add(GTK_CONTAINER(lap->dataLengthHbox), GTK_WIDGET(lap->dataLengthValue));

	lap->dataView			= gtk_tree_view_new();
	lap->dataStore			= gtk_list_store_new(5, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_STRING);

	lap->dataRenderer		= gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(lap->dataView), 0, "Index", lap->dataRenderer, "text", 0, NULL);
	lap->dataRenderer		= gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(lap->dataView), 1, "Tag Id", lap->dataRenderer, "text", 1, NULL);
	lap->dataRenderer		= gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(lap->dataView), 2, "Tag name", lap->dataRenderer, "text", 2, NULL);
	lap->dataRenderer		= gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(lap->dataView), 3, "Length", lap->dataRenderer, "text", 3, NULL);
	lap->dataRenderer		= gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(lap->dataView), 4, "Value", lap->dataRenderer, "text", 4, NULL);
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(lap->dataView), GTK_TREE_MODEL(lap->dataStore));
	g_object_unref(lap->dataStore);

	lap->dataContainer		= gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(lap->dataContainer), lap->dataView);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(lap->dataContainer), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(lap->vboxData), GTK_WIDGET(lap->dataContainer));
	gtk_widget_set_size_request(lap->dataContainer, 512, 128);

/*
	for(uint16_t i = 0; i < 32; i++){
	}
*/
	lap->hboxData 		= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(lap->vboxData), GTK_WIDGET(lap->hboxData));
	lap->dataType		= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lap->dataType), "0: No Operation");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lap->dataType), "65535: End");
	gtk_combo_box_set_active(GTK_COMBO_BOX(lap->dataType), 0);
	lap->dataValue		= gtk_entry_new();
	lap->dataButtonAdd	= gtk_button_new_with_label("Add");
	gtk_container_add(GTK_CONTAINER(lap->hboxData), GTK_WIDGET(lap->dataType));
	gtk_container_add(GTK_CONTAINER(lap->hboxData), GTK_WIDGET(lap->dataValue));
	gtk_container_add(GTK_CONTAINER(lap->hboxData), GTK_WIDGET(lap->dataButtonAdd));

	LAPresetUpdateUI(lap);

	g_signal_connect(lap->fileOpenButton, 			"clicked", 				G_CALLBACK(LAOnPresetFileOpen), 				lap);
	g_signal_connect(lap->window, 					"destroy", 				G_CALLBACK(LAOnPresetDestroy), 					lap);

	gtk_widget_show_all(lap->window);
	gtk_dialog_run(GTK_DIALOG(lap->window));
	gtk_widget_destroy(lap->window);

	return LA_NO_ERROR;
}

void LAOpenPresetEditor(GtkWidget *widget, LAWindow *law){
	LAPresetEditor lap;
	LAOpenPresetEditorWindow(&lap, law);
	(void) widget;
}
