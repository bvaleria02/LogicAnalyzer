#include "../liblogicanalyzer.h"
#include "loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

LAErrorCode LAShaderLoader(const char *path, uint8_t **output, size_t *size){
	LA_HANDLE_NULLPTR(path,   LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(output, LA_PROPAGATE_ERROR);
	
	int fd 				= -1;
	LAErrorCode code 	= LA_NO_ERROR;
	struct stat sb		= {0};
	bool isFileOpen 	= false;
	bool isMalloced   	= false;

	// Open file
	fd = open(path, O_RDONLY);
	if(fd < 0) goto handle_file_not_found_error;
	isFileOpen = true;

	// Read size
	if(fstat(fd, &sb) < 0) goto handle_file_error;
	if(sb.st_size == 0) goto handle_empty_file;

	// Malloc
	(*output) = (uint8_t *)malloc(sb.st_size + 1);
	if((*output) == NULL) goto handle_malloc_error;
	isMalloced = true;

	// Read
	ssize_t bytesRead = read(fd, (*output), sb.st_size);
	if((bytesRead != sb.st_size) || (bytesRead == -1)) goto handle_read_error;
	(*output)[sb.st_size] = '\0';
	if(size != NULL) (*size) = sb.st_size;

	goto cleanup;
	
handle_empty_file:
	fprintf(stderr, "Error, empty file.\n");
	code = LA_ERROR_ZEROLENGTH;
	goto cleanup;

handle_read_error:
	fprintf(stderr, "Error reading file, length mismatch.\n");
	code = LA_ERROR_FILEREAD;
	goto cleanup;

handle_malloc_error:
	fprintf(stderr, "Error malloc-ing memory to copy file.\n");
	code = LA_ERROR_MALLOC;
	goto cleanup;

handle_file_error:
	fprintf(stderr, "Error reading file.\n");
	code = LA_ERROR_FILE;
	goto cleanup;

handle_file_not_found_error:
	fprintf(stderr, "Error, not such file or permision.\n");
	code = LA_ERROR_FILENOTFOUND;
	goto cleanup;

cleanup:
	if(isFileOpen   && fd >= 0         ) 		close(fd);
	if(code && isMalloced && (*output) != NULL) free((*output));
	if(code)							 		(*output) = NULL;
	if(code && size != NULL)			 		(*size) = 0x0;
	return code;
}
