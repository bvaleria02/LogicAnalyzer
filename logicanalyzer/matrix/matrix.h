#ifndef LA_MATRIX
#define LA_MATRIX

#include "../liblogicanalyzer.h"
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
	LA_MAT_READ			= 0x01,
	LA_MAT_WRITE		= 0x02,
	LA_MAT_IS_VIEW		= 0x04,
	LA_MAT_TRANSPOSED	= 0x08,
	LA_MAT_IS_SET		= 0x10,
	LA_MAT_IS_ALLOCATED	= 0x20
} LAMatFlags;

typedef struct {
	size_t colOffset;
	size_t rowOffset;
	size_t virtualWidth;
	size_t virtualHeight;
} LAMatViewData_t;

typedef struct {
	LAMatFlags flags;
	size_t col;
	size_t row;
	LAMatViewData_t viewData;
	double *data;
} LAMat_t;

typedef enum {
	LA_MAT_PRINT_SHOW_META			= 0x1,
	LA_MAT_PRINT_SHOW_DATA			= 0x2,
	LA_MAT_PRINT_SHOW_LABEL			= 0x4,
	LA_MAT_PRINT_SHOW_VIEWDATA		= 0x8
} LAMatPrintFlags;

#define LA_MAT_INDEX_FAIL 0xFFFFFFFFFFFFFFFF

// create.c
LAErrorCode LAMatCreateFromArrayFlags(LAMat_t *mat, size_t row, size_t col, double *data, LAMatFlags flags);
LAErrorCode LAMatCreateFromArray(LAMat_t *mat, size_t row, size_t col, double *data);
LAErrorCode LAMatCreateDynamicFlags(LAMat_t *mat, size_t row, size_t col, LAMatFlags flags);
LAErrorCode LAMatCreateDynamic(LAMat_t *mat, size_t row, size_t col);
LAErrorCode LAMatCreateViewFlags(LAMat_t *dest, LAMat_t *src, size_t r0, size_t c0, size_t rf, size_t cf, LAMatFlags newFlags);
LAErrorCode LAMatCreateView(LAMat_t *dest, LAMat_t *src, size_t r0, size_t c0, size_t rf, size_t cf);

// init.c
LAErrorCode LAMatInit(LAMat_t *mat);

// destroy.c
LAErrorCode LAMatDestroy(LAMat_t *mat);

// flags.c
bool LAMatIsReadable(LAMat_t *mat);
bool LAMatIsWritable(LAMat_t *mat);
bool LAMatIsView(LAMat_t *mat);
bool LAMatIsTransposed(LAMat_t *mat);
bool LAMatIsSet(LAMat_t *mat);
bool LAMatIsAllocated(LAMat_t *mat);
bool LAMatIsValid(LAMat_t *mat);
bool LAMatAreSameDim(LAMat_t *m1, LAMat_t *m2);
bool LAMatAreMulDim(LAMat_t *m1, LAMat_t *m2, LAMat_t *m3);
bool LAMatIsSquare(LAMat_t *m1);
bool LAMatMatchDim(LAMat_t *m, size_t h, size_t w);

// access.c
size_t LAMatGetIndex(LAMat_t *mat, size_t row, size_t col);
size_t LAMatGetWidth(LAMat_t *mat);
size_t LAMatGetHeight(LAMat_t *mat);
LAErrorCode LAMatShape(LAMat_t *m, size_t *r, size_t *c);
bool LAMatValidateCoordinates(LAMat_t *mat, size_t row, size_t col);
LAErrorCode LAMatGet(LAMat_t *mat, size_t row, size_t col, double *value);
LAErrorCode LAMatSet(LAMat_t *mat, size_t row, size_t col, double value);

// print.c
LAErrorCode LAMatPrint(LAMat_t *mat);
LAErrorCode LAMatPrintWithLabel(LAMat_t *mat, char *label);
LAErrorCode LAMatPrintBackend(LAMat_t *mat, LAMatPrintFlags flags, char *label);

// add.c
LAErrorCode LAMatAdd(LAMat_t *m1, LAMat_t *m2, LAMat_t *re);
LAErrorCode LAMatAddCol(LAMat_t *m, size_t c1, size_t c2, double f);
LAErrorCode LAMatAddRow(LAMat_t *m, size_t r1, size_t r2, double f);

