#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <stdbool.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "solver.h"
#include "solverConsts.h"
#include "newtonRaphson.h"
#include "secant.h"

double LASolverEuler(double y0, double t0, double h, LASolverCallback f, void *ptr){
	LA_HANDLE_NULLPTR(f, 0.0);

	double k1 = f(t0, y0, ptr, 0);
	return y0 + h * k1;
}

double LASolverRK2(double y0, double t0, double h, LASolverCallback f, void *ptr){
	LA_HANDLE_NULLPTR(f, 0.0);

	double k1 = f(t0,           y0        	    , ptr, 0);
	double k2 = f(t0 + (h/2.0), y0 +  (h/2.0)*k1, ptr, 0.5);

	return y0 + (k1 + k2) * h/2.0;
}

double LASolverRK3(double y0, double t0, double h, LASolverCallback f, void *ptr){
	LA_HANDLE_NULLPTR(f, 0.0);

	double k1 = f(t0,           y0        	    , ptr, 0);
	double k2 = f(t0 + (h/2.0), y0 +  (h/2.0)*k1, ptr, 0.5);
	double k3 = f(t0 + h,       y0 +        h*k2, ptr, 1);

	return y0 + (k1 + 4*k2 + k3) * h/6.0;
}

double LASolverRK4(double y0, double t0, double h, LASolverCallback f, void *ptr){
	LA_HANDLE_NULLPTR(f, 0.0);

	double k1 = f(t0,         y0           , ptr, 0);
	double k2 = f(t0 + 0.5*h, y0 + 0.5*h*k1, ptr, 0.5);
	double k3 = f(t0 + 0.5*h, y0 + 0.5*h*k2, ptr, 0.5);
	double k4 = f(t0 +     h, y0 +     h*k3, ptr, 1);

	return y0 + (k1 + 2*k2 + 2*k3 + k4) * h / (double) 6.0;
}

double LASolverRK4_38(double y0, double t0, double h, LASolverCallback f, void *ptr){
	LA_HANDLE_NULLPTR(f, 0.0);

	double k1 = f(t0,               y0, 							   ptr, 0);
	double k2 = f(t0 + (1.0/3.0)*h, y0 + (1.0/3.0)*h*k1, 			   ptr, 1.0/3.0);
	double k3 = f(t0 + (2.0/3.0)*h, y0 - (1.0/3.0)*h*k1 + h*k2, 	   ptr, 2.0/3.0);
	double k4 = f(t0 +           h, y0 +           h*k1 - h*k2 + h*k3, ptr, 1);

	return y0 + (k1 + 3*k2 + 3*k3 + k4) * h / (double) 8.0;
}

// Only useful for higher order
double LASolverRKGeneric(double y0, double t0, double dt, LASolverCallback f, void *ptr, const double *a, const double *b, const double *c, double *k, const size_t order){
	LA_HANDLE_NULLPTR(f, 0.0);
	LA_HANDLE_NULLPTR(a, 0.0);
	LA_HANDLE_NULLPTR(b, 0.0);
	LA_HANDLE_NULLPTR(c, 0.0);
	LA_HANDLE_NULLPTR(k, 0.0);
	
	double tn = 0.0;
	double yn = 0.0;
	
	for(size_t i = 0; i < order; i++){
		yn = 0.0;
		tn = b[i];

		for(size_t j = 0; j < i; j++){
			// column, row, column length
			yn += LA_MATRIX_INDEX(j, i, order) * k[j];
		}

		k[i] = f(t0 + dt*tn, y0 + dt*yn, ptr, tn);
	}

	double dy = 0;
	for(size_t i = 0; i < order; i++){
		dy += c[i] * k[i];
	}

	return y0 + dy*dt;
}

double LASolverRK6(double y0, double t0, double h, LASolverCallback f, void *ptr){
	LA_HANDLE_NULLPTR(f, 0.0);

	double k[LA_SOLVER_RK6_STAGES] = {
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
	};

	return LASolverRKGeneric(y0, t0, h, f, ptr, LA_SOLVER_RK6_A_DATA, LA_SOLVER_RK6_B_DATA, LA_SOLVER_RK6_C_DATA, k, LA_SOLVER_RK6_STAGES);
}

double LASolverRK8(double y0, double t0, double h, LASolverCallback f, void *ptr){
	LA_HANDLE_NULLPTR(f, 0.0);

	double k[LA_SOLVER_RK8_STAGES] = {
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
	};

	return LASolverRKGeneric(y0, t0, h, f, ptr, LA_SOLVER_RK8_A_DATA, LA_SOLVER_RK8_B_DATA, LA_SOLVER_RK8_C_DATA, k, LA_SOLVER_RK8_STAGES);
}


