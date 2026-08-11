#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "../compiler.h"

bool LAMappedFileIsValid(LAMappedFile *file){
	LA_HANDLE_NULLPTR(file, 	false);
	
	if(file->data 		== NULL) return false;

	// Note: capacity is not used for normal file read
	// As LAMappedFile supports MAP_ANONYMOUS, this is kept
	// as the size of the mapped page, which can be different to
	// the logic end of data (length)
	// But capacity == 0 means:
	//	- corrupted LAMappedFile
	//  - mmap failed (but in this case, data is NULL)
	// As this is intended of read and write, this is kept

	if(file->capacity 	== 0   ) return false;
	if(file->capacity < file->length) return false;

	return true;
}

bool LAMappedFileCanRead(LAMappedFile *file, size_t bytes){
	LA_HANDLE_NULLPTR(file, 	false);

	// Invalid file
	if(!LAMappedFileIsValid(file)) return false;

	// Zero-length, can't read anything if bytes > 0
	if(file->length == 0 && bytes > 0) return false;

	// readOffset is corrupted
	if(file->readOffset > file->length){
		// corrects readOffset
		file->readOffset = file->length;
		return false;
	}

	// bytes is larger than available bytes
	if(bytes > file->length - file->readOffset) return false;

	size_t finalOffset = file->readOffset + bytes;

	// Overflow; can't read
	if(finalOffset < file->readOffset) return false;

	return (finalOffset <= file->length);
}

size_t LAMappedFileGetFreeBytes(LAMappedFile *file){
	LA_HANDLE_NULLPTR(file, 	0);

	// Invalid file
	if(!LAMappedFileIsValid(file)) return 0;

	if(file->readOffset >= file->length) return 0;

	return (file->length - file->readOffset);
}

LAErrorCode LAMappedFileReadBytes(LAMappedFile *file, uint8_t *output, size_t bytesRequest, size_t *bytesRead, bool strictSize){
	// If error, make sure bytesRead is 0;
	// bytesRead is NULL-able
	if(bytesRead != NULL) (*bytesRead) = 0;

	LA_HANDLE_NULLPTR(file,			LA_PROPAGATE_ERROR);
	if(!LAMappedFileIsValid(file)){
		LA_RAISE_ERROR(LA_ERROR_FILE);
		return LA_ERROR_FILE;
	}
	
	// output is NULL-able only if bytesRequest is 0
	if(output == NULL && bytesRequest == 0) return LA_NO_ERROR;
	LA_HANDLE_NULLPTR(output, LA_PROPAGATE_ERROR);
	
	// Avoid variables inside if/while/for (Rule 4)
	size_t availableBytes = 0;

	if(!LAMappedFileCanRead(file, bytesRequest) && strictSize){
		LA_RAISE_ERROR(LA_ERROR_OUTOFRANGE);
		return LA_ERROR_OUTOFRANGE;
	} else if (!strictSize){
		availableBytes = LAMappedFileGetFreeBytes(file);
		if(availableBytes < bytesRequest) bytesRequest = availableBytes;
	}

	// Why? Just to be sure
	// This is kept because the TLV parser can get a zero-length value (NOP, END)
	if(bytesRequest == 0) return LA_NO_ERROR;

	// It's ok to copy
	memcpy(output, file->data + file->readOffset, bytesRequest);

	// Update offset. LAMappedFileCanRead makes sure offset will not overflow
	file->readOffset += bytesRequest;

	// bytesRead is NULL-able
	if(bytesRead != NULL) (*bytesRead) = bytesRequest;

	return LA_NO_ERROR;
}

