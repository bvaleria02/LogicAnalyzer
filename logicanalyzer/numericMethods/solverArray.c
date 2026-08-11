#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "solver.h"
#include "solverConsts.h"
#include "../matrix/matrix.h"

/*
	y0	: initial value
	t0	: initial time
	h 	: delta-time
	y 	: output data
	ny	: output data size
	f 	: dy/dt function
	ptr	: additional data
*/

LAErrorCode LASolverEulerArray(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	double tn = t0;
	double yn = y0;
	double k1 = 0.0;

	for(size_t n = 0; n < ny; n++){
		k1 = f(yn, tn, n, ptr, 0.0);

		yn += h * k1;
		tn += h;
		y[n] = yn;
	}

	return LA_NO_ERROR;
}

LAErrorCode LASolverRK2Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	double tn = t0;
	double yn = y0;
	double k1 = 0.0;
	double k2 = 0.0;

	for(size_t n = 0; n < ny; n++){
		k1 = f(yn,                tn          , n, ptr, 0.0);
		k2 = f(yn + (h/2.0) * k1, tn + (h/2.0), n, ptr, 0.5);

		yn += (h/2.0) * (k1 + k2);
		tn += h;
		y[n] = yn;
	}

	return LA_NO_ERROR;
}

LAErrorCode LASolverRK2HeunArray(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	double tn = t0;
	double yn = y0;
	double k1 = 0.0;
	double k2 = 0.0;

	for(size_t n = 0; n < ny; n++){
		k1 = yn + h*f(yn, tn, n, ptr, 0.0);
		k2 = yn + (h/2.0) * (f(yn, tn, n, ptr, 0) + f(k1, tn+h, n, ptr, 1.0));

		yn = k2;
		tn += h;
		y[n] = yn;
	}

	return LA_NO_ERROR;
}

LAErrorCode LASolverRK3Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	double tn = t0;
	double yn = y0;
	double k1 = 0.0;
	double k2 = 0.0;
	double k3 = 0.0;

	for(size_t n = 0; n < ny; n++){
		k1 = f(yn,                tn          , n, ptr, 0.0);
		k2 = f(yn + (h/2.0) * k1, tn + (h/2.0), n, ptr, 0.5);
		k3 = f(yn +  h      * k2, tn + h,       n, ptr, 1.0);

		yn += (h/6.0) * (k1 + 4*k2 + k3);
		tn += h;
		y[n] = yn;
	}

	return LA_NO_ERROR;
}

LAErrorCode LASolverRK4Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	double tn = t0;
	double yn = y0;
	double k1 = 0.0;
	double k2 = 0.0;
	double k3 = 0.0;
	double k4 = 0.0;

	for(size_t n = 0; n < ny; n++){
		k1 = f(yn,                tn          , n, ptr, 0.0);
		k2 = f(yn + (h/2.0) * k1, tn + (h/2.0), n, ptr, 0.5);
		k3 = f(yn + (h/2.0) * k2, tn + (h/2.0), n, ptr, 0.5);
		k4 = f(yn +  h      * k3, tn + h,       n, ptr, 1.0);

		yn += (h/6.0) * (k1 + 2*k2 + 2*k3 + k4);
		tn += h;
		y[n] = yn;
	}

	return LA_NO_ERROR;
}

LAErrorCode LASolverRK4_38Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	double tn = t0;
	double yn = y0;
	double k1 = 0.0;
	double k2 = 0.0;
	double k3 = 0.0;
	double k4 = 0.0;

	for(size_t n = 0; n < ny; n++){
		k1 = f(yn,                         tn,               n, ptr, 0.0);
		k2 = f(yn + h*(1.0/3.0)*k1, 	   tn + h*(1.0/3.0), n, ptr, 1.0/2.0);
		k3 = f(yn - h*((1.0/3.0)*k1 - k2), tn + h*(1.0/3.0), n, ptr, 2.0/3.0);
		k4 = f(yn + h*(k1 - k2 + k3),      tn + h,           n, ptr, 1.0);

		yn += (h/8.0) * (k1 + 3*k2 + 3*k3 + k4);
		tn += h;
		y[n] = yn;
	}

	return LA_NO_ERROR;
}

// Only useful for higher order
LAErrorCode LASolverRKGenericMatrix(double y0, double t0, double h, LASolverArrayCallback f, size_t n, void *ptr, LAMat_t *a, LAMat_t *b, LAMat_t *c, LAMat_t *k, LAMat_t *r, const size_t stages, double alphaOffset, bool fsal){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(k, LA_PROPAGATE_ERROR);
	
	LAErrorCode code = LA_NO_ERROR;
	double kValue = 0.0;

	if(fsal){
		code = LAMatGet(k, 0, stages - 1, &kValue);
		if(code) return code;
	}

	code = LAMatZeros(k);
	if(code) return code;
	code = LAMatZeros(r);
	if(code) return code;

	double yn = 0.0;
	double aValue = 0.0;
	double bValue = 0.0;
	size_t kInitValue = 0;

	if(fsal){
		code = LAMatSet(k, 0, 0, kValue);
		if(code) return code;

		kInitValue = 1;
	}

	for(size_t y = kInitValue; y < stages; y++){
		yn = 0.0;

		for(size_t x = 0; x < y; x++){
			code = LAMatGet(a, y, x, &aValue);
			if(code) return code;
			code = LAMatGet(k, 0, x, &kValue);
			if(code) return code;
			yn += aValue * kValue;
		}

		code = LAMatGet(b, 0, y, &bValue);
		if(code) return code;

		kValue = f(y0 + h*yn, t0 + h*bValue, n, ptr, alphaOffset + bValue);
		code = LAMatSet(k, 0, y, kValue);
		if(code) return code;
	}

	code = LAMatMul(k, c, r);
	if(code) return code;

	return LA_NO_ERROR;
}

LAErrorCode LASolverRK5Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	const size_t stages = LA_SOLVER_RK5_STAGES;
	double k_data[LA_SOLVER_RK5_STAGES] = {
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0
	};
	double r_data[1] = {0.0};

	LAMat_t a, b, c, k, r;
	LAMatCreateFromArrayFlags(&a, stages, stages, (double *)LA_SOLVER_RK5_A_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&b, 1,      stages, (double *)LA_SOLVER_RK5_B_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&c, stages, 1,      (double *)LA_SOLVER_RK5_C_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&k, 1,      stages, (double *)k_data,               LA_MAT_READ | LA_MAT_WRITE);
	LAMatCreateFromArrayFlags(&r, 1,      1,      (double *)r_data,               LA_MAT_READ | LA_MAT_WRITE);

	double tn = t0;
	double yn = y0;
	double yo = 0.0;

	for(size_t n = 0; n < ny; n++){
		code = LASolverRKGenericMatrix(yn, tn, h, f, n, ptr, &a, &b, &c, &k, &r, stages, 0.0, false);
		if(code) return code;

		code = LAMatGet(&r, 0, 0, &yo);
		if(code) return code;

		yn	 += h * yo;
		tn   += h;
		y[n]  = yn;
	}

	return LA_NO_ERROR;
}

