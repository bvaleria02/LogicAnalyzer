#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "../compiler.h"

LAErrorCode LACreateDefineNode(LANodeDefine **node, char *label, LANodeSplit *startSplit){
	LA_HANDLE_NULLPTR(node,					LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(label,				LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(startSplit,			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(startSplit->value,	LA_PROPAGATE_ERROR);

	uint8_t isNodeAllocated 	= 0;
	uint8_t isLabelAllocated 	= 0;
	uint8_t isValueAllocated 	= 0;

	LANodeDefine *newNode = (LANodeDefine *)malloc(sizeof(LANodeDefine));
	if(newNode == NULL)				goto error;
	else							isNodeAllocated = 1;

	size_t labelLength = strlen(label);
	newNode->label = malloc(labelLength + 1);
	if(newNode->label == NULL)		goto error;
	else							isLabelAllocated = 1;

	strncpy(newNode->label, label, labelLength);
	newNode->labelLength = labelLength;

	LANodeSplit *nsplit = startSplit;
	size_t valueSize = 0;
	while(nsplit != NULL){
		valueSize += nsplit->length + 1;
		nsplit = nsplit->next;
	}

	newNode->value = malloc(valueSize + 1);
	if(newNode->value == NULL)		goto error;
	else							isValueAllocated = 1;

	nsplit = startSplit;
	size_t offset = 0;
	while(nsplit != NULL){
		memcpy(&(newNode->value[offset]), nsplit->value, nsplit->length);
		offset += nsplit->length;
		nsplit = nsplit->next;
		if(nsplit != NULL){
			newNode->value[offset] = ' ';
		} else {
			newNode->value[offset] = '\0';
		}
		offset++;
	}

	newNode->label[labelLength] = '\0';
	newNode->value[valueSize] = '\0';
	newNode->valueLength = valueSize;
	newNode->next = NULL;

	if((*node) == NULL){
		(*node) = newNode;
		return LA_NO_ERROR;
	}

	LANodeDefine *ndef = (*node);
	while(ndef->next != NULL){
		ndef = ndef->next;
	}

	ndef->next = newNode;

	return LA_NO_ERROR;

error:
	if(isValueAllocated){
		free(newNode->value);
		newNode->value = NULL;
	}

	if(isLabelAllocated){
		free(newNode->label);
		newNode->label = NULL;
	}

	if(isNodeAllocated){
		free(newNode);
		newNode = NULL;
	}

	LA_RAISE_ERROR(LA_ERROR_MALLOC);
	return LA_ERROR_MALLOC;
}

LAErrorCode LACreateNewNodeDefine(LANodeSplit *splitStart, LANodeDefine **defineStart){
	LA_HANDLE_NULLPTR(splitStart,			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(defineStart,			LA_PROPAGATE_ERROR);
	
	LANodeSplit *define = NULL;
	LANodeSplit *label 	= NULL;
	LANodeSplit *value 	= NULL;

	define = splitStart;
	if(define->next == NULL){
		LA_RAISE_ERROR(LA_ERROR_INVALIDSYNTAX);
		return LA_ERROR_INVALIDSYNTAX;
	}

	label = define->next;
	if(label->next == NULL){
		LA_RAISE_ERROR(LA_ERROR_INVALIDSYNTAX);
		return LA_ERROR_INVALIDSYNTAX;
	}

	value = label->next;
	printf("define: %s\n", define->value);
	printf("label: %s\n", label->value);
	printf("value: %s\n", value->value);

	LA_HANDLE_NULLPTR(define->value,	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(label->value,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value->value,		LA_PROPAGATE_ERROR);
	
	LAErrorCode code = LA_NO_ERROR;
	if(!strncmp(define->value, LA_COMPILER_DEFINE_TEXT, strlen(LA_COMPILER_DEFINE_TEXT))){
		code = LACreateDefineNode(defineStart, label->value, value);
		if(code) return code;
	}
	
	return LA_NO_ERROR;
}
