#ifndef LA_NUMERIC_METHODS_NEWTON_RAPHSON
#define LA_NUMERIC_METHODS_NEWTON_RAPHSON

#include "../liblogicanalyzer.h"
#include <stdlib.h>
#include <stdint.h>
#include "../matrix/matrix.h"

typedef enum {
	LA_NEWTON_RAPHSON_USE_ATOL			= 0x1,
	LA_NEWTON_RAPHSON_USE_RTOL			= 0x2,
	LA_NEWTON_RAPHSON_USE_FTOL			= 0x4,
	LA_NEWTON_RAPHSON_AVOID_SINGULARITY	= 0x8,
	LA_NEWTON_RAPHSON_CHECK_INF			= 0x10,
	LA_NEWTON_RAPHSON_CHECK_NAN			= 0x20,
} LANewtonRaphsonFlags;

typedef enum {
	LA_NEWTON_RAPHSON_OK				= 0x0,
	LA_NEWTON_RAPHSON_SINGULARITY		= 0x1,
	LA_NEWTON_RAPHSON_NO_CONVERGENCE	= 0x2,
	LA_NEWTON_RAPHSON_MATRIX_ERROR		= 0x3,
} LANewtonRaphsonStatus;

typedef double (*LANewtonRaphsonFunc)(double, void *);
typedef LAErrorCode (*LANewtonRaphsonMultiFunc)(size_t, LAMat_t *, LAMat_t *, void *ptr);
typedef LAErrorCode (*LANewtonRaphsonMultiJacobianFunc)(size_t, LAMat_t *, LAMat_t *, void *ptr);

typedef struct {
	size_t nmax;
	double atol;
	double rtol;
	double ftol;
	double eps;
	double reps;
	LANewtonRaphsonFlags flags;
	LANewtonRaphsonStatus status;
} LANewtonRaphsonData;

double LANewtonRaphson(LANewtonRaphsonFunc f, LANewtonRaphsonFunc df, double x0, void *ptr, size_t nmax, double atol, double rtol, double ftol, double eps, double reps, LANewtonRaphsonFlags flags, LANewtonRaphsonStatus *status);
double LANewtonRaphsonCompact(LANewtonRaphsonFunc f, LANewtonRaphsonFunc df, double x0, void *ptr, LANewtonRaphsonData *data);
LAErrorCode LANewtonRaphsonMulti(LANewtonRaphsonMultiFunc fc, LANewtonRaphsonMultiJacobianFunc jc, LAMat_t *x0, LAMat_t *xf, LAMat_t *j, LAMat_t *f, void *ptr, const size_t nmax, const double atol, const double rtol, const double ftol, const double eps, const double reps, LANewtonRaphsonFlags flags, LANewtonRaphsonStatus *status);
LAErrorCode LANewtonRaphsonMultiCompact(LANewtonRaphsonMultiFunc fc, LANewtonRaphsonMultiJacobianFunc jc, LAMat_t *x0, LAMat_t *xf, LAMat_t *j, LAMat_t *f, void *ptr, LANewtonRaphsonData *data);

#endif //LA_NUMERIC_METHODS_NEWTON_RAPHSON