LAErrorCode LASolverRK6Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	const size_t stages = LA_SOLVER_RK6_STAGES;
	double k_data[LA_SOLVER_RK6_STAGES] = {
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
	};
	double r_data[1] = {0.0};

	LAMat_t a, b, c, k, r;
	LAMatCreateFromArrayFlags(&a, stages, stages, (double *)LA_SOLVER_RK6_A_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&b, 1,      stages, (double *)LA_SOLVER_RK6_B_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&c, stages, 1,      (double *)LA_SOLVER_RK6_C_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&k, 1,      stages, (double *)k_data,               LA_MAT_READ | LA_MAT_WRITE);
	LAMatCreateFromArrayFlags(&r, 1,      1,      (double *)r_data,               LA_MAT_READ | LA_MAT_WRITE);

	double tn = t0;
	double yn = y0;
	double yo = 0.0;

	for(size_t n = 0; n < ny; n++){
		code = LASolverRKGenericMatrix(yn, tn, h, f, n, ptr, &a, &b, &c, &k, &r, stages, 0.0, false);
		if(code) return code;

		code = LAMatGet(&r, 0, 0, &yo);
		if(code) return code;

		yn	 += h * yo;
		tn   += h;
		y[n]  = yn;
	}

	return LA_NO_ERROR;
}

LAErrorCode LASolverRK8Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	const size_t stages = LA_SOLVER_RK8_STAGES;
	double k_data[LA_SOLVER_RK8_STAGES] = {
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
	};
	double r_data[1] = {0.0};

	LAMat_t a, b, c, k, r;
	LAMatCreateFromArrayFlags(&a, stages, stages, (double *)LA_SOLVER_RK8_A_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&b, 1,      stages, (double *)LA_SOLVER_RK8_B_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&c, stages, 1,      (double *)LA_SOLVER_RK8_C_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&k, 1,      stages, (double *)k_data,               LA_MAT_READ | LA_MAT_WRITE);
	LAMatCreateFromArrayFlags(&r, 1,      1,      (double *)r_data,               LA_MAT_READ | LA_MAT_WRITE);

	double tn = t0;
	double yn = y0;
	double yo = 0.0;

	for(size_t n = 0; n < ny; n++){
		code = LASolverRKGenericMatrix(yn, tn, h, f, n, ptr, &a, &b, &c, &k, &r, stages, 0.0, false);
		if(code) return code;

		code = LAMatGet(&r, 0, 0, &yo);
		if(code) return code;

		yn	 += h * yo;
		tn   += h;
		y[n]  = yn;
	}

	return LA_NO_ERROR;
}

