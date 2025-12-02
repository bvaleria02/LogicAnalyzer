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
	uint8_t buffer[LA_BUFFER_SIZE];
	uint16_t index = 0;
	int16_t numBytes = 0;
	uint16_t dataOffset = 0;

	do{
		if(atomic_load(&(law->connect.breakReadLoop))){
			break;
		}

		numBytes = read(law->connect.fd, &buffer, LA_BUFFER_SIZE);
		if(numBytes <= 0){
			continue;
		}

	//	tcflush(law->connect.fd, TCIFLUSH);
	//	g_print("Length: %i bytes\n", numBytes);

		if(numBytes >= LA_BUFFER_SIZE){
			numBytes = LA_BUFFER_SIZE;
		}

		if(atomic_load(&(lawp->rd.dontWrite))){
			continue;
		}

		LAWriteDataToDataBuffer(law, buffer, numBytes, &index);

		if(atomic_load(&(law->rd.addDataOffset))){
			dataOffset = index;
			atomic_store(&(law->rd.dataOffset), dataOffset);
		}

		atomic_store(&(law->connect.dataHasChanged), 1);
		atomic_fetch_add(&(law->rd.sampleCounter), numBytes);

		if(atomic_load(&(law->bd.bucketWrite))){
			LAHandleBucketWrite(buffer, numBytes);
		}

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