double LASolverEulerImplicit(double y0, double t0, double h, LASolverCallback f, void *ptr, void *data){
	LA_HANDLE_NULLPTR(f, 0.0);
	LA_HANDLE_NULLPTR(data, 0.0);

	LASolverEulerImplicitDetails *de = (LASolverEulerImplicitDetails *)data;

	double x_k  = LASolverRK2(y0, t0, h/2.0, f, ptr);
	double x_k1 = x_k;
	double x_kh = y0;
	double err  = 0.0;

	double dx = 0;
	double f1 = 0;
	double f2 = 0;

	for(size_t k = 0; k < de->nmax; k++){
		dx = x_k - x_kh;
		f1 = (x_k - y0 - h*(f(t0, x_k, ptr, 0)));
		f2 = (x_kh - y0 - h*(f(t0, x_kh, ptr, -1)));

		x_k1 = x_k - ((dx * f1) / (f1 - f2));

		err = fabs(x_k1 - x_k);
		if(err <= de->atol){
			break;
		}

		x_kh = x_k;
		x_k = x_k1;
	}

	return x_k1;
}

double LASolverHeun(double y0, double t0, double h, LASolverCallback f, void *ptr){
	LA_HANDLE_NULLPTR(f, 0.0);

	double y_n1 = y0 + h*f(t0, y0, ptr, 0);
	double y1   = y0 + (h/2.0) * (f(t0, y0, ptr, 0) + f(t0+h, y_n1, ptr, 1));

	return y1;
}



double LASolverTrapezoidal(double y0, double t0, double h, LASolverCallback f, void *ptr, void *data){
	LA_HANDLE_NULLPTR(f, 0.0);
	LA_HANDLE_NULLPTR(data, 0.0);

	LASolverTrapezoidalDetails *de = (LASolverTrapezoidalDetails *)data;

	double x_k  = LASolverRK2(y0, t0, h/2.0, f, ptr);
	double x_k1 = x_k;
	double x_kh = y0;
	double err  = 0.0;

	double dx = 0;
	double f1 = 0;
	double f2 = 0;

	for(size_t k = 0; k < de->nmax; k++){
		dx = x_k - x_kh;
		f1 = (x_k - y0 - (h/2.0)*(f(t0, x_k, ptr, 0) + f(t0, x_k1, ptr, 1)));
		f2 = (x_kh - y0 - (h/2.0)*(f(t0, x_kh, ptr, -1) + f(t0, x_k, ptr, 0)));

		x_k1 = x_k - ((dx * f1) / (f1 - f2));

		err = fabs(x_k1 - x_k);
		if(err <= de->atol){
			break;
		}

		x_kh = x_k;
		x_k = x_k1;
	}

	return x_k1;
}

double LASolverBDF2(double y0, double t0, double h, LASolverCallback f, void *ptr, void *data){
	LA_HANDLE_NULLPTR(f, 0.0);
	LA_HANDLE_NULLPTR(data, 0.0);

	LASolverBDF2Details *de = (LASolverBDF2Details *)data;

	double x_k  = LASolverRK2(y0, t0, h/2.0, f, ptr);
	double x_k1 = x_k;
	double x_kh = y0;
	double err  = 0.0;

	double dx = 0;
	double f1 = 0;
	double f2 = 0;
	
	for(size_t k = 0; k < de->nmax; k++){
		dx = x_k - x_kh;
		f1 = x_k  - (4.0/3.0)*y0 + (1.0/3.0)*de->prev[de->pIndex] - (2.0/3.0)*h*f(t0, x_k,  ptr,  0);
		f2 = x_kh - (4.0/3.0)*y0 + (1.0/3.0)*de->prev[de->pIndex] - (2.0/3.0)*h*f(t0-h, x_kh, ptr, -1);

		x_k1 = x_k - ((dx * f1) / (f1 - f2));

		err = fabs(x_k1 - x_k);
		if(err <= de->atol){
			break;
		}

		x_kh = x_k;
		x_k = x_k1;
	}

	de->prev[de->pIndex] = x_k1;
	return x_k1;
}

