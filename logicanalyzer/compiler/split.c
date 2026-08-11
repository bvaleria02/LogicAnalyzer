#include <stdio.h>
#include <stdlib.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "../compiler.h"
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

LAErrorCode LASplitCreateNode(LANodeSplit **node, size_t length){
	LA_HANDLE_NULLPTR(node,			LA_PROPAGATE_ERROR);

	uint8_t isNodeAllocated 		= 0;
	uint8_t isNodeDataAllocated 	= 0;

	(*node) = (LANodeSplit *)malloc(sizeof(LANodeSplit));
	if((*node) == NULL){
		goto malloc_error;
	}

	isNodeAllocated = 1;
	(*node)->length = length;
	(*node)->next 	= NULL;
	(*node)->value	= (char *)malloc(length + 1);
	if((*node) == NULL){
		goto malloc_error;
	}

	isNodeDataAllocated = 1;
	(*node)->length = length;
	memset((*node)->value, 0, length + 1);
	return LA_NO_ERROR;

malloc_error:
	if(isNodeDataAllocated){
		free((*node)->value);
	}

	if(isNodeAllocated){
		free((*node));
	}

	LA_RAISE_ERROR(LA_ERROR_MALLOC);
	return LA_ERROR_MALLOC;
}

LAErrorCode LASplitDestroyNodes(LANodeSplit **startNode){
	LA_HANDLE_NULLPTR(startNode,		LA_PROPAGATE_ERROR);

	LANodeSplit *node = (*startNode);
	LANodeSplit *next = NULL;

	while(node != NULL){
		next = node->next;
		if(node->value != NULL) free(node->value);
		free(node);
		node = next;
	}

	return LA_NO_ERROR;
}

LAErrorCode LASplitCopyFromFile(LANodeSplit *node, LAMappedFile *file, size_t offset){
	LA_HANDLE_NULLPTR(node,				LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(node->value,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(file,				LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(file->data,		LA_PROPAGATE_ERROR);
	
	if(offset > file->length){
		LA_RAISE_ERROR(LA_ERROR_OUTOFRANGE);
		return LA_ERROR_OUTOFRANGE;
	}

	size_t length = node->length;
	if((offset + length) > file->length){
		length = file->length - offset + 1;
	}

	if(length > node->length){
		length = node->length;
	}

	memcpy(node->value, &(file->data[offset]), length);
	return LA_NO_ERROR;
}

LAErrorCode LASplitLineIntoNodes(LAMappedFile *file, size_t *offset, LANodeSplit **startNode){
	LA_HANDLE_NULLPTR(file,				LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(file->data,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(offset,			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(startNode,		LA_PROPAGATE_ERROR);

	if((*offset) > file->length){
		LA_RAISE_ERROR(LA_ERROR_OUTOFRANGE);
		return LA_ERROR_OUTOFRANGE;
	}

	size_t offsetStart = (*offset);
	size_t offsetEnd   = (*offset);
	for(size_t i = offsetStart; i < file->length; i++){
		if(file->data[i] == '\n') break;
		offsetEnd = i;
	}

	if(offsetStart == offsetEnd){
		(*offset) = (*offset) + 1;
		return LA_NO_ERROR;
	}

	size_t splitStart 	= LA_SPLIT_UNSET;
	size_t splitEnd 	= LA_SPLIT_UNSET;
	LASplitStatus status = LA_SPLIT_STATUS_BLANK;
	char c = 0;

	LAErrorCode code = LA_NO_ERROR;
	LANodeSplit *node = (*startNode);
	LANodeSplit *next = NULL;
	if(node == NULL){
		next = NULL;
	} else {
		next = node->next;
	}
	size_t nodeLength = 0;

	for(size_t i = offsetStart; i <= offsetEnd; i++){
		c = file->data[i];
		
		if(status == LA_SPLIT_STATUS_BLANK && (c == ' ' || c == '\t' || c == ',')){
			continue;
		}

		if(status == LA_SPLIT_STATUS_BLANK && (c != ' ' && c != '\t' && c != ',')){
			status = LA_SPLIT_STATUS_MARKING;
			splitStart = i;
		}

		if((status == LA_SPLIT_STATUS_MARKING && (c == ' ' || c == '\t')) || (i == offsetEnd) || (c == '\n')){
			if(i == offsetEnd){
				splitEnd = i;
			} else {
				splitEnd = i - 1;
			}

			if(splitStart 	== LA_SPLIT_UNSET) continue;
			if(splitEnd 	== LA_SPLIT_UNSET) continue;

			nodeLength = splitEnd - splitStart + 1;
			code = LASplitCreateNode(&next, nodeLength);
			if(code) return code;

			code = LASplitCopyFromFile(next, file, splitStart);
			if(code) return code;

			if(node == NULL){
				(*startNode) = next;
				node = next;
				next = NULL;
			} else {
				node->next = next;
				node = node->next;
				next = NULL;
			}

			splitStart 	= LA_SPLIT_UNSET;
			splitEnd 	= LA_SPLIT_UNSET;
			status = LA_SPLIT_STATUS_BLANK;
		}

	}

	(*offset) = offsetEnd + 1;
	return LA_NO_ERROR;
}