LAErrorCode LASolverAdaptiveArray(const double yn, const double tn, const double h, LASolverArrayCallback f, const size_t n, void *ptr, const double atol, const double tmin, const double tbase, const double sbase, const double sexp, LAMat_t *a, LAMat_t *b, LAMat_t *c, LAMat_t *k, LAMat_t *r, const size_t stages, double *yo, bool fsal){
	LA_HANDLE_NULLPTR(f,  LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(a,  LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b,  LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c,  LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(k,  LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(r,  LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(yo, LA_PROPAGATE_ERROR);

	// Absolute: 0 - 1
	// Normalized: N*h - (N+1)*h

	LAErrorCode code = LA_NO_ERROR;
	double y4, y5;
	double ya				= yn;		// output value
	double aerr 			= 0.0;		// Absolute error
	double tr 				= 0.0;		// Time remaining (absolute)
	double ta 				= 0.0;		// Time accumulated (absolute)
	double t1 				= tbase;	// Step size (absolute)
	double dt 				= 0.0;		// Time step (normalized)
	bool   isLastStep		= false;	// Avoid step calculation and end it if true
	bool   isMinTimeReach 	= false;	// Avoids making the step shorter if true
	size_t i				= 0;		// Counter
	double kLastValue		= 0.0;		// Value of last k, used in FSAL

	while(ta < 1.0){
		tr = 1 - ta;
		if(tr < t1){						// Remaining time is SHORTER than step size
			t1 = tr;						// Makes the step size equal to the remaining time
			isLastStep = true;				// Avoids step calculation
		}

		dt = h * t1;						// Normalizes step size to h units
		if(isnan(dt) || isnan(yn)) break;	// avoids getting stuck if singularity

		// Calculate y4 and y5 (they are inside r matrix)
		code = LASolverRKGenericMatrix(ya, tn + h*ta, dt, f, n, ptr, a, b, c, k, r, stages, ta, fsal);
		if(code) return code;

		code = LAMatGet(r, 0, 0, &y4);		// Gets dy4 from r matrix
		if(code) return code;
		y4 = ya + dt*y4;					// Calculate y4 from dy4
		code = LAMatGet(r, 0, 1, &y5);		// Gets dy5 from r matrix
		if(code) return code;
		y5 = ya + dt*y5;					// Calculate y5 from dy5
	
		aerr = fabs(y5 - y4);				// Calculate absolute error between 4th and 5th order solution

		//printf("i: %lu\ny0: %lf\tt0: %lf\terr: %lf\tdt: %lf\tt1: %lf\tta: %lf\ty4: %lf\ty5: %lf\n", i, yn, tn, aerr, dt, t1, ta, y4, y5);

		// if error is below atol and always if the step time is reach
		// "Save it" and advance, else, recalculate step size and iterate
		if((aerr <= atol) || (isMinTimeReach)){
			ta += t1;						// Abs time + abs step size
			ya  = y5;						// Use the 5th order solution
			// Stores the last k_n-1 value
			if(fsal){
				code = LAMatGet(k, 0, stages-1, &kLastValue);
				if(code) return code;
			}
		} else {
			if(fsal){
				// Copies the last k_n-1 value into the k values
				code = LAMatSet(k, 0, stages-1, kLastValue);
				if(code) return code;
			}
		}

		if(isLastStep) break;				// No need to keep iteratin, explicit end
		
		// Step size calculation	sbase * pow(aerr / atol, sden);
		t1 *= sbase * pow(atol / aerr, sexp);

		if(t1 < tmin){
			isMinTimeReach = true;			// 
			t1 = tmin;						// clamp t1 to tmin
		} else {
			isMinTimeReach = false;			// t1 > tmin
		}

		i++;
	}

	(*yo) = ya;
	return LA_NO_ERROR;
}

LAErrorCode LASolverRKF45Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	const size_t stages = LA_SOLVER_RKF45_STAGES;
	double k_data[LA_SOLVER_RKF45_STAGES] = {
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0
	};
	double r_data[2] = {0.0, 0.0};

	LAMat_t a, b, c, k, r;
	LAMatCreateFromArrayFlags(&a, stages, stages, (double *)LA_SOLVER_RKF45_A_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&b, 1,      stages, (double *)LA_SOLVER_RKF45_B_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&c, stages, 2,      (double *)LA_SOLVER_RKF45_C_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&k, 1,      stages, (double *)k_data,                 LA_MAT_READ | LA_MAT_WRITE);
	LAMatCreateFromArrayFlags(&r, 1,      2,      (double *)r_data,                 LA_MAT_READ | LA_MAT_WRITE);

	double tn = t0;
	double yn = y0;
	double yo = 0.0;

	const double tbase 	= 0.125;
	const double tmin 	= 0.01;
	const double sbase 	= 0.9;
	const double sexp 	= 0.2;
	const double atol 	= 1e-5;

	for(size_t n = 0; n < ny; n++){
		code = LASolverAdaptiveArray(
			yn, tn, h, f, n, ptr, atol, tmin, tbase, sbase, sexp,
			&a, &b, &c, &k, &r, stages, &yo, false
		);
		if(code) return code;

		tn  += h;
		y[n] = yo;
		yn	 = yo;
	}

	return LA_NO_ERROR;
}

LAErrorCode LASolverRKF78Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	const size_t stages = LA_SOLVER_RKF78_STAGES;
	double k_data[LA_SOLVER_RKF78_STAGES] = {
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
	};
	double r_data[2] = {0.0, 0.0};

	LAMat_t a, b, c, k, r;
	LAMatCreateFromArrayFlags(&a, stages, stages, (double *)LA_SOLVER_RKF78_A_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&b, 1,      stages, (double *)LA_SOLVER_RKF78_B_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&c, stages, 2,      (double *)LA_SOLVER_RKF78_C_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&k, 1,      stages, (double *)k_data,                 LA_MAT_READ | LA_MAT_WRITE);
	LAMatCreateFromArrayFlags(&r, 1,      2,      (double *)r_data,                 LA_MAT_READ | LA_MAT_WRITE);

	double tn = t0;
	double yn = y0;
	double yo = 0.0;

	const double tbase 	= 0.125;
	const double tmin 	= 0.01;
	const double sbase 	= 0.84;
	const double sexp 	= 1.0/7.0;
	const double atol 	= 1e-5;

	for(size_t n = 0; n < ny; n++){
		code = LASolverAdaptiveArray(
			yn, tn, h, f, n, ptr, atol, tmin, tbase, sbase, sexp,
			&a, &b, &c, &k, &r, stages, &yo, false
		);
		if(code) return code;

		tn  += h;
		y[n] = yo;
		yn	 = yo;
	}

	return LA_NO_ERROR;
}

LAErrorCode LASolverDOP45Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	const size_t stages = LA_SOLVER_DOP45_STAGES;
	double k_data[LA_SOLVER_DOP45_STAGES] = {
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
	};
	double r_data[2] = {0.0, 0.0};

	LAMat_t a, b, c, k, r;
	LAMatCreateFromArrayFlags(&a, stages, stages, (double *)LA_SOLVER_DOP45_A_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&b, 1,      stages, (double *)LA_SOLVER_DOP45_B_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&c, stages, 2,      (double *)LA_SOLVER_DOP45_C_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&k, 1,      stages, (double *)k_data,                 LA_MAT_READ | LA_MAT_WRITE);
	LAMatCreateFromArrayFlags(&r, 1,      2,      (double *)r_data,                 LA_MAT_READ | LA_MAT_WRITE);

	double tn = t0;
	double yn = y0;
	double yo = 0.0;

	const double tbase 	= 0.125;
	const double tmin 	= 0.01;
	const double sbase 	= 0.92;
	const double sexp 	= 0.25;
	const double atol 	= 1e-5;

	for(size_t n = 0; n < ny; n++){
		code = LASolverAdaptiveArray(
			yn, tn, h, f, n, ptr, atol, tmin, tbase, sbase, sexp,
			&a, &b, &c, &k, &r, stages, &yo, true
		);
		if(code) return code;

		tn  += h;
		y[n] = yo;
		yn	 = yo;
	}

	return LA_NO_ERROR;
}

LAErrorCode LASolverBS23Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	const size_t stages = LA_SOLVER_BS23_STAGES;
	double k_data[LA_SOLVER_BS23_STAGES] = {
		0.0, 0.0, 0.0, 0.0
	};
	double r_data[2] = {0.0, 0.0};

	LAMat_t a, b, c, k, r;
	LAMatCreateFromArrayFlags(&a, stages, stages, (double *)LA_SOLVER_BS23_A_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&b, 1,      stages, (double *)LA_SOLVER_BS23_B_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&c, stages, 2,      (double *)LA_SOLVER_BS23_C_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&k, 1,      stages, (double *)k_data,                 LA_MAT_READ | LA_MAT_WRITE);
	LAMatCreateFromArrayFlags(&r, 1,      2,      (double *)r_data,                 LA_MAT_READ | LA_MAT_WRITE);

	double tn = t0;
	double yn = y0;
	double yo = 0.0;

	const double tbase 	= 0.125;
	const double tmin 	= 0.01;
	const double sbase 	= 0.87;
	const double sexp 	= 0.225;
	const double atol 	= 1e-5;

	for(size_t n = 0; n < ny; n++){
		code = LASolverAdaptiveArray(
			yn, tn, h, f, n, ptr, atol, tmin, tbase, sbase, sexp,
			&a, &b, &c, &k, &r, stages, &yo, false
		);
		if(code) return code;

		tn  += h;
		y[n] = yo;
		yn	 = yo;
	}

	return LA_NO_ERROR;
}

LAErrorCode LASolverODE12Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	const size_t stages = LA_SOLVER_ODE12_STAGES;
	double k_data[LA_SOLVER_ODE12_STAGES] = {
		0.0, 0.0
	};
	double r_data[2] = {0.0, 0.0};

	LAMat_t a, b, c, k, r;
	LAMatCreateFromArrayFlags(&a, stages, stages, (double *)LA_SOLVER_ODE12_A_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&b, 1,      stages, (double *)LA_SOLVER_ODE12_B_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&c, stages, 2,      (double *)LA_SOLVER_ODE12_C_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&k, 1,      stages, (double *)k_data,                 LA_MAT_READ | LA_MAT_WRITE);
	LAMatCreateFromArrayFlags(&r, 1,      2,      (double *)r_data,                 LA_MAT_READ | LA_MAT_WRITE);

	double tn = t0;
	double yn = y0;
	double yo = 0.0;

	const double tbase 	= 0.125;
	const double tmin 	= 0.01;
	const double sbase 	= 0.7875;
	const double sexp 	= 25.0/90.0;
	const double atol 	= 1e-5;

	for(size_t n = 0; n < ny; n++){
		code = LASolverAdaptiveArray(
			yn, tn, h, f, n, ptr, atol, tmin, tbase, sbase, sexp,
			&a, &b, &c, &k, &r, stages, &yo, false
		);
		if(code) return code;

		tn  += h;
		y[n] = yo;
		yn	 = yo;
	}

	return LA_NO_ERROR;
}

