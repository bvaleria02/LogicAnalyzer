#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "newtonRaphson.h"
#include "../matrix/matrix.h"

double LANewtonRaphson(LANewtonRaphsonFunc f, LANewtonRaphsonFunc df, double x0, void *ptr, size_t nmax, double atol, double rtol, double ftol, double eps, double reps, LANewtonRaphsonFlags flags, LANewtonRaphsonStatus *status){
	LA_HANDLE_NULLPTR(f,		0.0);
	LA_HANDLE_NULLPTR(df,		0.0);
	LA_HANDLE_NULLPTR(status,	0.0);
	if(nmax == 0){
		(*status) = LA_NEWTON_RAPHSON_OK;
		return x0;
	}

	double x_n  	= x0;
	double x_n1 	= x0;
	double fValue	= f(x_n, ptr);
	double dfValue	= 0.0;
	double aerr		= 0.0;
	double rerr		= 0.0;
	double ferr		= 0.0;

	bool absFlag = false;
	bool relFlag = false;
	bool fabFlag = false;
	bool earlyStop = false;

	(*status) = LA_NEWTON_RAPHSON_OK;

	for(size_t n = 0; n < nmax; n++){
		// Stops if f'(x_n) is below eps
		dfValue = df(x_n, ptr);
		if((fabs(dfValue) <= eps) && (flags & LA_NEWTON_RAPHSON_AVOID_SINGULARITY)){
			(*status) = LA_NEWTON_RAPHSON_SINGULARITY;
			break;
		}

		// Actual method: x_n+1 = x_n - f(x_n) / f'(x_n)
		x_n1 = x_n - (fValue / dfValue);
	
		// Infinity
		if(isinf(x_n1) && (flags & LA_NEWTON_RAPHSON_CHECK_INF)){
			(*status) = LA_NEWTON_RAPHSON_SINGULARITY;
			break;
		}

		// NaN
		if(isnan(x_n1) && (flags & LA_NEWTON_RAPHSON_CHECK_NAN)){
			(*status) = LA_NEWTON_RAPHSON_SINGULARITY;
			break;
		}

		// Absolute error
		aerr 	= fabs(x_n1 - x_n);
		absFlag = ((aerr <= atol) || !(flags & LA_NEWTON_RAPHSON_USE_ATOL));

		// Relative error
		rerr    = fabs(aerr / fmax(fabs(x_n1), reps));
		relFlag = ((rerr <= rtol) || !(flags & LA_NEWTON_RAPHSON_USE_RTOL));

		// Function absolute error
		fValue  = f(x_n1, ptr);
		ferr	= fabs(fValue);
		fabFlag = ((ferr <= ftol) || !(flags & LA_NEWTON_RAPHSON_USE_FTOL));

		// Copies value so x_n is the value calculated for x_n+1
		x_n = x_n1;

		// Tolerance break
		if(absFlag && relFlag && fabFlag){
			(*status) = LA_NEWTON_RAPHSON_OK;
			earlyStop = true;
			break;
		}
	}

	if(((*status) == LA_NEWTON_RAPHSON_OK) && !(earlyStop)) (*status) = LA_NEWTON_RAPHSON_NO_CONVERGENCE;
	
	return x_n;
}

double LANewtonRaphsonCompact(LANewtonRaphsonFunc f, LANewtonRaphsonFunc df, double x0, void *ptr, LANewtonRaphsonData *data){
	LA_HANDLE_NULLPTR(f,	0.0);
	LA_HANDLE_NULLPTR(df,	0.0);
	LA_HANDLE_NULLPTR(data,	0.0);
	
	return LANewtonRaphson(
		f,
		df,
		x0,
		ptr,
		data->nmax,
		data->atol,
		data->rtol,
		data->ftol,
		data->eps,
		data->reps,
		data->flags,
		&(data->status)
	);
}

