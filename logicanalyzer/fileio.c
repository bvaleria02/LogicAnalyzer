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
#define LA_CONVERT_FROM_FLAG(__var, __pos) (((__var) >> (__pos)) & 0x1)
#define LA_POS_READ_ENABLE 15
#define LA_POS_IS_MUTED 4
#define LA_VERSION_NUMBER 1
#define LA_CHANNEL_START_MAGIC 0xC0
#define LA_CHANNEL_END_MAGIC 0xDF

#define LA_CONVERT_COLOR_TO_UINT8(__color) ((uint8_t) (__color * 255))
#define LA_CONVERT_COLOR_TO_DOUBLE(__color) ((double) (__color / (double) 255))

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

LAErrorCode LAReadFileBytes(FILE *fp, size_t bytes, uint32_t *dest){;
	LA_HANDLE_NULLPTR(fp, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(dest, LA_PROPAGATE_ERROR);

	if(bytes > sizeof(uint32_t)) bytes = sizeof(uint32_t);

	size_t bytesRead = fread(dest, 1, bytes, fp);
	if(bytesRead < bytes){
		LA_RAISE_ERROR(LA_ERROR_FILEREAD);
		return LA_ERROR_FILEREAD;
	}
	
	return LA_NO_ERROR;
}

uint32_t LAReadFile32(FILE *fp){
	uint32_t value = 0;
	LAReadFileBytes(fp, sizeof(uint32_t), &value);
	return value;
}

uint16_t LAReadFile16(FILE *fp){
	uint32_t value = 0;
	LAReadFileBytes(fp, sizeof(uint16_t), &value);
	return (uint16_t) value;
}

uint8_t LAReadFile8(FILE *fp){
	uint32_t value = 0;
	LAReadFileBytes(fp, sizeof(uint8_t), &value);
	return (uint8_t) value;
}

#define LA_NO_MATCH_READ(__fp, __varRead, __size, __output) do{					\
	la_errno = LA_NO_ERROR;														\
																				\
	switch(__size){																\
		case 1:	__varRead = LAReadFile8(__fp);									\
				break;															\
		case 2:	__varRead = LAReadFile16(__fp);									\
				break;															\
		case 4:	__varRead = LAReadFile32(__fp);									\
				break;															\
	}																			\
																				\
	if(la_errno != LA_NO_ERROR){												\
		goto cleanup;															\
	}																			\
																				\
	if((__output) != NULL){														\
		(*(__output)) = __varRead;												\
	}																			\
} while(0)

#define LA_EXACT_MATCH_READ(__fp, __varRead, __size, __value, __output) do{		\
	LA_NO_MATCH_READ(__fp, __varRead, __size, __output);						\
																				\
	if(__varRead != __value){													\
		LA_RAISE_ERROR(LA_ERROR_INCORRECTVALUE);								\
		goto cleanup;															\
	}																			\
} while(0)

#define LA_HANDLE_NULLPTR_CLEANUP(__ptr, __label) do{							\
	if((__ptr) == NULL){															\
		LA_RAISE_ERROR(LA_ERROR_NULLPTR);										\
		goto __label;															\
	}																			\
} while(0)