LAErrorCode LAMappedFileSeekBytes(LAMappedFile *file, size_t bytesRequest, bool strictSize){
	LA_HANDLE_NULLPTR(file,			LA_PROPAGATE_ERROR);

	if(!LAMappedFileIsValid(file)){
		LA_RAISE_ERROR(LA_ERROR_FILE);
		return LA_ERROR_FILE;
	}
	
	size_t availableBytes = 0;
	if(!LAMappedFileCanRead(file, bytesRequest) && strictSize){
		LA_RAISE_ERROR(LA_ERROR_OUTOFRANGE);
		return LA_ERROR_OUTOFRANGE;
	} else if (!strictSize){
		availableBytes = LAMappedFileGetFreeBytes(file);
		if(availableBytes < bytesRequest) bytesRequest = availableBytes;
	}

	// Why? Just to be sure
	// This is kept because the TLV parser can get a zero-length value (NOP, END)
	if(bytesRequest == 0) return LA_NO_ERROR;
	
	// Update offset. LAMappedFileCanRead makes sure offset will not overflow
	file->readOffset += bytesRequest;

	return LA_NO_ERROR;
}

#define LA_INT_READER_MAX (64 / 8)
// Multi-byte integers are explicit little endian
// to keep consistency with the rest of the app, as it assumes LE for arrays

uint64_t LAParseIntFromArray(uint8_t *array, size_t size){
	LA_HANDLE_NULLPTR(array, 	0);
	if(size == 0) return 0;

	// Keep in mind, if size > 7, array[8] and onwards will be discarded
	if(size > LA_INT_READER_MAX) size = LA_INT_READER_MAX;

	uint64_t value = 0;
	for(size_t i = 0; i < size; i++){
		value |= ((uint64_t) array[i]) << (i * 8);
	}

	return value;
}

LAErrorCode LAMappedFileIntReadWrapper(LAMappedFile *file, uint64_t *output, size_t bytesRequest){
	LA_HANDLE_NULLPTR(file,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(output,	LA_PROPAGATE_ERROR);

	if(bytesRequest > LA_INT_READER_MAX) bytesRequest = LA_INT_READER_MAX;
	if(bytesRequest == 0){
		(*output) = 0;
		return LA_NO_ERROR;
	}

	uint8_t value[LA_INT_READER_MAX] 	= {0};
	size_t bytesRead 					= 0;

	LAErrorCode code = LAMappedFileReadBytes(file, value, bytesRequest, &bytesRead, true);
	if(code) return code;
	
	if(bytesRead != bytesRequest){
		LA_RAISE_ERROR(LA_ERROR_FILEREAD);
		return LA_ERROR_FILEREAD;
	}

	(*output) = LAParseIntFromArray(value, bytesRequest);
	return LA_NO_ERROR;
}

LAErrorCode LAMappedFileReadI8(LAMappedFile *file, uint8_t *output){
	LA_HANDLE_NULLPTR(file, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(output, 	LA_PROPAGATE_ERROR);

	uint64_t value = 0;
	LAErrorCode code = LAMappedFileIntReadWrapper(file, &value, 1);
	if(code) return code;

	(*output) = (uint8_t) value;
	return LA_NO_ERROR;
}

LAErrorCode LAMappedFileReadI16(LAMappedFile *file, uint16_t *output){
	LA_HANDLE_NULLPTR(file, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(output, 	LA_PROPAGATE_ERROR);

	uint64_t value = 0;
	LAErrorCode code = LAMappedFileIntReadWrapper(file, &value, 2);
	if(code) return code;

	(*output) = (uint16_t) value;
	return LA_NO_ERROR;
}

LAErrorCode LAMappedFileReadI32(LAMappedFile *file, uint32_t *output){
	LA_HANDLE_NULLPTR(file, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(output, 	LA_PROPAGATE_ERROR);

	uint64_t value = 0;
	LAErrorCode code = LAMappedFileIntReadWrapper(file, &value, 4);
	if(code) return code;

	(*output) = (uint32_t) value;
	return LA_NO_ERROR;
}

LAErrorCode LAMappedFileReadI64(LAMappedFile *file, uint64_t *output){
	LA_HANDLE_NULLPTR(file, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(output, 	LA_PROPAGATE_ERROR);

	uint64_t value = 0;
	LAErrorCode code = LAMappedFileIntReadWrapper(file, &value, 8);
	if(code) return code;

	(*output) = (uint64_t) value;
	return LA_NO_ERROR;
}
