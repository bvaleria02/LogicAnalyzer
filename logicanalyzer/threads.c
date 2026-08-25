#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <pthread.h>
#include <stdatomic.h>

#include "liblogicanalyzer.h"
#include "error.h"
#include "utils.h"
#include "types.h"
#include "threads.h"
#include "gtk_funcs.h"
#include "bucket.h"
#include "serial.h"
#include "enums.h"
#include "threads/ack.h"

void LAHandleBucketWrite(uint8_t *buffer, int16_t size){
	pthread_mutex_lock(&(lawp->mutexes.bucketAccess));
	
	for(int16_t i = 0; i < size; i++){
		LABucketInsertData(&(lawp->bd.bucketCurrent), buffer[i] & lawp->bd.dataMask);	
	}

	pthread_mutex_unlock(&(lawp->mutexes.bucketAccess));
}

[[maybe_unused]] static void LAWriteDataToDataBuffer(LAWindow *law, uint8_t *buffer, int16_t numBytes, uint16_t *index){
	pthread_mutex_lock(&(law->mutexes.dataBufferAccess));

	for(uint16_t i = 0; i < numBytes; i++){
		law->dataBuffer[((*index) + i) % LA_LARGE_BUFFER_SIZE] = buffer[i];
	}

	(*index) = ((*index) + numBytes) % LA_LARGE_BUFFER_SIZE;
	pthread_mutex_unlock(&(law->mutexes.dataBufferAccess));
}

LAErrorCode LADebugPrintHex(uint8_t *buffer, size_t length){
	LA_CHECK_NULLPTR(buffer);

	printf("Address          | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F \n");
	printf("-----------------|-------------------------------------------------\n");
	
	for(size_t i = 0; i < length; i++){
		if((i % 0x10) == 0x0){
			printf("\n%016lX | ", i);
		}
		printf("%02X ", buffer[i]);
	}
	
	printf("End\n\n");
		
	return LA_NO_ERROR;
}

typedef struct {
	LASerialV2RecvProtocol data;
	size_t cursor;
} LAPacketParser;

LAErrorCode LAPacketParserClear(LAPacketParser *p){
	LA_CHECK_NULLPTR(p);
	
	p->data.command = 0;
	p->data.length = 0;
	p->data.fcs = 0;
	memset(p->data.data,   0l, LA_SERIAL_V2_DATA_LENGTH);
	memset(p->data.frames, 0l, LA_SERIAL_V2_FRAME_LENGTH);

	p->cursor= 0;
	
	return LA_NO_ERROR;
}

LAErrorCode LAPacketParserInit(LAPacketParser *p){
	LA_CHECK_NULLPTR(p);

	LAErrorCode code = LA_NO_ERROR;
	code = LAPacketParserClear(p);

 	return code;
}

LAErrorCode LAPacketParserParse(LAPacketParser *parser, uint8_t *buffer, size_t length, size_t *pBytesParsed, bool *pIsDone){
	LA_CHECK_NULLPTR(parser);
	LA_CHECK_NULLPTR(buffer);
	LA_CHECK_NULLPTR(pBytesParsed);
	LA_CHECK_NULLPTR(pIsDone);

	for(size_t i = (*pBytesParsed); i < length; i++){
	//	printf("Length: %li\tBytes parsed: %li\tCursor: %li\n", length, (*pBytesParsed), parser->cursor);
	//	printf("Command: %i\tLength: %i\n", parser->data.command, parser->data.length);
		(*pBytesParsed) += 1;
		
		if(parser->cursor == 0){
			parser->data.command = buffer[i];
		} else if (parser->cursor == 1){
			parser->data.length = buffer[i];
		} else if (parser->cursor == 2){
			parser->data.length |= ((uint16_t)(buffer[i]) << 8);
			if(parser->data.length > LA_SERIAL_V2R_DATA_LENGTH){
				return LA_ERROR_PARSER_LENGTH;
			}
		} else if ((parser->cursor > 2) && (parser->cursor < (size_t)(3 + parser->data.length))){
			size_t dataIndex = parser->cursor - 3;
			parser->data.data[dataIndex] = buffer[i];
		} else if (parser->cursor == (size_t)(3 + parser->data.length)){
			parser->data.fcs = buffer[i];
		} else if (parser->cursor == (size_t)(4 + parser->data.length)){
			parser->data.fcs |= ((uint16_t)(buffer[i]) << 8);
			(*pIsDone) = true;
			break;
		}

		parser->cursor += 1;
	}
	
	return LA_NO_ERROR;
}