LAErrorCode LAApplyChangesFromConfig(LAWindow *law, LAConfigHeader *lah, LAConfigChannel *lac){
	LA_HANDLE_NULLPTR(law, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(lac, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(lah, LA_PROPAGATE_ERROR);

	law->rd.zoom = lah->zoom;
	law->rd.pollingTime = lah->pollingTime;
	law->rd.virtualBufferSize = lah->readDetails & 0x1FFF;
	law->rd.dontWrite 			= LA_CONVERT_FROM_FLAG(lah->flags, 		LA_CF_DONT_WRITE);
	law->rd.showRelativeTime 	= LA_CONVERT_FROM_FLAG(lah->flags, 		LA_CF_SHOW_RELATIVE_TIME);
	law->rd.showAbsoluteTime 	= LA_CONVERT_FROM_FLAG(lah->flags, 		LA_CF_SHOW_ABSOLUTE_TIME);
	law->rd.showRelativeSample 	= LA_CONVERT_FROM_FLAG(lah->flags, 		LA_CF_SHOW_RELATIVE_SAMPLES);
	law->rd.showAbsoluteSample 	= LA_CONVERT_FROM_FLAG(lah->flags, 		LA_CF_SHOW_ABSOLUTE_SAMPLES);
	law->rd.showBufferEnd 		= LA_CONVERT_FROM_FLAG(lah->flags, 		LA_CF_SHOW_BUFFER_END);
	law->rd.showBufferRuler 	= LA_CONVERT_FROM_FLAG(lah->flags, 		LA_CF_SHOW_BUFFER_RULER);
	law->rd.showClockRuler 		= LA_CONVERT_FROM_FLAG(lah->flags, 		LA_CF_SHOW_CLOCK_RULER);
	law->rd.addDataOffset 		= LA_CONVERT_FROM_FLAG(lah->flags, 		LA_CF_SCROLL);
	law->bd.dataMode			= lah->captureChannelMode;
	law->bd.dataMask			= lah->captureChannelMask;

	for(uint8_t i = 0; i < MAX_CHANNEL_COUNT; i++){
		LA_HANDLE_NULLPTR(&(lac[i]), LA_PROPAGATE_ERROR);
		law->channel[i].bit 	= lac[i].channel & 0x7;
		law->channel[i].muted 	= LA_CONVERT_FROM_FLAG(lac[i].channel, LA_POS_IS_MUTED);
		law->channel[i].r 		= LA_CONVERT_COLOR_TO_DOUBLE(lac[i].r);
		law->channel[i].g 		= LA_CONVERT_COLOR_TO_DOUBLE(lac[i].g);
		law->channel[i].b		= LA_CONVERT_COLOR_TO_DOUBLE(lac[i].b);

		if(lac[i].nameLength > 0 && lac[i].name != NULL){
			gtk_entry_set_text(GTK_ENTRY(law->channel[i].entry), lac[i].name);
		}
	}

	return LA_NO_ERROR;
}

gboolean LALoadSession(GtkWidget *widget, LAWindow *law){
	gchar *filename;
	LAConfigHeader lah;
	LAConfigChannel lac[MAX_CHANNEL_COUNT];
	uint32_t value = 0;
	uint8_t hasSelectedFile = 0;
	FILE *fp = NULL;
	for(uint8_t i = 0; i < MAX_CHANNEL_COUNT; i++){
		lac[i].name = NULL;
	}

	filename = LADialogOpenFile(law, "Load config file", "LogicAnalyzer Config file .lac");
	LA_HANDLE_NULLPTR_CLEANUP(filename, cleanup);

	hasSelectedFile = 1;
	fp = fopen(filename, "rb");
	LA_HANDLE_NULLPTR_CLEANUP(fp, cleanup);

	LA_EXACT_MATCH_READ(fp, value, 4, HEADER_START_MAGIC, &(lah.headerStart));
	LA_EXACT_MATCH_READ(fp, value, 2, LA_VERSION_NUMBER,  &(lah.fileVersion));
	LA_NO_MATCH_READ(   fp, value, 1,                     &(lah.device));
	LA_NO_MATCH_READ(   fp, value, 1,                     &(lah.zoom));
	LA_NO_MATCH_READ(   fp, value, 4,                     &(lah.flags));
	LA_NO_MATCH_READ(   fp, value, 4,                     &(lah.clockTime));
	LA_NO_MATCH_READ(   fp, value, 4,                     &(lah.pollingTime));
	LA_NO_MATCH_READ(   fp, value, 2,                     &(lah.readDetails));
	LA_NO_MATCH_READ(   fp, value, 1,                     &(lah.captureChannelMode));
	LA_NO_MATCH_READ(   fp, value, 1,                     &(lah.captureChannelMask));
	LA_EXACT_MATCH_READ(fp, value, 4, HEADER_END_MAGIC,   &(lah.headerEnd));

	for(uint8_t i = 0; i < MAX_CHANNEL_COUNT; i++){
		LA_EXACT_MATCH_READ(fp, value, 1, LA_CHANNEL_START_MAGIC, &(lac[i].startByte));
		LA_NO_MATCH_READ(   fp, value, 1,						  &(lac[i].channel));
		LA_NO_MATCH_READ(   fp, value, 1,						  &(lac[i].r));
		LA_NO_MATCH_READ(   fp, value, 1,						  &(lac[i].g));
		LA_NO_MATCH_READ(   fp, value, 1,						  &(lac[i].b));
		LA_NO_MATCH_READ(   fp, value, 1,						  &(lac[i].nameLength));
		
		if(lac[i].nameLength > 0){
			lac[i].name = malloc(lac[i].nameLength + 1);
			LA_HANDLE_NULLPTR_CLEANUP(lac[i].name, cleanup);
			fread(lac[i].name, lac[i].nameLength, sizeof(char), fp);
			lac[i].name[lac[i].nameLength] = '\0';
		} else {
			lac[i].name = NULL;
		}

		LA_EXACT_MATCH_READ(fp, value, 1, LA_CHANNEL_END_MAGIC,   &(lac[i].endByte));
	}

	uint32_t nullAhPointer = 0;
	LA_EXACT_MATCH_READ(fp, value, 4, LA_FILE_END_MAGIC, &nullAhPointer);
	LAApplyChangesFromConfig(law, &lah, lac);

cleanup:
	if(la_errno != LA_NO_ERROR){
		g_print("An error ocurred:\n");
		g_print("\terror code: %i\n", la_errno);
		g_print("\tfunction: %s\n", la_funcname);
		g_print("\tfile: %s\n", la_filename);
		g_print("\tline: %i\n", la_linenumber);
		g_print("\n");
		if(hasSelectedFile) LADialogErrorGeneric(law, "Error reading config file\n");
	} else {
		g_print("Everything is ok on file read\n");
	}

	for(uint8_t i = 0; i < MAX_CHANNEL_COUNT; i++){
		if(lac[i].name != NULL) free(lac[i].name);
	}

	if(fp != NULL) fclose(fp);
	if(filename != NULL) g_free(filename);
	return TRUE;
}
