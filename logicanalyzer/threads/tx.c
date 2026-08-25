#include <gtk/gtk.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>

#include "../error.h"
#include "../utils.h"
#include "../types.h"
#include "../liblogicanalyzer.h"

bool LATXThreadSleep(LAWindow *law){

	printf("TX: Sleep\n");
	// "Sleep" while dirty flags is not set
	// Using while to protect agains spurious awake
	// NOTE: ALWAYS lock mutex first, and then change "dirty" outsude.
	while((!law->tx.thread.dirty) && (!law->tx.thread.close)){
	printf("TX: Sleeping\tdirty: %i\tclose: %i\n", law->tx.thread.dirty, law->tx.thread.close);
		pthread_cond_wait(&(law->tx.thread.cond), &(law->tx.thread.mutex));

		bool isEmpty = false;
		LADequeStoreIsEmpty(&(law->tx.deque), &isEmpty);
		if(!isEmpty) break;
	}

	printf("TX: Awake\n");

	return (law->tx.thread.close);
}

void LATXThreadSendData(LAWindow *law, const void *data, const size_t length){
	size_t totalBytesSent = 0;
		
	while(totalBytesSent < length){
  		printf("TX: Send\n");

		int writeResponse = write(law->connect.fd, (uint8_t *)data + totalBytesSent, length);
		printf("Sent data:\tresponse: %i\tbytes sent:%li\n", writeResponse, length);

		if(writeResponse < 0){
			// TODO: Handle error, lol
		} else if (writeResponse == 0){
			// TODO: Handle error, lol
		}

		totalBytesSent += writeResponse;
	}
}

void LATXThreadIter(LAWindow *law){
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

		LATXThreadSendData(law, data, length);

		// data is non-NULL
		free(data);

	} while(true);

	return;
}

bool LATXThreadLoop(LAWindow *law){
	bool endRoutine = false;

	endRoutine = LATXThreadSleep(law);
	if(endRoutine) return endRoutine;

	LATXThreadIter(law);

	// TODO: mutex
	//LA_MUTEX(&(law->tx.thread.mutex), {
		law->tx.thread.dirty = false;
	//});

	return endRoutine;
}


void *LATXThreadRoutine(void *ptr){
	LAWindow *law = (LAWindow *)ptr;

	printf("TX: Create\n");
	pthread_mutex_lock(&(law->tx.thread.mutex));
	
	// Main loop
	bool endRoutine = false;
	while(!endRoutine){
		endRoutine = LATXThreadLoop(law);
	}
	
	pthread_mutex_unlock(&(law->tx.thread.mutex));
	printf("TX: Exit\n");

	return NULL;
}
