#ifndef LA_NUMERIC_METHODS_SOLVER
#define LA_NUMERIC_METHODS_SOLVER

#include "../liblogicanalyzer.h"
#include <stdlib.h>
#include "../matrix/matrix.h"
#include "newtonRaphson.h"

typedef enum {
	LA_SOLVER_EULER			= 0,
	LA_SOLVER_IMPLICIT		= 1,
	LA_SOLVER_TRAPEZOIDAL	= 2,
	LA_SOLVER_BDF2			= 3,
	LA_SOLVER_BDF3			= 4,
	LA_SOLVER_RK2			= 5,
	LA_SOLVER_HEUN			= 6,
	LA_SOLVER_RK3			= 7,
	LA_SOLVER_RK4			= 8,
	LA_SOLVER_RK4_38		= 9,
	LA_SOLVER_RK5			= 10,
	LA_SOLVER_RK6			= 11,
	LA_SOLVER_RK8			= 12,
	LA_SOLVER_ODE12			= 13,
	LA_SOLVER_BS23			= 14,
	LA_SOLVER_RKF45			= 15,
	LA_SOLVER_RKF78			= 16,
	LA_SOLVER_DOP45			= 17,
	LA_SOLVER_RKCK45		= 18,
	LA_SOLVER_LMS_AB2		= 19,
	LA_SOLVER_LMS_AB3		= 20,
	LA_SOLVER_LMS_AB4		= 21,
	LA_SOLVER_LMS_AB5		= 22,
	LA_SOLVER_LMS_AB6		= 23,
	LA_SOLVER_LMS_AB7		= 24,
	LA_SOLVER_RKI_GL_4		= 25,
	LA_SOLVER_RKI_GL_6		= 26,
	LA_SOLVER_RKI_R1A3		= 27,
	LA_SOLVER_RKI_R1A5		= 28,
	LA_SOLVER_RKI_R2A3		= 29,
	LA_SOLVER_RKI_R2A5		= 30,
	LA_SOLVER_RKI_L3A2		= 31,
	LA_SOLVER_RKI_L3A4		= 32,
	LA_SOLVER_RKI_L3A6		= 33,
	LA_SOLVER_RKI_L3A8		= 34,
	LA_SOLVER_RKI_L3B2		= 35,
	LA_SOLVER_RKI_L3B4		= 36,
	LA_SOLVER_RKI_L3B6		= 37,
	LA_SOLVER_RKI_L3B8		= 38,
	LA_SOLVER_RKI_L3C2		= 39,
	LA_SOLVER_RKI_L3C4		= 40,
	LA_SOLVER_RKI_L3C6		= 41,
	LA_SOLVER_RKI_L3C8		= 42,
	LA_SOLVER_ILMS_AM2		= 43,
	LA_SOLVER_ILMS_AM3		= 44,
	LA_SOLVER_ILMS_AM4		= 45,
	LA_SOLVER_ILMS_AM5		= 46
} LASolverType;

typedef double (*LASolverCallback)(double, double, void *, double);
typedef double (*LASolverCallbackDf)(double, double, void *, double);
typedef double (*LASolverArrayCallback)(double, double, size_t, void *, double);

typedef struct {
	size_t nmax;
	double atol;
	LASolverCallbackDf df;
	double *prev;
	size_t pIndex;
	double subTime;
	double minTime;
} LASolverEulerImplicitDetails;

typedef struct {
	LASolverArrayCallback f;
	void *p2;
	size_t n;
} LASolverFunctionTranslatorData;

typedef LASolverEulerImplicitDetails LASolverTrapezoidalDetails;
typedef LASolverEulerImplicitDetails LASolverBDF2Details;
typedef LASolverEulerImplicitDetails LASolverRKFDetails;

typedef struct {
	size_t n;
	void *ptr;
	LASolverArrayCallback f;
	double x1;
	double x2;
	double t1;
	double t2;
	double eps;
} LASolverNewtonAdaptorDetails;

typedef struct {
	double y;
	double h;
	double yn_1;
	double yn_2;
	LANewtonRaphsonFunc f;
	LANewtonRaphsonFunc df;
	void *ptr;
	double c0;
	double fv;
} LASolverImplicitAdaptorDetails;

#define LA_SOLVER_LMS_FMEM_SIZE 8

typedef struct {
	LAMat_t *a;
	LAMat_t *b;
	LASolverArrayCallback f;
	size_t n;
	void *fptr;
	double h;
	double eps;
	double tn;
	double yn;
	double reps;
} LASolverImplicitRKData;

