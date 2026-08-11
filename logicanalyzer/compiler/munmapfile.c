#include <stdio.h>
#include <stdlib.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "../compiler.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

LAErrorCode LAMemoryUnmapFile(LAMappedFile *file){
	LA_HANDLE_NULLPTR(file, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(file->data, 	LA_PROPAGATE_ERROR);

	if(file->data != NULL && file->capacity != 0){
		munmap(file->data, file->capacity);
		file->data = NULL;
		file->length = 0;
		file->capacity = 0;
	}

	return LA_NO_ERROR;
}