#define LA_TIMEOUT_VALUE 3.0

bool LAHasTimeout(struct timespec *lastTime, double *pTimeDelta){
	struct timespec currentTime;
	clock_gettime(CLOCK_MONOTONIC, &currentTime);

	int64_t seconds = currentTime.tv_sec  - lastTime->tv_sec;
	int64_t nano    = currentTime.tv_nsec - lastTime->tv_nsec;

	double sTimeDelta = seconds + ((nano) / (double) 1000000000);
	if(pTimeDelta != NULL) (*pTimeDelta) = sTimeDelta;

	return (sTimeDelta >= LA_TIMEOUT_VALUE);
}

LAErrorCode LAProtocolV2RPrint(LASerialV2RecvProtocol *p){
	LA_CHECK_NULLPTR(p);

	LAErrorCode code = LA_NO_ERROR;

	printf("\tCommand: %i\n", p->command);
	printf("\tLength: %i\n", p->length);
	printf("\tFCS: %i\n", p->fcs);
	printf("\tData:\n");
	code = LADebugPrintHex(p->data, p->length);		
				
	return code;
}


LAErrorCode LACalculateFCS_V2R(LASerialV2RecvProtocol *p, uint16_t *fcs){
	LA_CHECK_NULLPTR(p);
	LA_CHECK_NULLPTR(fcs);

	uint16_t crc16 = 0xFFFF;

	crc16 = LAModbusCRC16(crc16, p->command,              true);
	crc16 = LAModbusCRC16(crc16, p->length & 0xFF,        false);
	crc16 = LAModbusCRC16(crc16, (p->length >> 8) & 0xFF, false);

	for(size_t i = 0; i < p->length; i++){
		crc16 = LAModbusCRC16(crc16, p->data[i], false);
	}

	(*fcs) = crc16;
	return LA_NO_ERROR;
}

LAErrorCode LAReadACKHandler(LASerialV2RecvProtocol *p){
	LA_CHECK_NULLPTR(p);
	
	printf("RX Command ACK\n");

	LAErrorCode code = LA_NO_ERROR;

	if(p->length < 2){
		printf("Error: ACK received but response is too short.");
		return LA_ERROR_RESPONSE_TOO_SHORT;
	}

	uint16_t transactionId = 0;
	code = LAProtocolToHostU16(&transactionId, (uint8_t *)p->data, 0, LA_MIN(p->length, LA_SERIAL_V2_DATA_LENGTH));
	if(code) goto cleanup;

	bool found = false;
	double rtt = 0;
	code = LAResolveACK(&(lawp->ack), transactionId, &found, &rtt);
	if(code) goto cleanup;

	printf("Resolve ACK by Id:\n\tTransactionId: %i\tFound: %i\trtt: %lf ms\n", transactionId, found, rtt);

	size_t matches = 0;
	code = LAResolveTimeoutACK(&(lawp->ack), &matches);
	if(code) goto cleanup;

	printf("Resolve ACK by Timeout:\n\tMatches: %li\n", matches);
	
	goto cleanup;
	
cleanup:
	// Handle ACK (old)
	atomic_store(&(lawp->mutexes.isWaitingACK), 0);
	pthread_cond_signal(&(lawp->mutexes.condACK));

	return code;
}

LAErrorCode LAReadCaptureHandler(LASerialV2RecvProtocol *p){
	LA_CHECK_NULLPTR(p);
	
	printf("RX Command CAPTURE\n");
	
	//Actually not used, but the buffer write function needs a uint16_t *
	static uint16_t index = 0;

	// don't write means discard all incoming capture data
  if(atomic_load(&(lawp->rd.dontWrite))){
  	return LA_NO_ERROR;
  }
	
  // Writes to the oscilloscope buffer
	LAWriteDataToDataBuffer(lawp, p->data, p->length, &index);

	// Update offsets and counters
	atomic_fetch_add(&(lawp->rd.dataOffset), p->length);
  atomic_fetch_add(&(lawp->rd.sampleCounter), p->length);
  
  // Mark oscilloscope window as dirty
	atomic_store(&(lawp->connect.dataHasChanged), 1);
  
  // Writes to bucket if there is a buffer
  if(atomic_load(&(lawp->bd.bucketWrite))){
  	LAHandleBucketWrite(p->data, p->length);
  }
  
	return LA_NO_ERROR;
}

