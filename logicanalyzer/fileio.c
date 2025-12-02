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

#define HEADER_START_MAGIC 	0x7F4C4170u
#define HEADER_END_MAGIC 	0x7F4C4177u
#define LA_FILE_END_MAGIC 	0xFF4C41F0u

#define MAX_VIRTUAL_BUFFER_SIZE 1024
#define MIN_VIRTUAL_BUFFER_SIZE 1

typedef enum {
	LA_CF_DONT_WRITE			= 0,
	LA_CF_SHOW_RELATIVE_TIME	= 1,
	LA_CF_SHOW_ABSOLUTE_TIME	= 2,
	LA_CF_SHOW_RELATIVE_SAMPLES	= 3,
	LA_CF_SHOW_ABSOLUTE_SAMPLES	= 4,
	LA_CF_SHOW_BUFFER_END		= 5,
	LA_CF_SHOW_BUFFER_RULER		= 6,
	LA_CF_SHOW_CLOCK_RULER		= 7,
	LA_CF_SCROLL				= 8
} LAConfigHeaderFlags;

#define LA_CONVERT_TO_FLAG(__var, __pos) (((__var) & 0x1) << (__pos))
#define LA_POS_READ_ENABLE 15
#define LA_POS_IS_MUTED 4
#define LA_VERSION_NUMBER 1
#define LA_CHANNEL_START_MAGIC 0xC0
#define LA_CHANNEL_END_MAGIC 0xDF

#define LA_CONVERT_COLOR_TO_UINT8(__color) ((uint8_t) (__color * 255))