LAErrorCode LASolverRKCK45Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	const size_t stages = LA_SOLVER_RKCK45_STAGES;
	double k_data[LA_SOLVER_RKCK45_STAGES] = {
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0
	};
	double r_data[2] = {0.0, 0.0};

	LAMat_t a, b, c, k, r;
	LAMatCreateFromArrayFlags(&a, stages, stages, (double *)LA_SOLVER_RKCK45_A_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&b, 1,      stages, (double *)LA_SOLVER_RKCK45_B_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&c, stages, 2,      (double *)LA_SOLVER_RKCK45_C_DATA, LA_MAT_READ);
	LAMatCreateFromArrayFlags(&k, 1,      stages, (double *)k_data,                  LA_MAT_READ | LA_MAT_WRITE);
	LAMatCreateFromArrayFlags(&r, 1,      2,      (double *)r_data,                  LA_MAT_READ | LA_MAT_WRITE);

	double tn = t0;
	double yn = y0;
	double yo = 0.0;

	const double tbase 	= 0.125;
	const double tmin 	= 0.01;
	const double sbase 	= 0.9;
	const double sden 	= 0.2;
	const double atol 	= 1e-5;

	for(size_t n = 0; n < ny; n++){
		code = LASolverAdaptiveArray(
			yn, tn, h, f, n, ptr, atol, tmin, tbase, sbase, sden,
			&a, &b, &c, &k, &r, stages, &yo, false
		);
		if(code) return code;

		tn  += h;
		y[n] = yo;
		yn	 = yo;
	}

	return LA_NO_ERROR;
}

double LASolverFunctionTranslator(double t, double y, void *ptr, double a){
	// Converts single-step callback function into array-solver callbacks
	// Single-step callback:
	//    t, y, ptr, alpha
	//
	// Array callback:
	//    y, t, n, ptr, alpha
	//          |
	//          +----- array index

	LASolverFunctionTranslatorData *lsft = (LASolverFunctionTranslatorData *)ptr;
	return lsft->f(y, t, lsft->n, lsft->p2, a);
}

double LASolverNewtonFunctionAdapter(double x, void *ptr){
	LA_HANDLE_NULLPTR(ptr, 0.0);

	LASolverNewtonAdaptorDetails *ad = (LASolverNewtonAdaptorDetails *)ptr;

	double alpha = (x - ad->x1) / (ad->x2 - ad->x1);
	double t     = ad->t1 + alpha * (ad->t2 - ad->t1);

	return ad->f(x, t, ad->n, ad->ptr, alpha);
}

double LASolverNewtonFunctionDerivativeAdapter(double x, void *ptr){
	LA_HANDLE_NULLPTR(ptr, 0.0);

	LASolverNewtonAdaptorDetails *ad = (LASolverNewtonAdaptorDetails *)ptr;

	double x_1 = x + ad->eps;
	double a_1 = (x_1 - ad->x1) / (ad->x2 - ad->x1);
	double t_1 = ad->t1 + (a_1 * (ad->t2 - ad->t1));

	double x_2 = x - ad->eps;
	double a_2 = (x_2 - ad->x1) / (ad->x2 - ad->x1);
	double t_2 = ad->t1 + (a_2 * (ad->t2 - ad->t1));

	double f1 = ad->f(x_1, t_1, ad->n, ad->ptr, a_1);
	double f2 = ad->f(x_2, t_2, ad->n, ad->ptr, a_2);

	return (f1 - f2) / (2.0 * ad->eps);
}

LAErrorCode LASolverImplicitArrayBase(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr, LANewtonRaphsonFunc nf, LANewtonRaphsonFunc ndf){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(nf, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(ndf, LA_PROPAGATE_ERROR);

	LASolverNewtonAdaptorDetails ne;
	ne.ptr = ptr;
	ne.f   = f;
	ne.eps = 1e-5;

	LASolverImplicitAdaptorDetails ad;
	ad.h   = h;
	ad.f   = LASolverNewtonFunctionAdapter;
	ad.df  = LASolverNewtonFunctionDerivativeAdapter;
	ad.ptr = &ne;

	LASolverFunctionTranslatorData ft;
	ft.f   = f;
	ft.p2  = ptr;

	LANewtonRaphsonData ndata;
	ndata.nmax 		= 50;
	ndata.atol 		= 1e-5;
	ndata.rtol 		= 1e-8;
	ndata.ftol 		= 1e-6;
	ndata.eps  		= 1e-10;
	ndata.reps 		= 1e-10;
	ndata.flags		= LA_NEWTON_RAPHSON_USE_ATOL | LA_NEWTON_RAPHSON_USE_RTOL | LA_NEWTON_RAPHSON_USE_FTOL | LA_NEWTON_RAPHSON_CHECK_INF | LA_NEWTON_RAPHSON_CHECK_NAN | LA_NEWTON_RAPHSON_AVOID_SINGULARITY;
	ndata.status 	= 0;

	double yn   = y0;
	double tn   = t0;

	for(size_t n = 0; n < ny; n++){
		ft.n  = n;

		ne.n  = n;
		ne.t1 = tn;
		ne.t2 = tn + h;
		ne.x1 = (n == 0) ? y0 : y[n-1];
		ne.x2 = LASolverRK2(tn, yn, h, LASolverFunctionTranslator, &ft);
		
		ad.y   = (n == 0) ? y0 : y[n-1];
		if(n > 2){
			ad.yn_1 = y[n-2];
			ad.yn_2 = y[n-3];
		} else if (n == 2){
			ad.yn_1 = y[n-2];
			ad.yn_2 = y0;
		}else if (n == 1){
			ad.yn_1 = y0;
			ad.yn_2 = 0.0;
		} else {
			ad.yn_1 = 0.0;
			ad.yn_2 = 0.0;
		}

		yn = LANewtonRaphsonCompact(
				nf, ndf,
				yn, &ad, &ndata
			); 

		tn   += h;
		y[n]  = yn;
	}

	return LA_NO_ERROR;
}

double LASolverImplicitFunctionAdapter(double x, void *ptr){
	LA_HANDLE_NULLPTR(ptr, 0.0);

	LASolverImplicitAdaptorDetails *ad = (LASolverImplicitAdaptorDetails *)ptr;

	return x - ad->y - (ad->h * ad->f(x, ad->ptr));
}

double LASolverImplicitFunctionDerivativeAdapter(double x, void *ptr){
	LA_HANDLE_NULLPTR(ptr, 0.0);

	LASolverImplicitAdaptorDetails *ad = (LASolverImplicitAdaptorDetails *)ptr;

	return 1 - (ad->h * ad->df(x, ad->ptr));
}

LAErrorCode LASolverEulerImplicitArray(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	LAErrorCode code = LASolverImplicitArrayBase(
				y0, t0, h, f, y, ny, ptr,
				LASolverImplicitFunctionAdapter,
				LASolverImplicitFunctionDerivativeAdapter
		);
	return code;
}

double LASolverTrapezoidalFunctionAdapter(double x, void *ptr){
	LA_HANDLE_NULLPTR(ptr, 0.0);

	LASolverImplicitAdaptorDetails *ad = (LASolverImplicitAdaptorDetails *)ptr;

	return x - ad->y - (ad->h / 2.0) * (ad->f(x, ad->ptr) + ad->f(ad->y, ad->ptr));
}

double LASolverTrapezoidalFunctionDerivativeAdapter(double x, void *ptr){
	LA_HANDLE_NULLPTR(ptr, 0.0);

	LASolverImplicitAdaptorDetails *ad = (LASolverImplicitAdaptorDetails *)ptr;

	return 1 - (ad->h / 2.0) * (ad->df(x, ad->ptr));
}

LAErrorCode LASolverTrapezoidalArray(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	LAErrorCode code = LASolverImplicitArrayBase(
				y0, t0, h, f, y, ny, ptr,
				LASolverTrapezoidalFunctionAdapter,
				LASolverTrapezoidalFunctionDerivativeAdapter
		);
	return code;
}

double LASolverBDF2FunctionAdapter(double x, void *ptr){
	LA_HANDLE_NULLPTR(ptr, 0.0);

	LASolverImplicitAdaptorDetails *ad = (LASolverImplicitAdaptorDetails *)ptr;

	return x - (4.0/3.0)*(ad->y) + (1.0/3.0)*(ad->yn_1) - (2.0/3.0) * ad->h * ad->f(x, ad->ptr);
}

double LASolverBDF2FunctionDerivativeAdapter(double x, void *ptr){
	LA_HANDLE_NULLPTR(ptr, 0.0);

	LASolverImplicitAdaptorDetails *ad = (LASolverImplicitAdaptorDetails *)ptr;

	return 1 - (2.0/3.0) * ad->h  * (ad->df(x, ad->ptr));
}

LAErrorCode LASolverBDF2Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	LAErrorCode code = LASolverImplicitArrayBase(
				y0, t0, h, f, y, ny, ptr,
				LASolverBDF2FunctionAdapter,
				LASolverBDF2FunctionDerivativeAdapter
		);
	return code;
}

