#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "../enums.h"
#include "../types.h"
#include "../filter/windows.h"
#include "../filter/filter.h"
#include "../filterwindow.h"
#include "circuitsim.h"
#include "../numericMethods/solver.h"
#include "../numericMethods/ss.h"
#include "../numericMethods/derivative.h"
#include "../numericMethods/operations.h"

#define LA_CIRCUITSIM_RKF_SUBTIME 0.05
#define LA_CIRCUITSIM_RKF_MINTIME 0.002

double LASolverRCFunc(double t, double vc, void *ptr, double blend){
	LA_HANDLE_NULLPTR(ptr, 0.0);
	(void)t;

	LASolverParameterRC *params = ptr;

	int64_t index = params->i + floor(blend);
	double alpha = blend - floor(blend);
	
	size_t index1Vs = (index <= 0) ? 0 : index;
	index1Vs = (index1Vs >= params->vsSize) ? (params->vsSize - 1) : index1Vs;
	size_t index2Vs = (index <= 0) ? 0 : index + 1;
	index2Vs = (index2Vs >= params->vsSize) ? (params->vsSize - 1) : index2Vs;

	double vs = params->vs[index1Vs] * (1 - alpha) + (params->vs[index2Vs] * alpha);
	vs *= params->inputGain;

	return (vs - vc) / (double) params->rc;
}

double LASolverRCFuncArray(double vc, double t, size_t n, void *ptr, double blend){
	LA_HANDLE_NULLPTR(ptr, 0.0);
	(void)t;

	LASolverParameterRC *params = ptr;

	int64_t index = n + floor(blend);
	double alpha = blend - floor(blend);
	
	size_t index1Vs = (index <= 0) ? 0 : index;
	index1Vs = (index1Vs >= params->vsSize) ? (params->vsSize - 1) : index1Vs;
	size_t index2Vs = (index <= 0) ? 0 : index + 1;
	index2Vs = (index2Vs >= params->vsSize) ? (params->vsSize - 1) : index2Vs;

	double vs = params->vs[index1Vs] * (1 - alpha) + (params->vs[index2Vs] * alpha);
	vs *= params->inputGain;

	return (vs - vc) / (double) params->rc;
}

double LASolverRCFuncDf(double t, double vc, void *ptr, double blend){
	LA_HANDLE_NULLPTR(ptr, 0.0);
	(void)t;
	(void)blend;

	LASolverParameterRC *params = ptr;
	double dVs = params->vs[1] - params->vs[0];
	double dVc = vc - params->vc0;

	return (dVs - dVc) / (double) (params->rc);
}

LAErrorCode LACircuitSimRCLP(double *x, size_t nx, double *y, size_t ny, double r, double c, double v0, double sampleRate, size_t solver, double inputGain, double outputGain){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	LASolverParameterRC params;
	params.rc = r * c;
	double h = 1 / (double) (sampleRate);

	params.vs = x;
	params.vsSize = nx;
	params.inputGain = inputGain;

	code = LASolverArray(v0, 0.0, h, LASolverRCFuncArray, &params, solver, NULL, y, ny);

	(void) outputGain;
	return code;
}

LAErrorCode LACircuitSimRCHP(double *x, size_t nx, double *y, size_t ny, double r, double c, double v0, double sampleRate, size_t solver, double inputGain, double outputGain){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	LASolverParameterRC params;
	params.rc = r * c;
	double h = 1 / (double) (sampleRate);

	params.vs = x;
	params.vsSize = nx;
	params.inputGain = inputGain;

	code = LASolverArray(v0, 0.0, h, LASolverRCFuncArray, &params, solver, NULL, y, ny);
	if(code) return code;

	code = LADerivateArray(y, ny, h, v0);
	if(code) return code;

	code = LAMulArray(y, ny, outputGain / r);
	if(code) return code;

	return code;
}

double LASolverSKFunc(double t, double vc, void *ptr, double blend){
	LA_HANDLE_NULLPTR(ptr, 0.0);
	(void)t;
	(void)vc;
	(void)index;

	LASolverParameterSK *params = ptr;
	double vs = (1 - blend) * params->vs[0] + (blend) * params->vs[1];

	double u = 0;
	if(params->index == 1){
		u = (params->b3 / params->a3) * vs;
	}

	double dx = params->dx[params->index] + u;
	return dx;
}

