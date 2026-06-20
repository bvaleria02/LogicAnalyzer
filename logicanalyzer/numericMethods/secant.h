#ifndef LA_NUMERIC_METHODS_SECANT
#define LA_NUMERIC_METHODS_SECANT

#include "../liblogicanalyzer.h"
#include <stdlib.h>
#include <stdint.h>

typedef enum {
	LA_SECANT_USE_ATOL			= 0x1,
	LA_SECANT_USE_RTOL			= 0x2,
	LA_SECANT_USE_FTOL			= 0x4,
	LA_SECANT_AVOID_SINGULARITY	= 0x8,
	LA_SECANT_CHECK_INF			= 0x10,
	LA_SECANT_CHECK_NAN			= 0x20,
} LASecantFlags;

typedef enum {
	LA_SECANT_OK				= 0x0,
	LA_SECANT_SINGULARITY		= 0x1,
	LA_SECANT_NO_CONVERGENCE	= 0x2,
} LASecantStatus;

typedef double (*LASecantFunc)(double, void *);

typedef struct {
	size_t nmax;
	double atol;
	double rtol;
	double ftol;
	double eps;
	double reps;
	LASecantFlags flags;
	LASecantStatus status;
} LASecantData;

double LASecant(LASecantFunc f, double h0, double x0, void *ptr, size_t nmax, double atol, double rtol, double ftol, double eps, double reps, LASecantFlags flags, LASecantStatus *status);
double LASecantCompact(LASecantFunc f, double h0, double x0, void *ptr, LASecantData *data);

#endif //LA_NUMERIC_METHODS_SECANT