double LASolverEuler(double y0, double t0, double h, LASolverCallback f, void *ptr);
double LASolverEulerImplicit(double y0, double t0, double h, LASolverCallback f, void *ptr, void *de);
double LASolverTrapezoidal(double y0, double t0, double h, LASolverCallback f, void *ptr, void *de);
double LASolverBDF2(double y0, double t0, double h, LASolverCallback f, void *ptr, void *data);
double LASolverRK2(double y0, double t0, double h, LASolverCallback f, void *ptr);
double LASolverHeun(double y0, double t0, double h, LASolverCallback f, void *ptr);
double LASolverRK4(double y0, double t0, double h, LASolverCallback f, void *ptr);
double LASolverRK4_38(double y0, double t0, double h, LASolverCallback f, void *ptr);
double LASolverRK6(double y0, double t0, double h, LASolverCallback f, void *ptr);
double LASolverRK8(double y0, double t0, double h, LASolverCallback f, void *ptr);
double LASolverRKF45(double y0, double t0, double h, LASolverCallback f, void *ptr, void *data);
double LASolver(double y0, double t0, double h, LASolverCallback f, void *ptr, size_t solver, void *de);
double LASolverRKGenericDual(double y0, double t0, double dt, LASolverCallback f, double *k, const size_t stages, void *ptr, const double *a, const double *b, const double *c1, double *y1, const double *c2, double *y2);

LAErrorCode LASolverEulerArray(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRK2Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRK2HeunArray(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRK3Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRK4Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRK4_38Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKGenericMatrix(double y0, double t0, double h, LASolverArrayCallback f, size_t n, void *ptr, LAMat_t *a, LAMat_t *b, LAMat_t *c, LAMat_t *k, LAMat_t *r, const size_t stages, double alphaOffset, bool fsal);
LAErrorCode LASolverRK5Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRK6Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRK8Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverAdaptiveArray(const double yn, const double tn, const double h, LASolverArrayCallback f, const size_t n, void *ptr, const double atol, const double tmin, const double tbase, const double sbase, const double sexp, LAMat_t *a, LAMat_t *b, LAMat_t *c, LAMat_t *k, LAMat_t *r, const size_t stages, double *yo, bool fsal);
LAErrorCode LASolverRKF45Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKF78Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverDOP45Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverBS23Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverODE12Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKCK45Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverArray(const double y0, const double t0, const double h, LASolverArrayCallback f, void *ptr, size_t solver, void *de, double *y, const size_t ny);

double LASolverFunctionTranslator(double t, double y, void *ptr, double a);
double LASolverNewtonFunctionAdapter(double x, void *ptr);
double LASolverNewtonFunctionDerivativeAdapter(double x, void *ptr);
LAErrorCode LASolverImplicitArrayBase(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr, LANewtonRaphsonFunc nf, LANewtonRaphsonFunc ndf);

double LASolverImplicitFunctionAdapter(double x, void *ptr);
double LASolverImplicitFunctionDerivativeAdapter(double x, void *ptr);
LAErrorCode LASolverEulerImplicitArray(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);

double LASolverTrapezoidalFunctionAdapter(double x, void *ptr);
double LASolverTrapezoidalFunctionDerivativeAdapter(double x, void *ptr);
LAErrorCode LASolverTrapezoidalArray(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);

double LASolverBDF2FunctionAdapter(double x, void *ptr);
double LASolverBDF2FunctionDerivativeAdapter(double x, void *ptr);
LAErrorCode LASolverBDF2Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);

LAErrorCode LASolverLMSBackendArray(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr, const double *coef, size_t order);
LAErrorCode LASolverLMSAB2Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverLMSAB3Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverLMSAB4Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverLMSAB5Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverLMSAB6Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverLMSAB7Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);

LAErrorCode LASolverRKImplicitArray(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr, const double *a, const double *b, const double *c, const size_t stages);
LAErrorCode LASolverRKIGL4Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKIGL6Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKIR1A3Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKIR1A5Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKIR2A3Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKIR2A5Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKIL3A2Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKIL3A4Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKIL3A6Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKIL3A8Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKIL3B2Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKIL3B4Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKIL3B6Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKIL3B8Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKIL3C2Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKIL3C4Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKIL3C6Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverRKIL3C8Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);

LAErrorCode LASolverImplicitLMSBackendArray(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr, const double *coef, size_t order);
LAErrorCode LASolverILMSAM2Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverILMSAM3Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverILMSAM4Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);
LAErrorCode LASolverILMSAM5Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr);


#endif //LA_NUMERIC_METHODS_SOLVER
