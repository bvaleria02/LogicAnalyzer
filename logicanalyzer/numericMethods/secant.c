#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "secant.h"

double LASecant(LASecantFunc f, double h0, double x0, void *ptr, size_t nmax, double atol, double rtol, double ftol, double eps, double reps, LASecantFlags flags, LASecantStatus *status){
	LA_HANDLE_NULLPTR(f,		0.0);
	LA_HANDLE_NULLPTR(status,	0.0);
	if(nmax == 0){
		(*status) = LA_SECANT_OK;
		return x0;
	}

	double x_n  	= x0;
	double x_n1 	= x0;
	double fValue	= 0.0;						// f(x_n) - f(x_n-1)
	double f1Value	= f(x_n, ptr);				// f(x_n)
	double f2Value	= f(x_n - h0, ptr);			// f(x_n-1)
	double aerr		= 0.0;
	double rerr		= 0.0;
	double ferr		= 0.0;

	bool absFlag = false;
	bool relFlag = false;
	bool fabFlag = false;
	bool earlyStop = false;

	(*status) = LA_SECANT_OK;

	for(size_t n = 0; n < nmax; n++){
		// Stops if f'(x_n) is below eps
		fValue = f1Value - f2Value;
		if((fabs(fValue) <= eps) && (flags & LA_SECANT_AVOID_SINGULARITY)){
			(*status) = LA_SECANT_SINGULARITY;
			break;
		}

		// Actual method: x_n+1 = x_n - (x_n - x_n-1) * f(x_n) / (f(x_n) - f(x_n-1))
		x_n1 = x_n - ((x_n - h0) / fValue) * f1Value;
		printf("x: %lf\n", x_n1);
	
		// Infinity
		if(isinf(x_n1) && (flags & LA_SECANT_CHECK_INF)){
			(*status) = LA_SECANT_SINGULARITY;
			break;
		}

		// NaN
		if(isnan(x_n1) && (flags & LA_SECANT_CHECK_NAN)){
			(*status) = LA_SECANT_SINGULARITY;
			break;
		}

		// Absolute error
		aerr 	= fabs(x_n1 - x_n);
		absFlag = ((aerr <= atol) || !(flags & LA_SECANT_USE_ATOL));

		// Relative error
		rerr    = fabs(aerr / fmax(fabs(x_n1), reps));
		relFlag = ((rerr <= rtol) || !(flags & LA_SECANT_USE_RTOL));

		// Function absolute error
		f2Value = f1Value;
		f1Value = f(x_n1, ptr);
		ferr	= fabs(fValue);
		fabFlag = ((ferr <= ftol) || !(flags & LA_SECANT_USE_FTOL));

		// Copies value so x_n is the value calculated for x_n+1
		h0  = x_n;
		x_n = x_n1;

		// Tolerance break
		if(absFlag && relFlag && fabFlag){
			(*status) = LA_SECANT_OK;
			earlyStop = true;
			break;
		}
	}

	if(((*status) == LA_SECANT_OK) && !(earlyStop)) (*status) = LA_SECANT_NO_CONVERGENCE;
	
	return x_n;
}

double LASecantCompact(LASecantFunc f, double h0, double x0, void *ptr, LASecantData *data){
	LA_HANDLE_NULLPTR(f,	0.0);
	LA_HANDLE_NULLPTR(data,	0.0);
	
	return LASecant(
		f,
		h0,
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