LAErrorCode LASSCallbackSK(double *dx, double *x, size_t s, void *p){
	LA_HANDLE_NULLPTR(dx, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(p, 		LA_PROPAGATE_ERROR);

	LASolverParameterSK *params = p;

	for(size_t i = 0; i < s; i++){
		params->dx[i] = dx[i];
	}

	return LA_NO_ERROR;
}

LAErrorCode LASSCallbackIndexSK(size_t i, void *p, void *d){
	LA_HANDLE_NULLPTR(p, 		LA_PROPAGATE_ERROR);

	LASolverParameterSK *params = p;
	params->index = i;

	LASolverEulerImplicitDetails *de = d;
	de->pIndex = i;

	return LA_NO_ERROR;
}

double LASolverSKFunc2(double t, double vc, void *ptr, double blend){
	LA_HANDLE_NULLPTR(ptr, 0.0);
	(void)t;
	(void)vc;
	(void)index;
	(void) blend;

	LASolverParameterSK *params = ptr;

	int64_t index = params->i + floor(blend);
	double alpha = blend - floor(blend);
	
	size_t index1Vs = (index <= 0) ? 0 : index;
	index1Vs = (index1Vs >= params->vsSize) ? (params->vsSize - 1) : index1Vs;
	size_t index2Vs = (index <= 0) ? 0 : index + 1;
	index2Vs = (index2Vs >= params->vsSize) ? (params->vsSize - 1) : index2Vs;
//	printf("%li\t1: %li\t2: %li\talpha: %lf\n", params->i, index1Vs, index2Vs, alpha);

	double vs = params->vs[index1Vs] * (1 - alpha) + (params->vs[index2Vs] * alpha);
	vs *= params->inputGain;

	double u = 0;
	if(params->index == 1){
		u = (params->b3 / params->a3) * vs;
	}

	double dx = params->dx[params->index] + u;
	return dx;
}

LAErrorCode LACircuitSimSKLP(double *x, size_t nx, double *y, size_t ny, double r1, double r2, double c1, double c2, double v0c1, double v0c2, double rl, double rh, double slewRate, double sampleRate, size_t solver, double inputGain, double outputGain){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	LASolverParameterSK params;

	double H  = rl / (rl + rh);
/*
	double a1 = H;
	double a2 = r1*c1 - H*(r1*c1 - r1*c2 - r2*c2);
	double a3 = r1*r2*c1*c2*H;
*/

	double a1 = H*(1 + 2*r1/r2);
	double a2 = r1*c1*(1+H) - H*c2*(r1+r2);
	double a3 = r1*r2*c1*c2*H;

	const size_t s = 2;
	double a[4] 	= {  0.0,				 1.0,
						-a1 / a3,		-a2 / a3};
	double b[4] 	= {	0.0,				0.0,
					   	0.0,				0.0};
	double c[4] 	= { 1.0, 0.0,
						0.0, 0.0};
	double d[4] 	= {	0.0,				0.0,
						0.0,				0.0};

	double dx[2]	= {0.0, 0.0};
	double u[2]		= {0.0, 0.0};
	double xs[2]	= {v0c1, v0c2};
	double ys[2]	= {0.0, 0.0};

	double h 		= 1 / (double) (sampleRate);
	double t0		= 0;

	params.a3 			= a3;
	params.b3 			= 1.0;
	params.slewRate		= slewRate / h;

	double yn1			= 0;
	double dy			= 0;

	LASolverEulerImplicitDetails de;
	de.nmax = 50;
	de.atol = 1e-10;
	de.df   = LASolverRCFuncDf;
	double prev[2] = {0.0, 0.0};
	de.prev = prev;
	de.pIndex = 0;
	de.subTime = LA_CIRCUITSIM_RKF_SUBTIME;
	de.minTime = LA_CIRCUITSIM_RKF_MINTIME;

	params.vs = x;
	params.vsSize = nx;
	params.y = y;
	params.ySize = ny;
	params.inputGain = inputGain;

	for(size_t i = 0; i < nx; i++){
		if(i >= nx) break;
		if(i >= ny) break;

		params.i 	 = i;

		code = LAStateSystemSolver(a, b, c, d, dx, u, xs, ys, s, solver, LASolverSKFunc2, &params, h, t0, LASSCallbackSK, LASSCallbackIndexSK, &de);
		if(code) return code;

		dy = ys[0] - yn1;
		if(dy > slewRate) 			dy = slewRate;
		else if(dy < -(slewRate))	dy = -slewRate;

		dy = yn1 + dy;
		y[i] = dy * outputGain;
		yn1 = dy;
	}

	return code;
}

LAErrorCode LACircuitSimSKHP(double *x, size_t nx, double *y, size_t ny, double r1, double r2, double c1, double c2, double v0c1, double v0c2, double rl, double rh, double slewRate, double sampleRate, size_t solver, double inputGain, double outputGain){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	LASolverParameterSK params;

	double H  = rl / (rl + rh);
	double a1 = H / (r1*r2*c1*c2);
	double a2 = (1+H)/(r1*c1) + (H/r2)*(1/c1 + 1/c2);
	double a3 = H*(1 + 2*c2/c1);

	const size_t s = 2;
	double a[4] 	= {  0.0, 1.0,
						-a1/a3, -a2/a3};
	double b[4] 	= {	0.0, 0.0,
					   	0.0, 0.0};
	double c[4] 	= { 1.0, 0.0,
						0.0, 0.0};
	double d[4] 	= {	0.0, 0.0,
						0.0, 0.0};

	double dx[2]	= {0.0, 0.0};
	double u[2]		= {0.0, 0.0};
	double xs[2]	= {v0c1, v0c2};
	double ys[2]	= {0.0, 0.0};

	double h 		= 1 / (double) (sampleRate);
	double t0		= 0;

	params.a3 			= a3;
	params.b3 			= 1.0;
	params.slewRate		= slewRate / h;

	double yn1			= 0;
	double dy			= 0;

	LASolverEulerImplicitDetails de;
	de.nmax = 50;
	de.atol = 1e-10;
	de.df   = LASolverRCFuncDf;
	double prev[2] = {0.0, 0.0};
	de.prev = prev;
	de.pIndex = 0;
	de.subTime = LA_CIRCUITSIM_RKF_SUBTIME;
	de.minTime = LA_CIRCUITSIM_RKF_MINTIME;

	params.vs = x;
	params.vsSize = nx;
	params.y = y;
	params.ySize = ny;
	params.inputGain = inputGain;

	for(size_t i = 0; i < nx; i++){
		if(i >= nx) break;
		if(i >= ny) break;

		params.i 	 = i;

		code = LAStateSystemSolver(a, b, c, d, dx, u, xs, ys, s, solver, LASolverSKFunc2, &params, h, t0, LASSCallbackSK, LASSCallbackIndexSK, &de);
		if(code) return code;

		dy = ys[0] - yn1;
		if(dy > slewRate) 			dy = slewRate;
		else if(dy < -(slewRate))	dy = -slewRate;

		dy = yn1 + dy;
		y[i] = dy * outputGain;
		yn1 = dy;
	}

	return code;
}

LAErrorCode LACircuitSimMFBBP(double *x, size_t nx, double *y, size_t ny, double r1, double r2, double r3, double c1, double c2, double v0c1, double v0c2, double rl, double rh, double slewRate, double sampleRate, size_t solver, double inputGain, double outputGain){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	LASolverParameterSK params;

	double H  = rl / (rl + rh);
	double a1 = (H-1) * (1 + r1/r2);
	double a2 = r1*(c1+c2) * (H - 1) + c2*r3*H * (1 + r1/r2);
	double a3 = r1*r3*c2*(c1+c2)*H;
	double b2 = c2*r3;

	const size_t s = 2;
	double a[4] 	= {  0.0, 1.0,
						-a1/a3, -a2/a3};
	double b[4] 	= {	0.0, 0.0,
					   	0.0, 0.0};
	double c[4] 	= { 1.0, 0.0,
						0.0, 0.0};
	double d[4] 	= {	0.0, 0.0,
						0.0, 0.0};

	double dx[2]	= {0.0, 0.0};
	double u[2]		= {0.0, 0.0};
	double xs[2]	= {v0c1, v0c2};
	double ys[2]	= {0.0, 0.0};

	double h 		= 1 / (double) (sampleRate);
	double t0		= 0;

	params.a3 			= a3;
	params.b3 			= b2;
	params.slewRate		= slewRate / h;

	double yn1			= 0;
	double dy			= 0;

	LASolverEulerImplicitDetails de;
	de.nmax = 50;
	de.atol = 1e-10;
	de.df   = LASolverRCFuncDf;
	double prev[2] = {0.0, 0.0};
	de.prev = prev;
	de.pIndex = 0;
	de.subTime = LA_CIRCUITSIM_RKF_SUBTIME;
	de.minTime = LA_CIRCUITSIM_RKF_MINTIME;

	params.vs = x;
	params.vsSize = nx;
	params.y = y;
	params.ySize = ny;
	params.inputGain = inputGain;

	for(size_t i = 0; i < nx; i++){
		if(i >= nx) break;
		if(i >= ny) break;

		params.i 	 = i;

		code = LAStateSystemSolver(a, b, c, d, dx, u, xs, ys, s, solver, LASolverSKFunc2, &params, h, t0, LASSCallbackSK, LASSCallbackIndexSK, &de);
		if(code) return code;

		dy = ys[0] - yn1;
		if(dy > slewRate) 			dy = slewRate;
		else if(dy < -(slewRate))	dy = -slewRate;

		dy = yn1 + dy;
		y[i] = dy * outputGain;
		yn1 = dy;
	}

	return code;
	(void) r2;
}

LAErrorCode LAFilterCircuitSimSolve(double *x, size_t nx, double *y, size_t ny, LAFilterValue *value){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value, 	LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	double r1 	= value->csParam[LA_CS_PARAM_R1];
	double r2 	= value->csParam[LA_CS_PARAM_R2];
	double r3 	= value->csParam[LA_CS_PARAM_R3];
	double rh 	= value->csParam[LA_CS_PARAM_RH];
	double rl 	= value->csParam[LA_CS_PARAM_RL];
	double c1 	= value->csParam[LA_CS_PARAM_C1];
	double c2 	= value->csParam[LA_CS_PARAM_C2];
	double c3 	= value->csParam[LA_CS_PARAM_C3];
	double v0c1 = value->csParam[LA_CS_PARAM_V0C1];
	double v0c2 = value->csParam[LA_CS_PARAM_V0C2];
	double v0c3 = value->csParam[LA_CS_PARAM_V0C3];
	double slr 	= value->csParam[LA_CS_PARAM_SLEWRATE];
	double sar 	= value->csParam[LA_CS_PARAM_SAMPLERATE];
	double ig 	= value->csParam[LA_CS_PARAM_INPUTGAIN];
	double og 	= value->csParam[LA_CS_PARAM_OUTPUTGAIN];

	switch(value->csType){
		case 0:	code = LACircuitSimRCLP(x, nx, y, ny, r1, c1, v0c1, sar, value->csSolver, ig, og);
				break;
		case 1:	code = LACircuitSimRCHP(x, nx, y, ny, r1, c1, v0c1, sar, value->csSolver, ig, og);
				break;
		case 2:	code = LACircuitSimSKLP(x, nx, y, ny, r1, r2, c1, c2, v0c1, v0c2, rl, rh, slr, sar, value->csSolver, ig, og);
				break;
		case 3:	code = LACircuitSimSKHP(x, nx, y, ny, r1, r2, c1, c2, v0c1, v0c2, rl, rh, slr, sar, value->csSolver, ig, og);
				break;
		case 4:	code = LACircuitSimMFBBP(x, nx, y, ny, r1, r2, r3, c1, c2, v0c1, v0c2, rl, rh, slr, sar, value->csSolver, ig, og);
				break;
	}

	(void) c3;
	(void) v0c3;
	return code;
}
