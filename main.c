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
#include "logicanalyzer/threads/tx.h"
#include <stdbool.h>
#include <pthread.h>
#include "logicanalyzer/utils.h"

LAWindow *lawp;

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
	pthread_join(law.tx.thread.thread, NULL);
	LADequeStore *txDequep = &(law.tx.deque);
	LADequeStoreDestroy(&txDequep);
	
	(void) argc;
	(void) argv;
	return 0;
}