double LASolverBDF3FunctionAdapter(double x, void *ptr){
	LA_HANDLE_NULLPTR(ptr, 0.0);

	LASolverImplicitAdaptorDetails *ad = (LASolverImplicitAdaptorDetails *)ptr;

	return x - (18.0/11.0)*(ad->y) + (9.0/11.0)*(ad->yn_1) - (2.0/11.0)*(ad->yn_2) - (6.0/11.0) * ad->h * ad->f(x, ad->ptr);
}

double LASolverBDF3FunctionDerivativeAdapter(double x, void *ptr){
	LA_HANDLE_NULLPTR(ptr, 0.0);

	LASolverImplicitAdaptorDetails *ad = (LASolverImplicitAdaptorDetails *)ptr;

	return 1 - (6.0/11.0) * ad->h  * (ad->df(x, ad->ptr));
}

LAErrorCode LASolverBDF3Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	LAErrorCode code = LASolverImplicitArrayBase(
				y0, t0, h, f, y, ny, ptr,
				LASolverBDF3FunctionAdapter,
				LASolverBDF3FunctionDerivativeAdapter
		);
	return code;
}

LAErrorCode LASolverLMSBackendArray(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr, const double *coef, size_t order){
	LA_HANDLE_NULLPTR(f, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(coef, LA_PROPAGATE_ERROR);

	double fMem[LA_SOLVER_LMS_FMEM_SIZE];
	for(size_t k = 0; k < LA_SOLVER_LMS_FMEM_SIZE; k++) fMem[k] = 0.0;

	LASolverFunctionTranslatorData lsft;
	lsft.f = f;
	lsft.p2 = ptr;

	double tn = t0;
	double yn = y0;

	if(order > LA_SOLVER_LMS_FMEM_SIZE) order = LA_SOLVER_LMS_FMEM_SIZE;

	// Kickstart the method
	for(size_t n = 0; n < order; n++){
		lsft.n = n;
		y[n] = LASolverRK4(yn, tn, h, LASolverFunctionTranslator, &lsft);

		if(n == 0){
			fMem[0] = (y[0] - y0) / h;
		} else {
			fMem[n] = (y[n] - y[n-1]) / h;
		}

		yn  = y[n];
		tn += h;
	}

	double fValue = 0.0;
	double acc    = 0.0;

	// The actual method
	for(size_t n = (order - 1); n < ny; n++){

		fValue = f(yn, tn, n, ptr, 0.0);
		fMem[n % LA_SOLVER_LMS_FMEM_SIZE] = fValue;
		acc = 0.0;

		for(size_t k = 0; k < order; k++){
			size_t fMemIndex = (n - k);
			fMemIndex = fMemIndex % LA_SOLVER_LMS_FMEM_SIZE;
			acc += coef[k] * fMem[fMemIndex];
		}

		y[n] = yn + h * acc;
		yn   = y[n];
		tn  += h;
	}

	return LA_NO_ERROR;
}

LAErrorCode LASolverLMSAB2Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	static const double coef[2] = {3.0/2.0, -1.0/2.0};
	size_t order   				= 2;

	return LASolverLMSBackendArray(y0, t0, h, f, y, ny, ptr, coef, order);
}

LAErrorCode LASolverLMSAB3Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	static const double coef[3] = {23.0/12.0, -16.0/12.0, 5.0/12.0};
	size_t order   				= 3;

	return LASolverLMSBackendArray(y0, t0, h, f, y, ny, ptr, coef, order);
}

LAErrorCode LASolverLMSAB4Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	static const double coef[4] = {55.0/24.0, -59.0/24.0, 37.0/24.0, -9.0/24.0};
	size_t order   				= 4;

	return LASolverLMSBackendArray(y0, t0, h, f, y, ny, ptr, coef, order);
}

LAErrorCode LASolverLMSAB5Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	static const double coef[5] = {1901.0/720.0, -2774.0/720.0, 2616.0/720.0, -1274.0/720.0, 251.0/720.0};
	size_t order   				= 5;

	return LASolverLMSBackendArray(y0, t0, h, f, y, ny, ptr, coef, order);
}

LAErrorCode LASolverLMSAB6Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	static const double coef[6] = {19087.0/60480.0, -27127.0/30240.0, 26729.0/30240.0, -24642.0/302400.0, 17837.0/30240.0, -5257.0/60480.0};
	size_t order   				= 6;

	return LASolverLMSBackendArray(y0, t0, h, f, y, ny, ptr, coef, order);
}

LAErrorCode LASolverLMSAB7Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	static const double coef[7] = {328573.0/95256.0, -2366603.0/317520.0, 3692875.0/317520.0, -3605377.0/317520.0, 2133221.0/317520.0, -706045.0/317520.0, 100152.0/317520.0};
	size_t order   				= 7;

	return LASolverLMSBackendArray(y0, t0, h, f, y, ny, ptr, coef, order);
}