LAErrorCode LAGetConfigHeader(LAWindow *law, LAConfigHeader *lac){
	LA_HANDLE_NULLPTR(law, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(lac, LA_PROPAGATE_ERROR);

	lac->zoom		  = law->rd.zoom;
	lac->device		  = law->connect.internalDeviceId;
	lac->clockTime    = 0;
	lac->pollingTime  = law->rd.pollingTime;
	lac->readDetails  = law->rd.virtualBufferSize % MAX_VIRTUAL_BUFFER_SIZE;
	lac->readDetails |= LA_CONVERT_TO_FLAG(1, LA_POS_READ_ENABLE);
	lac->flags		  = 0;
	lac->flags		 |= LA_CONVERT_TO_FLAG(law->rd.dontWrite, 			LA_CF_DONT_WRITE);
	lac->flags		 |= LA_CONVERT_TO_FLAG(law->rd.showRelativeTime, 	LA_CF_SHOW_RELATIVE_TIME);
	lac->flags		 |= LA_CONVERT_TO_FLAG(law->rd.showAbsoluteTime, 	LA_CF_SHOW_ABSOLUTE_TIME);
	lac->flags		 |= LA_CONVERT_TO_FLAG(law->rd.showRelativeSample, 	LA_CF_SHOW_RELATIVE_SAMPLES);
	lac->flags		 |= LA_CONVERT_TO_FLAG(law->rd.showAbsoluteSample,	LA_CF_SHOW_ABSOLUTE_TIME);
	lac->flags		 |= LA_CONVERT_TO_FLAG(law->rd.showBufferEnd, 		LA_CF_SHOW_BUFFER_END);
	lac->flags		 |= LA_CONVERT_TO_FLAG(law->rd.showBufferRuler, 	LA_CF_SHOW_BUFFER_RULER);
	lac->flags		 |= LA_CONVERT_TO_FLAG(law->rd.showClockRuler, 		LA_CF_SHOW_CLOCK_RULER);
	lac->flags		 |= LA_CONVERT_TO_FLAG(law->rd.addDataOffset, 		LA_CF_SCROLL);
	lac->captureChannelMode	= law->bd.dataMode;
	lac->captureChannelMask	= law->bd.dataMask;

	return LA_NO_ERROR;
}

LAErrorCode LASetMagicConfigHeader(LAConfigHeader *lac){
	LA_HANDLE_NULLPTR(lac, LA_PROPAGATE_ERROR);
	
	lac->headerStart 	= HEADER_START_MAGIC;
	lac->fileVersion	= LA_VERSION_NUMBER;
	lac->headerEnd 		= HEADER_END_MAGIC;
	
	return LA_NO_ERROR;
}

LAErrorCode LASetMagicConfigChannel(LAConfigChannel *lac){
	LA_HANDLE_NULLPTR(lac, LA_PROPAGATE_ERROR);
	
	lac->startByte 	= LA_CHANNEL_START_MAGIC;
	lac->endByte 	= LA_CHANNEL_END_MAGIC;
	
	return LA_NO_ERROR;
}

LAErrorCode LAGetChannelConfig(LAChannel *lac, LAConfigChannel *lax){
	LA_HANDLE_NULLPTR(lac, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(lax, LA_PROPAGATE_ERROR);

	lax->channel 	 = lac->bit;
	lax->channel 	|= LA_CONVERT_TO_FLAG(lac->muted, LA_POS_IS_MUTED);
	lax->r			 = LA_CONVERT_COLOR_TO_UINT8(lac->r);
	lax->g			 = LA_CONVERT_COLOR_TO_UINT8(lac->g);
	lax->b			 = LA_CONVERT_COLOR_TO_UINT8(lac->b);

	//g_print("Before name\n");
	const gchar *name= gtk_entry_get_text(GTK_ENTRY(lac->entry));
	//g_print("Name: %p - %s - %li\n", name, name, strlen(name));
	if(name == NULL || strlen(name) == 0){
		lax->nameLength = 0;
		lax->name = NULL;
		//g_print("No name, skipping\n");
		return LA_NO_ERROR;
	}

	lax->nameLength	 = strlen(name);
	lax->name		 = malloc(lax->nameLength + 1);
	if(lax->name == NULL){
		LA_RAISE_ERROR(LA_ERROR_MALLOC);
		return LA_ERROR_MALLOC;
	}

	strncpy(lax->name, name, lax->nameLength);
	return LA_NO_ERROR;
}

LAErrorCode LACreateConfigHeader(LAWindow *law, LAConfigHeader *lac){
	LA_HANDLE_NULLPTR(law, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(lac, LA_PROPAGATE_ERROR);
	
	LAErrorCode response = LA_NO_ERROR;

	response = LASetMagicConfigHeader(lac);
	if(response != LA_NO_ERROR) return response;

	response = LAGetConfigHeader(law, lac);
	if(response != LA_NO_ERROR) return response;

	return LA_NO_ERROR;
}

gboolean LASaveSession(GtkWidget *widget, LAWindow *law){
	gchar *filename = LADialogSaveFile(law, "Save config file", "LogicAnalyzer Config file .lac");
	if(filename == NULL){
		return TRUE;
	}

	LAConfigHeader lac;
	LAConfigChannel lax[MAX_CHANNEL_COUNT];

	LACreateConfigHeader(law, &lac);
	for(uint8_t i = 0; i < MAX_CHANNEL_COUNT; i++){
		LASetMagicConfigChannel(&(lax[i]));
		LAGetChannelConfig(&(law->channel[i]), &(lax[i]));
	}

	FILE *fp = fopen(filename, "wb+");
	if(fp == NULL) goto cleanup;

	fwrite(&lac, 1, sizeof(LAConfigHeader), fp);
	for(uint8_t i = 0; i < MAX_CHANNEL_COUNT; i++){
		fwrite(&(lax[i].startByte), 1, sizeof(uint8_t), fp);
		fwrite(&(lax[i].channel), 	1, sizeof(uint8_t), fp);
		fwrite(&(lax[i].r), 		1, sizeof(uint8_t), fp);
		fwrite(&(lax[i].g), 		1, sizeof(uint8_t), fp);
		fwrite(&(lax[i].b), 		1, sizeof(uint8_t), fp);
		fwrite(&(lax[i].nameLength),1, sizeof(uint8_t), fp);
		if(lax[i].nameLength != 0 && lax[i].name != NULL){
			fwrite(lax[i].name,      lax[i].nameLength, sizeof(char), fp);
		}
		fwrite(&(lax[i].endByte), 1, sizeof(uint8_t), fp);
	}
	
	uint32_t fileEndMagic = LA_FILE_END_MAGIC;
	fwrite(&(fileEndMagic), 1, sizeof(uint32_t), fp);
	fclose(fp);
	
cleanup:
	for(uint8_t i = 0; i < MAX_CHANNEL_COUNT; i++){
		if(lax[i].name != NULL){
			free(lax[i].name);
		}
	}

	g_print("AAA %i\n", 0);
	if(filename != NULL){
		g_free(filename);
	}

	return TRUE;
}