LAErrorCode LAProtocolV2RDispatch(LASerialV2RecvProtocol *p){
	LA_CHECK_NULLPTR(p);

	LAErrorCode code = LA_NO_ERROR;
	
	uint16_t fcs = 0x0;
	code = LACalculateFCS_V2R(p, &fcs);
	printf("FCS | calculated: %04X\treceived: %04X\n", fcs, p->fcs);
	// FCS mismatch
	if(fcs != p->fcs){
		printf("Error, FCS does not match\n");
		return LA_ERROR_MISMATCH_FCS;
	}
	
	switch(p->command){
		case LA_RX_COMMAND_NOP:     
																printf("RX Command NOP\n");
			                  				break;
		case LA_RX_COMMAND_ACK:     
																code = LAReadACKHandler(p);
			                  				break;
		case LA_RX_COMMAND_CAPTURE: 
																code = LAReadCaptureHandler(p);
			                  				break;
		default:
		                    				break;
	}

	return code;
}

void *LAReadThread(void *vlaw){
	LAWindow *law = vlaw;
//	uint16_t index = 0;
//	uint16_t dataOffset = 0;
//	LASerialV2RecvProtocol pr;
	//LAProtocolV2RInit(&pr);

	/*
	tcflush(law->connect.fd, TCIFLUSH);
	law->mutexes.isWaitingACK = 0;
*/

	uint8_t buffer[LA_READ_BUFFER_LENGTH];
	ssize_t numBytes = 0;
	
	LAErrorCode code = LA_NO_ERROR;
	
	LAPacketParser parser;
	code = LAPacketParserInit(&parser);
	if(code) return NULL;
	struct timespec lastTime;
	clock_gettime(CLOCK_MONOTONIC, &lastTime);
	double timeDelta = 0.0;
	
	do{
		numBytes = read(law->connect.fd, &buffer, LA_READ_BUFFER_LENGTH);

		// Handle EOF (0) or error (-1)
		if(numBytes <= 0)        continue;
		
		// Discard previous data if timed out
		if(LAHasTimeout(&lastTime, &timeDelta)){
			printf("Last read has timeout, restarting protocol\n");
			code = LAPacketParserClear(&parser);
		}

		clock_gettime(CLOCK_MONOTONIC, &lastTime);
		printf("Time since last read: %lf\n", timeDelta);
		printf("Bytes read: %li\n", numBytes);
		printf("Payload:\n");
		code = LADebugPrintHex(buffer, numBytes);		

		bool isDone = false;
		size_t bytesRead = 0;
		while(bytesRead < (size_t)numBytes){
			code = LAPacketParserParse(&parser, buffer, numBytes, &bytesRead, &isDone);
			
			// Error (Packet too big, nullptr, other)
			if(code) {
				printf("Parser error, (code %i)\n", code);
				code = LAPacketParserClear(&parser);
				isDone = false;
				break;				
			}

		  // Packet is okay
			if(isDone){
				printf("Packet received:\n");
				code = LAProtocolV2RPrint(&(parser.data));
				code = LAProtocolV2RDispatch(&(parser.data));
				if(code){
					printf("Dispatch error: code %i\n", code);
				}
				code = LAPacketParserClear(&parser);
				isDone = false;
			}
		}	
		
	} while(1);

	(void) code;
	return NULL;
}

int LARedrawConnector(void *vlaw){
	LAWindow *law = vlaw;
	static int a = 0;

	g_print("aaaaa %i\n", a);
	a++;
	LARedrawAllScopes(law);
	return TRUE;
}

/*
void LAWindowUpdateLoop(void *vlaw){
	LAWindow *law = vlaw;

	struct timespec ts;
	long sleepTime = 66666666;
	int source = 0;

	ts.tv_sec = 0;
	ts.tv_nsec = sleepTime;

	while(1){
		if(atomic_load(&(law->breakWindowUpdate))){
			break;
		}

		nanosleep(&ts, NULL);
		if(atomic_load(&(law->connect.dataHasChanged)) == 0){
			continue;
		}

		source = g_idle_add(LARedrawConnector, law);
		atomic_store(&(law->connect.dataHasChanged), 0);
		//g_source_remove(source);
	}
}
*/

void LAWindowUpdateLoop(LAWindow *law){
	LAUpdateStatusBar(law);

	if(atomic_load(&(law->connect.dataHasChanged)) == 0){
		return;
	}

	LARedrawAllScopes(law);
	atomic_store(&(law->connect.dataHasChanged), 0);
}

int LAWindowUpdateLoopConnector(void *vlaw){
	LAWindow *law = vlaw;
	LAWindowUpdateLoop(law);
	return TRUE;
}