LAErrorCode LAImplicitRKFunc(size_t stages, LAMat_t *k0, LAMat_t *kf, void *ptr){
	LA_HANDLE_NULLPTR(k0, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(kf, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(ptr, 	LA_PROPAGATE_ERROR);

	LASolverImplicitRKData *data = (LASolverImplicitRKData *)ptr;

	double tn 	= data->tn;
	double yn 	= data->yn;
	LAMat_t *a 	= data->a;
	LAMat_t *b 	= data->b;
	double h    = data->h;
	
	double b_i  = 0.0;
	double k_i  = 0.0;
	double k_j  = 0.0;
	double a_ij = 0.0;
	double acc  = 0.0;

	LAErrorCode code = LA_NO_ERROR;

	for(size_t i = 0; i < stages; i++){
		
		acc = 0.0;
		for(size_t j = 0; j < stages; j++){
			code = LAMatGet(a, i, j, &a_ij);
			if(code) return code;
			code = LAMatGet(k0, j, 0, &k_j);
			if(code) return code;

			acc += (k_j * a_ij);
		}

		code = LAMatGet(b, i, 0, &b_i);
		if(code) return code;
		code = LAMatGet(k0, i, 0, &k_i);
		if(code) return code;

		// k_n+1 = k_n - f(y_n*, t_n*)
		k_i = k_i - data->f(yn + h*acc, tn + h*b_i, data->n, data->fptr, b_i);

		code = LAMatSet(kf, i, 0, k_i);
		if(code) return code;
	}
	
	return LA_NO_ERROR;
}

LAErrorCode LAImplicitRKJacobian(size_t stages, LAMat_t *k0, LAMat_t *j, void *ptr){
	LA_HANDLE_NULLPTR(k0, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(j, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(ptr, 	LA_PROPAGATE_ERROR);

	LASolverImplicitRKData *data = (LASolverImplicitRKData *)ptr;

	double tn 	= data->tn;
	double yn 	= data->yn;
	LAMat_t *a 	= data->a;
	LAMat_t *b 	= data->b;
	double h    = data->h;
	
	LAErrorCode code = LA_NO_ERROR;

	double b_i = 0.0;
	double k_m = 0.0;
	double k_l = 0.0;
	double a_im = 0.0;
	double a_il = 0.0;
	double j_il = 0.0;
	double acc  = 0.0;
	double kv1  = 0.0;
	double kv2  = 0.0;

	for(size_t i = 0; i < stages; i++){

		code = LAMatGet(b, i, 0, &b_i);
		if(code) return code;

		acc = 0.0;
		for(size_t m = 0; m < stages; m++){
			code = LAMatGet(k0, m, 0, &k_m);
			if(code) return code;
			code = LAMatGet(a,  i, m, &a_im);
			if(code) return code;
			acc += (k_m * a_im);
		}

		for(size_t l = 0; l < stages; l++){
			code = LAMatGet(a,  i, l, &a_il);
			if(code) return code;
			code = LAMatGet(k0, l, 0, &k_l);
			if(code) return code;
			
			kv1 = data->f(yn + h*(acc + (k_l * data->eps)), tn + h*b_i, data->n, data->fptr, b_i);
			kv2 = data->f(yn + h*(acc - (k_l * data->eps)), tn + h*b_i, data->n, data->fptr, b_i);

			// Kronecker delta
			j_il  = (i == l) ? 1.0 : 0.0;
			j_il -= h * a_il * ((kv1 - kv2) / (h * 2 * fmax(k_l, data->reps) * data->eps));

			code = LAMatSet(j, i, l, j_il);
			if(code) return code;
		}
	}
	
	return LA_NO_ERROR;
}

LAErrorCode LASolverRKImplicitArray(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr, const double *a, const double *b, const double *c, const size_t stages){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(a, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(b, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(c, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	LAMat_t fc, x0, xf, jc;
	code = LAMatCreateDynamicFlags(&fc, stages, 1, 		LA_MAT_READ | LA_MAT_WRITE);
	if(code) goto cleanup;
	code = LAMatCreateDynamicFlags(&x0, stages, 1, 		LA_MAT_READ | LA_MAT_WRITE);
	if(code) goto cleanup;
	code = LAMatCreateDynamicFlags(&xf, stages, 1,		LA_MAT_READ | LA_MAT_WRITE);
	if(code) goto cleanup;
	code = LAMatCreateDynamicFlags(&jc, stages, stages, LA_MAT_READ | LA_MAT_WRITE);
	if(code) goto cleanup;

	code = LAMatZeros(&x0);
	if(code) goto cleanup;

	LANewtonRaphsonData ndata;
	ndata.nmax 		= 50;
	ndata.atol 		= 1e-5;
	ndata.rtol 		= 1e-8;
	ndata.ftol 		= 1e-5;
	ndata.eps  		= 1e-10;
	ndata.reps 		= 1e-8;
	ndata.flags		= LA_NEWTON_RAPHSON_USE_ATOL | LA_NEWTON_RAPHSON_USE_RTOL | LA_NEWTON_RAPHSON_USE_FTOL | LA_NEWTON_RAPHSON_CHECK_INF | LA_NEWTON_RAPHSON_CHECK_NAN /*| LA_NEWTON_RAPHSON_AVOID_SINGULARITY*/;
	ndata.status 	= 0;

	LAMat_t ma, mb;
	code = LAMatCreateFromArrayFlags(&ma, stages, stages, (double *)a, LA_MAT_READ);
	if(code) goto cleanup;
	code = LAMatCreateFromArrayFlags(&mb, stages, 1,      (double *)b, LA_MAT_READ);
	if(code) goto cleanup;

	LASolverImplicitRKData rkdata;
	rkdata.a 	= &ma; 
	rkdata.b 	= &mb; 
	rkdata.f 	= f;
	rkdata.fptr = ptr;
	rkdata.h    = h;
	rkdata.eps  = 1e-4;
	rkdata.reps = 1e-15;

	double xf_k = 0.0;
	double yn   = y0;
	double tn   = t0;
	double acc	= 0.0;

	for(size_t n = 0; n < ny; n++){
		rkdata.n 	= n;
		rkdata.yn 	= yn;
		rkdata.tn 	= tn;

		code = LAMatRand(&x0);
		if(code) goto cleanup;

		code = LANewtonRaphsonMultiCompact(
					LAImplicitRKFunc,
					LAImplicitRKJacobian,
					&x0, &xf, &jc, &fc, &rkdata, &ndata
				);

		if(code) goto cleanup;

		acc = 0.0;
		for(size_t k = 0; k < stages; k++){
			code = LAMatGet(&xf, k, 0, &xf_k);
			if(code) goto cleanup;
			acc += xf_k * c[k];
		}

		yn  = yn + h * acc;
		tn += h;
		y[n] = yn;
	}

	goto cleanup;

cleanup:
	LAMatDestroy(&fc);
	LAMatDestroy(&jc);
	LAMatDestroy(&x0);
	LAMatDestroy(&xf);
	return code;
}

LAErrorCode LASolverRKIGL4Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_GL4_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_GL4_A_DATA, LA_SOLVER_GL4_B_DATA, LA_SOLVER_GL4_C_DATA, stages);
	return code;
}

LAErrorCode LASolverRKIGL6Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_GL6_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_GL6_A_DATA, LA_SOLVER_GL6_B_DATA, LA_SOLVER_GL6_C_DATA, stages);
	return code;
}

LAErrorCode LASolverRKIR1A3Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_R1A3_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_R1A3_A_DATA, LA_SOLVER_R1A3_B_DATA, LA_SOLVER_R1A3_C_DATA, stages);
	return code;
}

LAErrorCode LASolverRKIR1A5Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_R1A5_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_R1A5_A_DATA, LA_SOLVER_R1A5_B_DATA, LA_SOLVER_R1A5_C_DATA, stages);
	return code;
}

