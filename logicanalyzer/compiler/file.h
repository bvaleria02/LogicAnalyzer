#ifndef LA_COMPILER_FILE
#define LA_COMPILER_FILE

// compiler/filereader.c
bool LAMappedFileIsValid(LAMappedFile *file);
bool LAMappedFileCanRead(LAMappedFile *file, size_t bytes);
size_t LAMappedFileGetFreeBytes(LAMappedFile *file);
LAErrorCode LAMappedFileReadBytes(LAMappedFile *file, uint8_t *output, size_t bytesRequest, size_t *bytesRead, bool strictSize);
LAErrorCode LAMappedFileSeekBytes(LAMappedFile *file, size_t bytesRequest, bool strictSize);
uint64_t LAParseIntFromArray(uint8_t *array, size_t size);
LAErrorCode LAMappedFileReadI8(LAMappedFile *file, uint8_t *output);
LAErrorCode LAMappedFileReadI16(LAMappedFile *file, uint16_t *output);
LAErrorCode LAMappedFileReadI32(LAMappedFile *file, uint32_t *output);
LAErrorCode LAMappedFileReadI64(LAMappedFile *file, uint64_t *output);

#endif //LA_COMPILER_FILE
