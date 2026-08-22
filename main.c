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

LAWindow *lawp;

void testCompiler(){
	char *filename = "testFile.lasl";
	LAMappedFile file;

	LAErrorCode code = LAMemoryMapFile(filename, &file);
	write(1, file.data, file.length);

	size_t offset = 0;
	LANodeSplit *splitStart = NULL;
	LANodeSplit *node = NULL;

	LANodeDefine *nodeDefine = NULL;
	LANodeDefine *node2 = NULL;

	do{
		code = LASplitLineIntoNodes(&file, &offset, &splitStart);
		if(code != LA_NO_ERROR){
			printf("Error: %i\n", code);
			break;
		}

		node = splitStart;
		while(node != NULL){
			printf("\tNode: %s\n", node->value);
			node = node->next;
		}
		printf("\n");

		LACreateNewNodeDefine(splitStart, &nodeDefine);
	
		code = LASplitDestroyNodes(&splitStart);
		if(code != LA_NO_ERROR){
			printf("Error: %i\n", code);
		}
		splitStart = NULL;

	} while(1);

		node2 = nodeDefine;
		while(node2 != NULL){
			printf("\tNode: %s \t %s\n", node2->label, node2->value);
			node2 = node2->next;
		}

	code = LAMemoryUnmapFile(&file);
}