LAErrorCode LASolverRKIR2A3Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_R2A3_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_R2A3_A_DATA, LA_SOLVER_R2A3_B_DATA, LA_SOLVER_R2A3_C_DATA, stages);
	return code;
}

LAErrorCode LASolverRKIR2A5Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_R2A5_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_R2A5_A_DATA, LA_SOLVER_R2A5_B_DATA, LA_SOLVER_R2A5_C_DATA, stages);
	return code;
}

LAErrorCode LASolverRKIL3A2Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_L3A2_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_L3A2_A_DATA, LA_SOLVER_L3A2_B_DATA, LA_SOLVER_L3A2_C_DATA, stages);
	return code;
}

LAErrorCode LASolverRKIL3A4Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_L3A4_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_L3A4_A_DATA, LA_SOLVER_L3A4_B_DATA, LA_SOLVER_L3A4_C_DATA, stages);
	return code;
}

LAErrorCode LASolverRKIL3A6Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_L3A6_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_L3A6_A_DATA, LA_SOLVER_L3A6_B_DATA, LA_SOLVER_L3A6_C_DATA, stages);
	return code;
}

LAErrorCode LASolverRKIL3A8Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_L3A8_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_L3A8_A_DATA, LA_SOLVER_L3A8_B_DATA, LA_SOLVER_L3A8_C_DATA, stages);
	return code;
}

LAErrorCode LASolverRKIL3B2Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_L3B2_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_L3B2_A_DATA, LA_SOLVER_L3B2_B_DATA, LA_SOLVER_L3B2_C_DATA, stages);
	return code;
}

LAErrorCode LASolverRKIL3B4Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_L3B4_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_L3B4_A_DATA, LA_SOLVER_L3B4_B_DATA, LA_SOLVER_L3B4_C_DATA, stages);
	return code;
}

LAErrorCode LASolverRKIL3B6Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_L3B6_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_L3B6_A_DATA, LA_SOLVER_L3B6_B_DATA, LA_SOLVER_L3B6_C_DATA, stages);
	return code;
}

LAErrorCode LASolverRKIL3B8Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_L3B8_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_L3B8_A_DATA, LA_SOLVER_L3B8_B_DATA, LA_SOLVER_L3B8_C_DATA, stages);
	return code;
}

LAErrorCode LASolverRKIL3C2Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_L3C2_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_L3C2_A_DATA, LA_SOLVER_L3C2_B_DATA, LA_SOLVER_L3C2_C_DATA, stages);
	return code;
}

LAErrorCode LASolverRKIL3C4Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_L3C4_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_L3C4_A_DATA, LA_SOLVER_L3C4_B_DATA, LA_SOLVER_L3C4_C_DATA, stages);
	return code;
}

LAErrorCode LASolverRKIL3C6Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_L3C6_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_L3C6_A_DATA, LA_SOLVER_L3C6_B_DATA, LA_SOLVER_L3C6_C_DATA, stages);
	return code;
}

LAErrorCode LASolverRKIL3C8Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	const size_t stages = LA_SOLVER_L3C8_STAGES;

	LAErrorCode code = LASolverRKImplicitArray(y0, t0, h, f, y, ny, ptr, LA_SOLVER_L3C8_A_DATA, LA_SOLVER_L3C8_B_DATA, LA_SOLVER_L3C8_C_DATA, stages);
	return code;
}

double LASolverImplicitLMSFuncNewton(double x, void *ptr){
	LA_HANDLE_NULLPTR(ptr, 0.0);

	LASolverImplicitAdaptorDetails *data = (LASolverImplicitAdaptorDetails *)ptr;

	return x - data->y - data->fv - (data->c0 * data->h * data->f(x, data->ptr));
}

double LASolverImplicitLMSDerivativeFuncNewton(double x, void *ptr){
	LA_HANDLE_NULLPTR(ptr, 0.0);

	LASolverImplicitAdaptorDetails *data = (LASolverImplicitAdaptorDetails *)ptr;

	return 1 - (data->c0 * data->h * data->df(x, data->ptr));
}

