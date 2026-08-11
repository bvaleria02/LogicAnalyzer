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

LAErrorCode LAMemoryMapFile(char *filename, LAMappedFile *file){
	LA_HANDLE_NULLPTR(filename, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(file, 		LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	int fd = -1;
	uint8_t isFileOpen = 0;

	fd =  open(filename, O_RDONLY);
	if(fd < 0){
		fprintf(stderr, "File not found: %s\n", filename);
		code = LA_ERROR_FILENOTFOUND;
		goto handle_error;
	}

	isFileOpen = 1;
	struct stat sb;
	if(fstat(fd, &sb) < 0){
		fprintf(stderr, "File size error\n");
		code = LA_ERROR_FILE;
		goto handle_error;
	}

	file->data = mmap(NULL, sb.st_size,  PROT_READ, MAP_PRIVATE, fd, 0);
	if(file->data == MAP_FAILED){
		fprintf(stderr, "File mmap error\n");
		code = LA_ERROR_MMAP;
		goto handle_error;
	}

	file->length = sb.st_size;
	file->capacity = sb.st_size;
	file->readOffset = 0;

	close(fd);
	return LA_NO_ERROR;

handle_error:
	if(isFileOpen){
		close(fd);
		isFileOpen = 0;
	}

	file->length = 0;
	file->capacity = 0;
	file->data 	 = NULL;

	LA_RAISE_ERROR(code);
	return code;
}
