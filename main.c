#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "logicanalyzer/liblogicanalyzer.h"
#include "logicanalyzer/error.h"
#include "logicanalyzer/types.h"
#include "logicanalyzer/gtk_funcs.h"
#include "logicanalyzer/compiler.h"
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include "logicanalyzer/numericMethods/secant.h"
#include "logicanalyzer/matrix/matrix.h"
#include "logicanalyzer/la_bigint/laBigInt.h"
#include "logicanalyzer/structures/listStore.h"
#include "logicanalyzer/structures/dequeStore.h"
#include "logicanalyzer/threads/ack.h"
#include <stdbool.h>
#include <pthread.h>
#include "logicanalyzer/utils.h"

LAWindow *lawp;



void *LATXThreadRoutine(void *ptr){
	LAWindow *law = (LAWindow *)ptr;

	printf("TX: Create\n");
	pthread_mutex_lock(&(law->tx.thread.mutex));
	
	// Main loop
	while(true){
		printf("TX: Sleep\n");
		// "Sleep" while dirty flags is not set
		// Using while to protect agains spurious awake
		// NOTE: ALWAYS lock mutex first, and then change "dirty" outsude.
		while((!law->tx.thread.dirty) && (!law->tx.thread.close)){
		printf("TX: Sleeping\tdirty: %i\tclose: %i\n", law->tx.thread.dirty, law->tx.thread.close);
			pthread_cond_wait(&(law->tx.thread.cond), &(law->tx.thread.mutex));
		}

		printf("TX: Awake\n");

		if(law->tx.thread.close) break;

		void *data = NULL;
		size_t length = 0;
		// Handle list
		printf("TX: Iter\n");
		do{
			data = NULL;
			
			printf("TX: Pop\n");
			LA_MUTEX(&(law->tx.dequeMutex), {
				LADequeStorePopLeft(&(law->tx.deque), &data, &length);			         
			});

			// End if deque is empty (data == NULL)
			if(data == NULL) break;

			size_t totalBytesSent = 0;
			while(totalBytesSent < length){
  			printf("TX: Send\n");
				int writeResponse = write(law->connect.fd, data, length);
				printf("Sent data:\tresponse: %i\tbytes sent:%li\n", writeResponse, length);
				if(writeResponse < 0){
					// TODO: Handle error, lol
				}
				totalBytesSent += writeResponse;
			}

			if(data != NULL) free(data);
		} while(true);

		law->tx.thread.dirty = false;
	}
	
	pthread_mutex_unlock(&(law->tx.thread.mutex));

	return NULL;
}

LAErrorCode LACreateTXThread(LAWindow *law){
	int response = 0;

	law->tx.thread.dirty = false;
	law->tx.thread.close = false;
	
	LADequeStoreInit(&(law->tx.deque), 0, true, 0, false);

	response = pthread_mutex_init(&(law->tx.thread.mutex), NULL);
	if(response != 0) goto error;
	
	response = pthread_cond_init(&(law->tx.thread.cond), NULL);
	if(response != 0) goto error;
		
	response = pthread_create(&(law->tx.thread.thread), NULL, LATXThreadRoutine, (void *)law);
	if(response != 0) goto error;

	return LA_NO_ERROR;

error:
	printf("Error creating TX Thread\nResponse: %i\n", response);
	return LA_NO_ERROR; // TODO: generic error
}

int main(int argc, char **argv){
	LAWindow law = {0};
	lawp = &law;

	gtk_init(&argc, &argv);

	LAListStoreInit(&(law.ack.ackList), 0, true, 0, false);

	LACreateTXThread(&law);

	LAWindowCreate(&law);
	LAWindowCreateMenu(&law);
//	LAWindowCreateConnect(&law);
	LAWindowCreateChannels(&law);
	LAPlaceStatusBar(&law);

	for(uint16_t i = 0; i < LA_LARGE_BUFFER_SIZE; i++){
		//law.dataBuffer[i] = ((int) sqrt(i) & 0xFF);
		//law.dataBuffer[i] = i & 0xFF;
		law.dataBuffer[i] = ((i >> 8) & 0x1) ? 0xFF : 0x0;
		//law.dataBuffer[i] = 0x80 + 0x7F * sin(2 * M_PI * (i / (double) 2048));
	}

	LAWindowRun(&law);
	
	LAListStore *acklistp = &(law.ack.ackList);
	LAListStoreDestroy(&(acklistp));

	law.tx.thread.close = true;
	pthread_cond_broadcast(&(law.tx.thread.cond));

	(void) argc;
	(void) argv;
	return 0;
}