LAErrorCode LASolverImplicitLMSBackendArray(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr, const double *coef, size_t order){
	LA_HANDLE_NULLPTR(f, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(coef, LA_PROPAGATE_ERROR);

	double fMem[LA_SOLVER_LMS_FMEM_SIZE];
	for(size_t k = 0; k < LA_SOLVER_LMS_FMEM_SIZE; k++) fMem[k] = 0.0;

	LASolverNewtonAdaptorDetails ne;
	ne.ptr = ptr;
	ne.f   = f;
	ne.eps = 1e-5;

	LASolverImplicitAdaptorDetails ad;
	ad.h  	= h;
	ad.f  	= LASolverNewtonFunctionAdapter;
	ad.df 	= LASolverNewtonFunctionDerivativeAdapter;
	ad.ptr 	= &ne;

	LASolverFunctionTranslatorData lsft;
	lsft.f = f;
	lsft.p2 = ptr;

	LANewtonRaphsonData ndata;
	ndata.nmax 		= 50;
	ndata.atol 		= 1e-5;
	ndata.rtol 		= 1e-8;
	ndata.ftol 		= 1e-6;
	ndata.eps  		= 1e-10;
	ndata.reps 		= 1e-10;
	ndata.flags		= LA_NEWTON_RAPHSON_USE_ATOL | LA_NEWTON_RAPHSON_USE_RTOL | LA_NEWTON_RAPHSON_USE_FTOL | LA_NEWTON_RAPHSON_CHECK_INF | LA_NEWTON_RAPHSON_CHECK_NAN | LA_NEWTON_RAPHSON_AVOID_SINGULARITY;
	ndata.status 	= 0;

	double tn = t0;
	double yn = y0;

	if(order > LA_SOLVER_LMS_FMEM_SIZE) order = LA_SOLVER_LMS_FMEM_SIZE;

	// fv = 0.0 and c0 = 1.0 makes this actually the implicit euler method
	ad.fv = 0.0;
	ad.c0 = 1.0;

	// Kickstart the method
	for(size_t n = 0; n < order; n++){
		lsft.n = n;

		// Predictor
		ne.n  = n;
		ne.t1 = tn;
		ne.t2 = tn + h;
		ne.x1 = (n == 0) ? y0 : y[n-1];
		ne.x2 = LASolverRK4(tn, yn, h, LASolverFunctionTranslator, &lsft);
		ad.y  = yn;

		// y_n+1 = y_n + f(t_n+1, y_n+1)
		// x     = y_n+1
		// x_k+1 = x_k - (x_k - y_n  - f(t_n+1, x_k)) / (1 - f'(t_n+1, x_k))

		// Corrector
		y[n] = LANewtonRaphsonCompact(
					LASolverImplicitLMSFuncNewton,
					LASolverImplicitLMSDerivativeFuncNewton,
					yn, &ad, &ndata
				);

		if(n == 0){
			fMem[0] = (y[0] - y0) / h;
		} else {
			fMem[n] = (y[n] - y[n-1]) / h;
			//fMem[n] = f(y[n], ne.t2, n, ptr, 1.0);
		}

		yn  = y[n];
		tn += h;
	}

	double fValue = 0.0;
	double y_newton = 0.0;
	ad.c0   = coef[0];

	// The actual method
	for(size_t n = order; n < ny; n++){
		lsft.n = n;

		// Predictor
		ne.n  = n;
		ne.t1 = tn;
		ne.t2 = tn + h;
		ne.x1 = yn;
		ne.x2 = LASolverRK4(tn, yn, h, LASolverFunctionTranslator, &lsft);
		ad.y  = yn;

		ad.fv = 0.0;
		for(size_t k = 1; k < order; k++){
			size_t fMemIndex = (n - k);
			fMemIndex = fMemIndex % LA_SOLVER_LMS_FMEM_SIZE;
			ad.fv += coef[k] * fMem[fMemIndex];
		}
		ad.fv *= h;

		// Corrector
		y_newton = LANewtonRaphsonCompact(
					LASolverImplicitLMSFuncNewton,
					LASolverImplicitLMSDerivativeFuncNewton,
					yn, &ad, &ndata
				);
		
		fValue = (y_newton - y[n-1] - ad.fv) / (coef[0] * h);
		fMem[n % LA_SOLVER_LMS_FMEM_SIZE] = fValue;

		y[n] = y_newton;
		yn   = y[n];
		tn  += h;
	}

	return LA_NO_ERROR;
}

LAErrorCode LASolverILMSAM2Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	static const double coef[2] = {1.0/2.0, 1.0/2.0};
	size_t order   				= 2;

	return LASolverImplicitLMSBackendArray(y0, t0, h, f, y, ny, ptr, coef, order);
}

LAErrorCode LASolverILMSAM3Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	static const double coef[3] = {5.0/12.0, 2.0/3.0, -1.0/12.0};
	size_t order   				= 3;

	return LASolverImplicitLMSBackendArray(y0, t0, h, f, y, ny, ptr, coef, order);
}

LAErrorCode LASolverILMSAM4Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	static const double coef[4] = {3.0/8.0, 19.0/24.0, -5.0/24.0, 1.0/24.0};
	size_t order   				= 4;

	return LASolverImplicitLMSBackendArray(y0, t0, h, f, y, ny, ptr, coef, order);
}

LAErrorCode LASolverILMSAM5Array(const double y0, const double t0, const double h, LASolverArrayCallback f, double *y, const size_t ny, void *ptr){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);

	static const double coef[5] = {251.0/720.0, 646.0/720.0, -264.0/720.0, 106.0/720.0, -19.0/720.0};
	size_t order   				= 5;

	return LASolverImplicitLMSBackendArray(y0, t0, h, f, y, ny, ptr, coef, order);
}

LAErrorCode LASolverArray(double y0, double t0, double h, LASolverArrayCallback f, void *ptr, size_t solver, void *de, double *y, const size_t ny){
	LA_HANDLE_NULLPTR(f, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, LA_PROPAGATE_ERROR);
	
	LAErrorCode code = LA_NO_ERROR;
	switch(solver){
		case LA_SOLVER_EULER:		code = LASolverEulerArray(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_IMPLICIT:	code = LASolverEulerImplicitArray(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_TRAPEZOIDAL:	code = LASolverTrapezoidalArray(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_BDF2:		code = LASolverBDF2Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_BDF3:		code = LASolverBDF3Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RK2:			code = LASolverRK2Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_HEUN:		code = LASolverRK2HeunArray(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RK3:			code = LASolverRK3Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RK4:			code = LASolverRK4Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RK4_38:		code = LASolverRK4_38Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RK5:			code = LASolverRK5Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RK6:			code = LASolverRK6Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RK8:			code = LASolverRK8Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_ODE12:		code = LASolverODE12Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_BS23:		code = LASolverBS23Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKF45:		code = LASolverRKF45Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKF78:		code = LASolverRKF78Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_DOP45:		code = LASolverDOP45Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKCK45:		code = LASolverRKCK45Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_LMS_AB2:		code = LASolverLMSAB2Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_LMS_AB3:		code = LASolverLMSAB3Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_LMS_AB4:		code = LASolverLMSAB4Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_LMS_AB5:		code = LASolverLMSAB5Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_LMS_AB6:		code = LASolverLMSAB6Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_LMS_AB7:		code = LASolverLMSAB7Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_GL_4:	code = LASolverRKIGL4Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_GL_6:	code = LASolverRKIGL6Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_R1A3:	code = LASolverRKIR1A3Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_R1A5:	code = LASolverRKIR1A5Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_R2A3:	code = LASolverRKIR2A3Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_R2A5:	code = LASolverRKIR2A5Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_L3A2:	code = LASolverRKIL3A2Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_L3A4:	code = LASolverRKIL3A4Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_L3A6:	code = LASolverRKIL3A6Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_L3A8:	code = LASolverRKIL3A8Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_L3B2:	code = LASolverRKIL3B2Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_L3B4:	code = LASolverRKIL3B4Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_L3B6:	code = LASolverRKIL3B6Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_L3B8:	code = LASolverRKIL3B8Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_L3C2:	code = LASolverRKIL3C2Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_L3C4:	code = LASolverRKIL3C4Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_L3C6:	code = LASolverRKIL3C6Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_RKI_L3C8:	code = LASolverRKIL3C8Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_ILMS_AM2:	code = LASolverILMSAM2Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_ILMS_AM3:	code = LASolverILMSAM3Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_ILMS_AM4:	code = LASolverILMSAM4Array(y0, t0, h, f, y, ny, ptr);
									break;
		case LA_SOLVER_ILMS_AM5:	code = LASolverILMSAM5Array(y0, t0, h, f, y, ny, ptr);
									break;

		// Failback if invalid method is selected
		default:					code = LASolverEulerArray(y0, t0, h, f, y, ny, ptr);
									break;
	}

	(void) de;
	return code;
}
