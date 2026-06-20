#ifndef LIB_LA_COMPILER_H
#define LIB_LA_COMPILER_H

#include "liblogicanalyzer.h"
#include <stdint.h>
#include <stdlib.h>

#define LA_DEFAULT_PAGE_SHIFT 12
#define LA_DEFAULT_PAGE_SIZE (1 << LA_DEFAULT_PAGE_SHIFT)

#define LA_SPLIT_UNSET 0xFFFFFFFFFFFFFFFF

typedef enum {
	LA_SPLIT_STATUS_BLANK = 0,
	LA_SPLIT_STATUS_MARKING = 1,
} LASplitStatus;

typedef struct _LANodeDefine{
	char *label;
	size_t labelLength;
	char *value;
	size_t valueLength;
	struct _LANodeDefine *next;
} LANodeDefine;

typedef struct _LANodeSplit{
	char *value;
	size_t length;
	struct _LANodeSplit *next;
} LANodeSplit;

#define LA_COMPILER_DEFINE_TEXT "#define"

LAErrorCode LAMemoryMapFile(char *filename, LAMappedFile *file);
LAErrorCode LAMemoryUnmapFile(LAMappedFile *file);
size_t LAPageAlignSize(size_t size);
LAErrorCode LARemapFile(LAMappedFile *file, size_t length);
LAErrorCode LACreateMemoryFile(LAMappedFile *file, size_t capacity);
LAErrorCode LAMappedFileWrite(LAMappedFile *file, uint8_t *data, size_t length);

LAErrorCode LASplitCreateNode(LANodeSplit **node, size_t length);
LAErrorCode LASplitDestroyNodes(LANodeSplit **startNode);
LAErrorCode LASplitCopyFromFile(LANodeSplit *node, LAMappedFile *file, size_t offset);
LAErrorCode LASplitLineIntoNodes(LAMappedFile *file, size_t *offset, LANodeSplit **splitStart);

LAErrorCode LACreateDefineNode(LANodeDefine **node, char *label, LANodeSplit *startSplit);
LAErrorCode LACreateNewNodeDefine(LANodeSplit *splitStart, LANodeDefine **defineStart);

#endif // LIB_LA_COMPILER_H
