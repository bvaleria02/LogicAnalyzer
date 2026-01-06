#include <stdio.h>
#include <stdlib.h>
#include "../liblogicanalyzer.h"
#include "../compiler.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

size_t LAPageAlignSize(size_t size){
	if(size == 0){
		return LA_DEFAULT_PAGE_SIZE;
	}

	if((size % LA_DEFAULT_PAGE_SIZE) == 0 && size > 0){
		return size;
	}

	size = size >> LA_DEFAULT_PAGE_SHIFT;
	size += 1;
	size = size << LA_DEFAULT_PAGE_SHIFT;

	return size;
}


LAErrorCode LACreateMemoryFile(LAMappedFile *file, size_t capacity){
	LA_HANDLE_NULLPTR(file, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	// Align capacity to 4096 bytes pages
	capacity = LAPageAlignSize(capacity);
	
	// Munmap later with LAMemoryUnmapFile
	file->data = mmap(NULL, capacity, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if(file->data == MAP_FAILED){
		code = LA_ERROR_MMAP;
		goto handle_error;
	}

	file->length = 0;
	file->capacity = capacity;

	return LA_NO_ERROR;

handle_error:
	file->data 		= NULL;
	file->length 	= 0;
	file->capacity 	= 0;

	LA_RAISE_ERROR(code);
	return code;
}