double f_rsqrt(double y, void *ptr){
	double x = *(double *)ptr;
	return 1/(y*y) - x;
}
/*
double df_rsqrt(double y, void *ptr){
	(void) ptr;
	return -2/(y*y*y);
}
*/
int main(int argc, char **argv){
	/*
	double x = 16;
	LASecantData nrdata;
	nrdata.nmax  = 50;
	nrdata.atol  = 1e-8;
	nrdata.rtol  = 1e-5;
	nrdata.ftol	 = 1e-6;
	nrdata.eps   = 1e-15;
	nrdata.reps  = 1e-15;
	nrdata.flags = LA_SECANT_AVOID_SINGULARITY | LA_SECANT_USE_ATOL | LA_SECANT_USE_RTOL | LA_SECANT_USE_FTOL;
	nrdata.status = 0;

	double y = LASecantCompact(f_rsqrt, 0.05, 0.24, &x, &nrdata);
	printf("x: %lf\ty: %lf\n", x, y);
	printf("status: %i\n", nrdata.status);
*/
/*
	testCompiler();
	return 0;
*/

	double a[3*2] = {
		1.0, 	0.0,
		0.0,	-2.0,
		0.2, -12.00
	};

	LAMat_t mat, m2, m3;
	LAMatCreateFromArray(&mat, 3, 2, a);
	LAMatPrint(&mat);
	double b[2*4] = {
		4.0, 1.0, -1.0, -4.0,
		0.0, -10.0, 6.0, -5.0
	};
	LAMatCreateFromArray(&m2, 2, 4, b);
	LAMatPrint(&m2);
	double c[3*4] = {
		-100, -100, -100, -100,
		-100, -100, -100, -100,
		-100, -100, -100, -100
	};
	LAMatCreateFromArray(&m3, 3, 4, c);
	LAMatPrint(&m3);
	LAMatMulCum(&mat, &m2, &m3);
	LAMatPrint(&m3);

	double d4[5*5];
	LAMat_t m4, ma4;
	LAMatCreateFromArray(&m4, 5, 5, d4);
	LAMatFill(&m4, 7.2);
	LAMatPrint(&m4);
	LAMatZeros(&m4);
	LAMatPrint(&m4);
	LAMatOnes(&m4);
	LAMatPrint(&m4);
	LAMatRandi(&m4, -12, 12);
	LAMatPrint(&m4);
	LAMatRand(&m4);
	LAMatPrint(&m4);
	LAMatEye(&m4);
	LAMatPrint(&m4);

	LAMatRowSwap(&m4, 0, 2);
	LAMatPrint(&m4);
	LAMatAddRow(&m4, 1, 2, 10);
	LAMatPrint(&m4);

	double a_4[5*5] = {
		0.0520,	-0.8279,	-0.6156,	0.3265,	0.7805,
	   -0.3022,	-0.8717,	-0.9600,	-0.0846, -0.8738,
	   -0.5234,	0.9413,	0.8044,	0.7018,	-0.4667,
	    0.0795,	-0.2496,	0.5205,	0.0251,	0.3354,
	    0.0632,	-0.9214,	-0.1247,	0.8637,	0.8616,
	};
	double p4[5*5];
	double l4[5*5];
	double u4[5*5];
	double z4[5*5];
	double m_4[5*5];
	double y_4[5*5];
	LAMat_t mp4, ml4, mu4, mz4, mm4, my4;
	LAMatCreateFromArray(&ma4, 5, 5, a_4);
	LAMatCreateFromArray(&mp4, 5, 5, p4);
	LAMatCreateFromArray(&ml4, 5, 5, l4);
	LAMatCreateFromArray(&mu4, 5, 5, u4);
	LAMatCreateFromArray(&mz4, 5, 5, z4);
	LAMatCreateFromArray(&mm4, 5, 5, m_4);
	LAMatCreateFromArray(&my4, 5, 5, y_4);
/*
	LAMatLU(&ma4, &ml4, &mu4, NULL);
	LAMatPrintWithLabel(&ma4, "A");
	LAMatPrintWithLabel(&ml4, "L");
	LAMatPrintWithLabel(&mu4, "U");
	LAMatPrintWithLabel(&mp4, "P");
*/
/*
	LAMatCopy(&ma4, &ml4);
	LAMatInverseGaussJordan(&ml4, &mu4);
	LAMatPrintWithLabel(&ma4, "A");
	LAMatPrintWithLabel(&ml4, "I");
	LAMatPrintWithLabel(&mu4, "A-1");

	LAMatMul(&ma4, &mu4, &mm4);
	LAMatPrintWithLabel(&mm4, "A'");
*/
	LAMatInverseLU(&ma4, &ml4, &mu4, &mp4, &mz4, &mm4);
	LAMatPrintWithLabel(&ma4, "A");
	LAMatPrintWithLabel(&ml4, "L");
	LAMatPrintWithLabel(&mu4, "U");
	LAMatPrintWithLabel(&mp4, "P");
	LAMatPrintWithLabel(&mz4, "Z");
	LAMatPrintWithLabel(&mm4, "M");

	LAMatMul(&ma4, &mm4, &my4);
	LAMatPrintWithLabel(&my4, "I");

	LAMat_t mv;
	for(size_t x = 0; x < 5; x++){
		LAMatCreateViewFlags(&mv, &my4, 0, x, 4, x, LA_MAT_READ | LA_MAT_WRITE);
		LAMatTranspose(&mv);
		LAMatPrintWithLabel(&mv, "V");
	}

	LAMatCreateViewFlags(&mv, &mu4, 1, 2, 4, 3, LA_MAT_READ | LA_MAT_WRITE);
	LAMatTranspose(&mv);
	LAMatPrintWithLabel(&mv, "V");


	LAListStore list;
	LAListStoreInit(&list, 0, true, 0, false);

	for(int i = 0; i < 12; i++){
		printf("i: %i\t code: %i\n", i, LAListStoreInsert(&list, &i, sizeof(int)));
	}

	LAErrorCode listStoreCallback(LAListStore *list, LAListStoreNode *node, size_t index, void *data, bool *stopIter){
		if(node->data == NULL) return LA_ERROR_NULLPTR;

		printf("i: %li\tx: %i\n", index, *(int *)(node->data));

		(void) list;
		(void) node;
		(void) index;
		(void) data;
		(void) stopIter;
		return LA_NO_ERROR;
	}

	printf("code: %i\n", LAListStoreIter(&list, listStoreCallback, NULL));

	LAListStore *listp = &list;
	LAListStoreDestroy(&listp);
	

	gtk_init(&argc, &argv);

	LAWindow law;
	lawp = &law;

	LAListStoreInit(&(law.ack.ackList), 0, true, 0, false);
	law.ack.ackMutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	law.ack.transactionIdMutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	LARegisterACK(&(law.ack), NULL);
	LARegisterACK(&(law.ack), NULL);
	LARegisterUsingIdACK(&(law.ack), 0x42);
	LARegisterUsingIdACK(&(law.ack), 0x13);
	LARegisterUsingIdACK(&(law.ack), 0x67);
	LARegisterUsingIdACK(&(law.ack), 0x69);
	LARegisterACK(&(law.ack), NULL);
	LARegisterUsingIdACK(&(law.ack), 0x420);
	LARegisterUsingIdACK(&(law.ack), 0x666);
	LARegisterUsingIdACK(&(law.ack), 0x777);
	LARegisterUsingIdACK(&(law.ack), 0x1337);
	LARegisterUsingIdACK(&(law.ack), 0x6942);
	LARegisterUsingIdACK(&(law.ack), 0x6666);
	LARegisterUsingIdACK(&(law.ack), 0x6969);
	LARegisterUsingIdACK(&(law.ack), 0x8085);
	LARegisterACK(&(law.ack), NULL);
	LARegisterACK(&(law.ack), NULL);
	LARegisterACK(&(law.ack), NULL);
	LARegisterACK(&(law.ack), NULL);
	LAPrintACKList(&(law.ack));

	bool found = false;
	size_t ackIndex = 0;
	LAFindACK(&(law.ack), 0x420, &found, &ackIndex);
	printf("Found: %i\nIndex: %li\n", found, ackIndex);

	double rtt = 0.0;
	LAResolveACK(&(law.ack), 0x420, &found, &rtt);
	printf("Found: %i\tRTT: %lf ms\n", found, rtt);
	
	LAPrintACKList(&(law.ack));

	LADequeStore deque = {0};
	LADequeStoreInit(&deque, 0, true, 0, false);

	size_t testInt = 0x69;	
	LADequeStorePush(&deque, (void *)(&testInt), sizeof(size_t));
	testInt = 0x42;	
	LADequeStorePush(&deque, (void *)(&testInt), sizeof(size_t));
	testInt = 0x666;	
	LADequeStorePushLeft(&deque, (void *)(&testInt), sizeof(size_t));
	testInt = 0x69420;	
	LADequeStorePushLeft(&deque, (void *)(&testInt), sizeof(size_t));
	testInt = 0x1337;	
	LADequeStorePush(&deque, (void *)(&testInt), sizeof(size_t));

	LAErrorCode dequeStoreCallback(LAListStore *list, LAListStoreNode *node, size_t index, void *data, bool *stopIter){
		if(node->data == NULL) return LA_ERROR_NULLPTR;

		printf("Node: %li\tx: 0x%016lX\n", index, *(size_t *)(node->data));

		(void) list;
		(void) node;
		(void) index;
		(void) data;
		(void) stopIter;
		return LA_NO_ERROR;
	}
	
	LADequeStoreIter(&deque, dequeStoreCallback, NULL);

	size_t matches = 0;

	printf("Sleep for 2 seconds (2 total)\n");
	sleep(2);
	LAResolveTimeoutACK(&(law.ack), &matches);
	LAPrintACKList(&(law.ack));
	printf("%li matches last LAResolveTimeoutACK\n", matches);
	
	printf("Sleep for 4 seconds (6 total)\n");
	sleep(4);
	LAResolveTimeoutACK(&(law.ack), &matches);
	LAPrintACKList(&(law.ack));
	printf("%li matches last LAResolveTimeoutACK\n", matches);

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
	return 0;
}
