#ifndef LA_COMPLEX
#define LA_COMPLEX

typedef struct {
	double re;
	double im;
} LAComplexDouble;

typedef struct {
	float re;
	float im;
} LAComplexFloat;

typedef LAComplexDouble LAComplex;

LAErrorCode LAComplexDoubleModulo(LAComplexDouble *cmpx, double *res);
LAErrorCode LAComplexFloatModulo(LAComplexFloat *cmpx, float *res);

LAErrorCode LAComplexDoubleArg(LAComplexDouble *cmpx, double *res);
LAErrorCode LAComplexFloatArg(LAComplexFloat *cmpx, float *res);

LAErrorCode LAComplexDoubleRoots(LAComplexDouble *r1, LAComplexDouble *r2, double a, double b, double c);
LAErrorCode LAComplexDoubleRootsArray(LAComplexDouble *r1, LAComplexDouble *r2, double *coef, const size_t size);

#endif // LA_COMPLEX
