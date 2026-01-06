#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "logicanalyzer/liblogicanalyzer.h"
#include "logicanalyzer/compiler.h"
#include <math.h>
#include <unistd.h>
#include <fcntl.h>

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


int main(int argc, char **argv){
/*
	testCompiler();
	return 0;
*/
	gtk_init(&argc, &argv);

	LAWindow law;
	lawp = &law;

	LAWindowCreate(&law);
	LAWindowCreateMenu(&law);
//	LAWindowCreateConnect(&law);
	LAWindowCreateChannels(&law);
	LAPlaceStatusBar(&law);

	for(uint16_t i = 0; i < LA_LARGE_BUFFER_SIZE; i++){
		law.dataBuffer[i] = ((int) i & 0xFF);
	}

	LAWindowRun(&law);
	return 0;
}
