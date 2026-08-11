#ifndef MULTI_N
#define MULTI_N

#include <stdint.h>

#define	_F96_SIZE		96
#define	_F128_SIZE		128

// 2 * 64 = 128 
#define BUFFER_SIZE		 32
#define BUFFER_N_SIZE(x) x / BUFFER_SIZE
#define BUFFER_128_SIZE 2

typedef uint32_t multiNBuffer;

// src/fixedpoint/multin.c
void multiNSetInt(multiNBuffer *y, uint8_t size, uint64_t value);
void multiNBlank(multiNBuffer *y, uint64_t size);
void multiNMul(multiNBuffer *y1, uint64_t size1, multiNBuffer *y2, uint64_t size2, multiNBuffer *a, uint64_t size3);
void multiNPrint(multiNBuffer *y, uint64_t size);
void multiNPrintBinary(multiNBuffer *y, uint64_t size);
void multiNIndex(multiNBuffer *y, uint64_t size, uint64_t index, uint64_t *highIndex, uint64_t *lowIndex);
uint8_t multiNGetBit(multiNBuffer *y, uint64_t size, uint64_t index);
void multiNSetBit(multiNBuffer *y, uint64_t size, uint64_t index, uint8_t value);
void multiNLeftShift(multiNBuffer *y, uint64_t size, uint64_t shift);
void multiNRightShift(multiNBuffer *y, uint64_t size, uint64_t shift);
void multiNAdd(multiNBuffer *y1, uint64_t size1, multiNBuffer *y2, uint64_t size2, multiNBuffer *a, uint64_t size3);
void multiNSub(multiNBuffer *y1, uint64_t size1, multiNBuffer *y2, uint64_t size2, multiNBuffer *a, uint64_t size3);
int8_t multiNCompare(multiNBuffer *y1, uint64_t size1, multiNBuffer *y2, uint64_t size2);
uint8_t multiNIsZero(multiNBuffer *y, uint64_t size);
uint64_t multiNAbsSize(multiNBuffer *y, uint64_t size);
void multiNCopy(multiNBuffer *x, uint64_t sizeX, multiNBuffer *y, uint64_t sizeY);
void multiNDiv(multiNBuffer *y1, uint64_t size1, multiNBuffer *y2, uint64_t size2, multiNBuffer *a, uint64_t size3, multiNBuffer *b, uint64_t size4, multiNBuffer *tmp1, multiNBuffer *tmp2);

#endif // MULTI_N
