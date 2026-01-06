#include <stdio.h>
#include <stdlib.h>

#define _GNU_SOURCE 1
#include <unistd.h>
#include <sys/mman.h>

#include "../liblogicanalyzer.h"
#include "../compiler.h"
#include <fcntl.h>
#include <sys/stat.h>

LAErrorCode LARemapFile(LAMappedFile *file, size_t length){
	LA_HANDLE_NULLPTR(file,			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(file->data,	LA_PROPAGATE_ERROR);

	size_t newSize = file->length + length;

	// There is no need to remap
	if(newSize <= file->capacity){
		return LA_NO_ERROR;
	}

	if(newSize < (file->length << 1)){
		newSize = file->length << 1;
	}

	newSize = LAPageAlignSize(newSize);

#ifdef mremap
	// Using mremap if available
	void *newMap = mremap(file->data, file->capacity, newSize, MREMAP_MAYMOVE);
	if(newMap == MAP_FAILED){
		LA_RAISE_ERROR(LA_ERROR_MMAP);
		return LA_ERROR_MMAP;
	}
#else
	// Using manual and slow mmap/munmap if mremap is not available
	void *newMap = mmap(NULL, newSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if(newMap == MAP_FAILED){
		LA_RAISE_ERROR(LA_ERROR_MMAP);
		return LA_ERROR_MMAP;
	}

	memcpy(newMap, file->data, file->capacity);
	int munmapret = munmap(file->data, file->capacity);
	if(munmapret < 0){
		munmap(newMap, newSize);
		LA_RAISE_ERROR(LA_ERROR_MUNMAP);
		return LA_ERROR_MUNMAP;
	}
#endif

	file->data = newMap;
	file->capacity = newSize;
	return LA_NO_ERROR;
}

LAErrorCode LAMappedFileWrite(LAMappedFile *file, uint8_t *data, size_t length){
	LA_HANDLE_NULLPTR(file,			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(file->data,	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(data,			LA_PROPAGATE_ERROR);
	
	LAErrorCode code = LARemapFile(file, length);
	if(code != LA_NO_ERROR){
		// Is not safe to copy, abort
		LA_RAISE_ERROR(code);
		return code;
	}

	memcpy(&(file->data[file->length]), data, length);
	file->length = file->length + length;

	return LA_NO_ERROR;
}