LAErrorCode LANewtonRaphsonMulti(LANewtonRaphsonMultiFunc fc, LANewtonRaphsonMultiJacobianFunc jc, LAMat_t *x0, LAMat_t *xf, LAMat_t *j, LAMat_t *f, void *ptr, const size_t nmax, const double atol, const double rtol, const double ftol, const double eps, const double reps, LANewtonRaphsonFlags flags, LANewtonRaphsonStatus *status){
	LA_HANDLE_NULLPTR(fc,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(jc,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(f,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(j,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(x0,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(xf,		LA_PROPAGATE_ERROR);
	
	if(!(LAMatIsValid(f))) 		return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(j))) 		return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(x0))) 	return LA_ERROR_MATRIX;
	if(!(LAMatIsValid(xf))) 	return LA_ERROR_MATRIX;
	
	if(!(LAMatAreSameDim(f,  x0)))	return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatAreSameDim(x0, xf)))	return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatIsSquare(j)))			return LA_ERROR_NONMATCHING_DIMENSION;

	size_t fw,fh;
	LAErrorCode code = LA_NO_ERROR;
	code = LAMatShape(f, &fh, &fw);
	if(code) return code;

	if(!(LAMatMatchDim(f,  fh,  1)))	return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatMatchDim(x0, fh,  1)))	return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatMatchDim(xf, fh,  1)))	return LA_ERROR_NONMATCHING_DIMENSION;
	if(!(LAMatMatchDim(j,  fh, fh)))	return LA_ERROR_NONMATCHING_DIMENSION;
	
	LAMat_t l,u,p,z,m;
	code = LAMatCreateDynamicFlags(&l, fh, fh, LA_MAT_READ | LA_MAT_WRITE);
	if(code) goto cleanup;
	code = LAMatCreateDynamicFlags(&u, fh, fh, LA_MAT_READ | LA_MAT_WRITE);
	if(code) goto cleanup;
	code = LAMatCreateDynamicFlags(&p, fh, fh, LA_MAT_READ | LA_MAT_WRITE);
	if(code) goto cleanup;
	code = LAMatCreateDynamicFlags(&z, fh, fh, LA_MAT_READ | LA_MAT_WRITE);
	if(code) goto cleanup;
	code = LAMatCreateDynamicFlags(&m, fh, fh, LA_MAT_READ | LA_MAT_WRITE);
	if(code) goto cleanup;

	// X0 = x_n		(Initial vector)
	// XF = x_n+1   (Final vector)

	double normF  	= 0.0;
	double normJ  	= 0.0;
	double normX_n  = 0.0;
	double normX_n1 = 0.0;

	double aerr     = 0.0;
	double rerr     = 0.0;
	bool   absFlag	= false;
	bool   relFlag	= false;
	bool   fabFlag	= false;
	bool earlyStop  = false;

	if(status != NULL) (*status) = LA_NEWTON_RAPHSON_MATRIX_ERROR;

	// Computes the first F(X0)
	code = fc(fh, x0, f, ptr);
	if(code) goto cleanup;

	for(size_t n = 0; n < nmax; n++){
		// Computes the jacobian J(vec(x_n))
		code = jc(fh, x0, j, ptr);
		if(code) goto cleanup;

		// Check if norm(J) < eps
		code = LAMatFrobeniusNorm(j, &normJ);
		if(code) goto cleanup;
		if((fabs(normJ) <= eps) && (flags & LA_NEWTON_RAPHSON_AVOID_SINGULARITY)){
			if(status != NULL) (*status) = LA_NEWTON_RAPHSON_SINGULARITY;
			break;
		}

		// Invert J matrix -> M
		code = LAMatInverseLU(j, &l, &u, &p, &z, &m);
		if(code) goto cleanup;

		// Copies x0 to xf
		code = LAMatCopy(x0, xf);
		if(code) goto cleanup;

		// Multiply: Xn+1 = Xn + (J-1 * F) * -1.0
		//           Xn+1 = Xn - (J-1 * F)
		// Xf = X0 = X_n, then Xf converts to X_n+1
		code = LAMatMulCumFactor(&m, f, xf, -1.0);
		if(code) goto cleanup;

		code = LAMatFrobeniusNormCol(x0, 0, &normX_n);
		if(code) goto cleanup;
		code = LAMatFrobeniusNormCol(xf, 0, &normX_n1);
		if(code) goto cleanup;

		// Absolute error
		aerr = fabs(normX_n - normX_n1);
		absFlag = ((aerr <= atol) || !(flags & LA_NEWTON_RAPHSON_USE_ATOL));

		// Relative error
		rerr = fabs(normX_n - normX_n1) / fmax(fabs(normX_n1), reps);
		relFlag = ((rerr <= rtol) || !(flags & LA_NEWTON_RAPHSON_USE_RTOL));

		// Function error (ferr = normF)
		// Computes F(vec(x_n+1)), reuses in the next iteration
		code = fc(fh, xf, f, ptr);
		if(code) goto cleanup;
		code = LAMatFrobeniusNormCol(f,  0, &normF);
		if(code) goto cleanup;
		fabFlag = ((normF <= ftol) || !(flags & LA_NEWTON_RAPHSON_USE_FTOL));

		// Copies xf to x0
		code = LAMatCopy(xf, x0);
		if(code) goto cleanup;

		// Tolerance break
		if(absFlag && relFlag && fabFlag){
			if(status != NULL) (*status) = LA_NEWTON_RAPHSON_OK;
			earlyStop = true;
			break;
		}
	}

	if(status != NULL){
		if(earlyStop) {
			(*status) = LA_NEWTON_RAPHSON_OK;
		} else if(((*status) != LA_NEWTON_RAPHSON_SINGULARITY) && ((*status) != LA_NEWTON_RAPHSON_MATRIX_ERROR)){
			(*status) = LA_NEWTON_RAPHSON_NO_CONVERGENCE;
		}
	}

	code = LA_NO_ERROR;
	goto cleanup;

cleanup:
	LAMatDestroy(&l);
	LAMatDestroy(&u);
	LAMatDestroy(&p);
	LAMatDestroy(&z);
	LAMatDestroy(&m);
	return code;
}

LAErrorCode LANewtonRaphsonMultiCompact(LANewtonRaphsonMultiFunc fc, LANewtonRaphsonMultiJacobianFunc jc, LAMat_t *x0, LAMat_t *xf, LAMat_t *j, LAMat_t *f, void *ptr, LANewtonRaphsonData *data){
	LA_HANDLE_NULLPTR(fc,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(jc,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(f,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(j,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(x0,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(xf,		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(data,		LA_PROPAGATE_ERROR);

	return LANewtonRaphsonMulti(
		fc, jc, x0, xf, j, f, ptr,
		data->nmax,
		data->atol,
		data->rtol,
		data->ftol,
		data->eps,
		data->reps,
		data->flags,
		&(data->status)
	);
}
