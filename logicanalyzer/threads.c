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


void LAHandleBucketWrite(uint8_t *buffer, int16_t size){
	pthread_mutex_lock(&(lawp->mutexes.bucketAccess));
	
	for(int16_t i = 0; i < size; i++){
		LABucketInsertData(&(lawp->bd.bucketCurrent), buffer[i] & lawp->bd.dataMask);	
	}

	pthread_mutex_unlock(&(lawp->mutexes.bucketAccess));
}

static void LAWriteDataToDataBuffer(LAWindow *law, uint8_t *buffer, int16_t numBytes, uint16_t *index){
	pthread_mutex_lock(&(law->mutexes.dataBufferAccess));

	for(uint16_t i = 0; i < numBytes; i++){
		law->dataBuffer[((*index) + i) % LA_LARGE_BUFFER_SIZE] = buffer[i];
	}

	(*index) = ((*index) + numBytes) % LA_LARGE_BUFFER_SIZE;
	pthread_mutex_unlock(&(law->mutexes.dataBufferAccess));
}

void *LAReadThread(void *vlaw){
	LAWindow *law = vlaw;
	uint8_t buffer[LA_READ_BUFFER_LENGTH];
	uint16_t index = 0;
	int16_t numBytes = 0;
	uint16_t dataOffset = 0;
	LASerialV2RecvProtocol pr;
	LAProtocolV2RInit(&pr);

	tcflush(law->connect.fd, TCIFLUSH);
	law->mutexes.isWaitingACK = 0;

	do{
		if(atomic_load(&(law->connect.breakReadLoop))){
			break;
		}

		numBytes = read(law->connect.fd, &buffer, LA_READ_BUFFER_LENGTH);
		if(numBytes <= 0){
			continue;
		}
/*
		for(uint16_t i = 0; i < numBytes; i++){
			printf(" %02X", buffer[i]);
		}
*/
	//	g_print("Length: %i bytes\n", numBytes);

		LAProtocolV2RFill(&pr, buffer, numBytes);
//		g_print("Offset: %i\tsize: %i\n", pr.offset, numBytes);

/*
		if(numBytes >= LA_READ_BUFFER_LENGTH){
			numBytes = LA_READ_BUFFER_LENGTH;
		}
*/

		if(LAProtocolV2RIsReady(&pr) == 0){
//			g_print("Protocol is not ready\n");
		} else {
			LAProtocolV2RUnpack(&pr);
			/*
			g_print("Command: %i\n", pr.command);
			g_print("Length: %i\n", pr.length);
*/
			switch(pr.command){
				case LA_RX_COMMAND_ACK:	
										atomic_store(&(law->mutexes.isWaitingACK), 0);
										pthread_cond_signal(&(law->mutexes.condACK));
										break;

				case LA_RX_COMMAND_CAPTURE:
											if(atomic_load(&(lawp->rd.dontWrite))){
												continue;
											}
											
											LAWriteDataToDataBuffer(law, pr.data, pr.length, &index);

											if(atomic_load(&(law->rd.addDataOffset))){
												dataOffset = index;
												atomic_store(&(law->rd.dataOffset), dataOffset);
											}

											atomic_store(&(law->connect.dataHasChanged), 1);
											atomic_fetch_add(&(law->rd.sampleCounter), numBytes);

											if(atomic_load(&(law->bd.bucketWrite))){
												LAHandleBucketWrite(buffer, numBytes);
											}
											
											break;

			}

			LAProtocolV2RInit(&pr);
		}

/*



*/
	} while(1);

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