// sub.c
LAErrorCode LAMatSub(LAMat_t *m1, LAMat_t *m2, LAMat_t *re);
LAErrorCode LAMatSubCol(LAMat_t *m, size_t c1, size_t c2, double f);
LAErrorCode LAMatSubRow(LAMat_t *m, size_t r1, size_t r2, double f);

// mul.c
LAErrorCode LAMatMul(LAMat_t *m1, LAMat_t *m2, LAMat_t *m3);
LAErrorCode LAMatMulCum(LAMat_t *m1, LAMat_t *m2, LAMat_t *m3);
LAErrorCode LAMatMulCumFactor(LAMat_t *m1, LAMat_t *m2, LAMat_t *m3, double factor);
LAErrorCode LAMatMulCol(LAMat_t *m, size_t c, double f);
LAErrorCode LAMatMulRow(LAMat_t *m, size_t r, double f);

// matlab.c
LAErrorCode LAMatFill(LAMat_t *m, double value);
LAErrorCode LAMatZeros(LAMat_t *m);
LAErrorCode LAMatOnes(LAMat_t *m);
LAErrorCode LAMatRandi(LAMat_t *m, double vmin, double vmax);
LAErrorCode LAMatRand(LAMat_t *m);
LAErrorCode LAMatEye(LAMat_t *m);

// swap.c
LAErrorCode LAMatRowSwap(LAMat_t *m, size_t r1, size_t r2);
LAErrorCode LAMatColSwap(LAMat_t *m, size_t c1, size_t c2);
LAErrorCode LAMatValueSwap(LAMat_t *m, size_t r1, size_t c1, size_t r2, size_t c2);

// getHighest.c
LAErrorCode LAMatGetHighestRow(LAMat_t *m, size_t c, size_t *r, bool useFabs);
LAErrorCode LAMatGetHighestCol(LAMat_t *m, size_t r, size_t *c, bool useFabs);
LAErrorCode LAMatGetHighestRowFrom(LAMat_t *m, size_t c, size_t r0, size_t *r, bool useFabs);
LAErrorCode LAMatGetHighestColFrom(LAMat_t *m, size_t r, size_t c0, size_t *c, bool useFabs);

// lu.c
LAErrorCode LAMatLU(LAMat_t *a, LAMat_t *l, LAMat_t *u, LAMat_t *p);
LAErrorCode LAMatInverseLU(LAMat_t *a, LAMat_t *l, LAMat_t *u, LAMat_t *p, LAMat_t *z, LAMat_t *m);
LAErrorCode LAMatInverseFromLU(LAMat_t *l, LAMat_t *u, LAMat_t *z, LAMat_t *m);

// gaussjordan.c
LAErrorCode LAMatTriU(LAMat_t *a, LAMat_t *p);
LAErrorCode LAMatInverseGaussJordan(LAMat_t *a, LAMat_t *p);

// copy.c
LAErrorCode LAMatCopy(LAMat_t *src, LAMat_t *dest);

// norm.c
LAErrorCode LAMatFrobeniusNormBounded(LAMat_t *m, size_t r0, size_t c0, size_t rf, size_t cf, double *norm);
LAErrorCode LAMatFrobeniusNormShape(LAMat_t *m, size_t r0, size_t c0, size_t h, size_t w, double *norm);
LAErrorCode LAMatFrobeniusNorm(LAMat_t *m, double *norm);
LAErrorCode LAMatFrobeniusNormRow(LAMat_t *m, size_t r, double *norm);
LAErrorCode LAMatFrobeniusNormCol(LAMat_t *m, size_t c, double *norm);

// qr.c
LAErrorCode LAMatQR(LAMat_t *a, LAMat_t *q, LAMat_t *r);
LAErrorCode LAMatHMatrix(LAMat_t *hMat, LAMat_t *uVec);
LAErrorCode LAMatHMatrixAndNormalize(LAMat_t *hMat, LAMat_t *uVec);
LAErrorCode LAMatUVector(LAMat_t *a, LAMat_t *uVec, size_t col);

// set.c
LAErrorCode LAMatTranspose(LAMat_t *m);

// kronecker.c
LAErrorCode LAMatKronecker(LAMat_t *a, LAMat_t *b, LAMat_t *c);

#endif //LA_MATRIX