// Useful for adaptive methods
double LASolverRKGenericDual(double y0, double t0, double dt, LASolverCallback f, double *k, const size_t stages, void *ptr, const double *a, const double *b, const double *c1, double *y1, const double *c2, double *y2){
	LA_HANDLE_NULLPTR(f, 0.0);
	LA_HANDLE_NULLPTR(a, 0.0);
	LA_HANDLE_NULLPTR(b, 0.0);
	LA_HANDLE_NULLPTR(c1, 0.0);
	LA_HANDLE_NULLPTR(c2, 0.0);
	LA_HANDLE_NULLPTR(k, 0.0);
	LA_HANDLE_NULLPTR(y1, 0.0);
	LA_HANDLE_NULLPTR(y2, 0.0);
	
	double tn = 0.0;
	double yn = 0.0;
	
	for(size_t i = 0; i < stages; i++){
		yn = 0.0;
		tn = b[i];

		for(size_t j = 0; j < i; j++){
			// column, row, column length
			yn += LA_MATRIX_INDEX(j, i, stages) * k[j];
		}

		k[i] = f(t0 + dt*tn, y0 + dt*yn, ptr, tn);
	}

	(*y1) = y0;
	(*y2) = y0;
	for(size_t i = 0; i < stages; i++){
		(*y1) = (*y1) + (c1[i] * k[i] * dt);
		(*y2) = (*y2) + (c2[i] * k[i] * dt);
	}

	return fabs((*y2) - (*y1));
}

double LASolverRKF45(double y0, double t0, double h, LASolverCallback f, void *ptr, void *data){
	LA_HANDLE_NULLPTR(f, 0.0);
	LA_HANDLE_NULLPTR(data, 0.0);
	
	LASolverRKFDetails *de = (LASolverRKFDetails *)data;

	const size_t stages = LA_SOLVER_RKF45_STAGES;
	double k[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

	double err = 0;
	double y4 = 0;
	double y5 = 0;
	double tr = 0;
	double ta = 0;
	double t1 = de->subTime;
	double dt = h * t1;
	bool isLastStep = false;
	bool isMinTimeReach = false;
	size_t i = 0;

	while(ta < 1){
		tr = 1 - ta;
		if(tr < t1){
			t1 = tr;
			isLastStep = true;
		}

		dt = h * t1;

		if(isnan(dt) || isnan(y0)){
			break;
		}

		err = LASolverRKGenericDual(y0, t0, dt, f, k, stages, ptr, LA_SOLVER_RKF45_A_DATA, LA_SOLVER_RKF45_B_DATA, LA_SOLVER_RKF45_C_DATA, &y4, &(LA_SOLVER_RKF45_C_DATA[stages]), &y5);
//		printf("i: %lu\ny0: %lf\tt0: %lf\terr: %lf\tdt: %lf\tt1: %lf\tta: %lf\ty4: %lf\ty5: %lf\n", i, y0, t0, err, dt, t1, ta, y4, y5);

		if(((err <= de->atol) && !(isLastStep)) || (isMinTimeReach)){
			t0 += dt;
			ta += t1;
			y0  = y5;
		}

		if(isLastStep) break;

		t1 *= 0.84 * pow((de->atol / err), 0.25);
		if(t1 < de->minTime){
			isMinTimeReach = true;
			t1 = de->minTime;
		} else {
			isMinTimeReach = false;
		}
		i++;
	}

	return y5;
}

double LASolver(double y0, double t0, double h, LASolverCallback f, void *ptr, size_t solver, void *de){
	LA_HANDLE_NULLPTR(f, 0.0);
	
	double y1 = 0;
	switch(solver){
		case LA_SOLVER_EULER:		y1 = LASolverEuler(y0, t0, h, f, ptr);
									break;
		case LA_SOLVER_IMPLICIT:	y1 = LASolverEulerImplicit(y0, t0, h, f, ptr, de);
									break;
		case LA_SOLVER_TRAPEZOIDAL:	y1 = LASolverTrapezoidal(y0, t0, h, f, ptr, de);
									break;
		case LA_SOLVER_BDF2:		y1 = LASolverBDF2(y0, t0, h, f, ptr, de);
									break;
		case LA_SOLVER_RK2:			y1 = LASolverRK2(y0, t0, h, f, ptr);
									break;
		case LA_SOLVER_HEUN:		y1 = LASolverHeun(y0, t0, h, f, ptr);
									break;
		case LA_SOLVER_RK3:			y1 = LASolverRK3(y0, t0, h, f, ptr);
									break;
		case LA_SOLVER_RK4:			y1 = LASolverRK4(y0, t0, h, f, ptr);
									break;
		case LA_SOLVER_RK4_38:		y1 = LASolverRK4_38(y0, t0, h, f, ptr);
									break;
		case LA_SOLVER_RK6:			y1 = LASolverRK6(y0, t0, h, f, ptr);
									break;
		case LA_SOLVER_RK8:			y1 = LASolverRK8(y0, t0, h, f, ptr);
									break;
		case LA_SOLVER_RKF45:		y1 = LASolverRKF45(y0, t0, h, f, ptr, de);
									break;

		// Failback if invalid method is selected
		default:				y1 = LASolverEuler(y0, t0, h, f, ptr);
								break;
	}

	return y1;
}
