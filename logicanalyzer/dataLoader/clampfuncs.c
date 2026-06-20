#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include "../liblogicanalyzer.h"
#include "dataLoader.h"

static inline double midpoint(double a, double b){
	return (b + a) / (double) 2;
}

static inline double midsize(double a, double b){
	return (b - a) / (double) 2;
}

static inline double max(double a, double b){
	return (b >= a) ? b : a;
}

static inline double min(double a, double b){
	return (b <= a) ? b : a;
}


LAErrorCode LAClampNoClamping(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);
/*
	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		y[i] = x[i];
	}*/

	LAErrorCode code = LACLAMP_NO_CLAMP(x, &sizeX, y, &sizeY);
	(void) paramCount;
	return code;
}

LAErrorCode LAClampHardClipping(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double b = LA_GET_PARAMETER(1, params, paramCount);
	/*
	double p = midpoint(a, b);
	double q = midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		y[i] = max(min(x[i], b), a);
	}
*/
	LAErrorCode code = LACLAMP_HARD_CLIP(x, &sizeX, y, &sizeY, a, b);

	return code;
}

LAErrorCode LAClampSoftClipping(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double b = LA_GET_PARAMETER(1, params, paramCount);
	double k = LA_GET_PARAMETER(2, params, paramCount);
	/*
	double p = midpoint(a, b);
	double q = midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		y[i] = q * tanh(k * (x[i] - p)) + p;
	}
	*/
	LAErrorCode code = LACLAMP_SOFT_CLIP(x, &sizeX, y, &sizeY, a, b, k);

	return code;
}

static inline double r(double t){
	return (t >= 0) ? t : 0;
}

static inline double tri_0(double t){
	return (t >= 0 && t < 1) ? 4*r(t) - 8*r(t - 0.25) + 8*r(t - 0.75) : 0;
}

static inline double tri(double t){
	return tri_0(t - floor(t));
}

LAErrorCode LAClampHardWavefolding(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double b = LA_GET_PARAMETER(1, params, paramCount);
	double k = LA_GET_PARAMETER(2, params, paramCount);
	/*
	double p = midpoint(a, b);
	double q = midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		y[i] = q * tri(k * (x[i]-p) / (double) (4*q)) + p;
	}
*/
	LAErrorCode code = LACLAMP_HARD_FOLD(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

LAErrorCode LAClampSoftWavefolding(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double b = LA_GET_PARAMETER(1, params, paramCount);
	double k = LA_GET_PARAMETER(2, params, paramCount);
	/*
	double p = midpoint(a, b);
	double q = midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		y[i] = q * sin(((k*M_PI) / (double) 2) * (x[i]-p) /(double) q) + p;
	}
*/
	LAErrorCode code = LACLAMP_SOFT_FOLD(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

static inline double decimal(double t){
	return t - floor(t);
}

LAErrorCode LAClampModulo(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double b = LA_GET_PARAMETER(1, params, paramCount);
	double k = LA_GET_PARAMETER(2, params, paramCount);
	/*
	double p = midpoint(a, b);
	double q = midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		y[i] = 2*q * decimal(((x[i] - p) / (double) (2*q)) + 0.5) - q + p;
	}
*/
	LAErrorCode code = LACLAMP_MODULO(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

LAErrorCode LAClampSoftSign(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double b = LA_GET_PARAMETER(1, params, paramCount);
	double k = LA_GET_PARAMETER(2, params, paramCount);
/*	double p = midpoint(a, b);
	double q = midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		y[i] = p + q * k * (x[i] - p) / (double) (1 + k*fabs(x[i]-p));
	}
*/
	LAErrorCode code = LACLAMP_SOFT_SIGN(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

static inline double x3kx(double t, double k){
	return (t*t*t + (k*t)) / (double) (1 + k);
}

LAErrorCode LAClampCubicClamping(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double b = LA_GET_PARAMETER(1, params, paramCount);
	double k = LA_GET_PARAMETER(2, params, paramCount);
/*	double p = midpoint(a, b);
	double q = midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		if(x[i] < a) 		y[i] = a;
		else if (x[i] > b)	y[i] = b;
		else 				y[i] = q * x3kx((x[i]-p) / (double) q, k) + p;
	}
*/
	LAErrorCode code = LACLAMP_CUBIC_CLAMP(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

LAErrorCode LAClampSimpleFoldover(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double b = LA_GET_PARAMETER(1, params, paramCount);
	double k = LA_GET_PARAMETER(2, params, paramCount);
/*	double p = midpoint(a, b);
	double q = midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		if      (k * x[i] < a) 	y[i] = - k * x[i] + 2*a;
		else if (k * x[i] > b)	y[i] = - k * x[i] + 2*b;
		else					y[i] = k * x[i];
	}
*/
	LAErrorCode code = LACLAMP_SIMPLE_FOLDOVER(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

static inline double sign(double t){
	return (t != 0) 
				? ((t > 0) ? 1 : -1)
				: 0;
}

static inline double x2m1(double t){
	return -pow(fabs(t) - 1, 2) + 1;
}

LAErrorCode LAClampDoubleCuspFold(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double b = LA_GET_PARAMETER(1, params, paramCount);
	double k = LA_GET_PARAMETER(2, params, paramCount);
/*	double p = midpoint(a, b);
	double q = midsize(a, b);
	double t = 0;

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		t = (x[i] - p) / (double) q;

		y[i] = q * sign(k*t) * x2m1(k*t) + p;
	}
*/
	LAErrorCode code = LACLAMP_DOUBLE_CUSP_FOLD(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

static inline double expNorm(double t, double k){
	return (1 - exp(-k*t)) / (double) (1 - exp(-k));
}

static inline double u(double t){
	return (t >= 0) ? 1 : 0;
}

static inline double sawSquare_0(double t){
	return 2*r(t) - u(t-0.5) - 4*r(t-0.5);
}

static inline double sawSquare(double t){
	return sawSquare_0((t+1)/(double)4 - floor((t+1)/(double)4));
}

LAErrorCode LAClampExponentialFold(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double b = LA_GET_PARAMETER(1, params, paramCount);
	double k = LA_GET_PARAMETER(2, params, paramCount);
/*	double p = midpoint(a, b);
	double q = midsize(a, b);
	double t = 0;

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		t = sawSquare((x[i] - p) / q);
		
		if(t > 0) t = expNorm(t, k);
		else	  t = 1 - expNorm(-t, k);

		y[i] = q*(2*t-1) + p;
	}
*/
	LAErrorCode code = LACLAMP_EXPONENTIAL_FOLD(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

LAErrorCode LAClampPartialWrap(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double b = LA_GET_PARAMETER(1, params, paramCount);
	double k = LA_GET_PARAMETER(2, params, paramCount);
/*	double p = midpoint(a, b);
	double q = midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;

		if(x[i] > b)		y[i] = (k * (x[i] - b)) + b;
		else if(x[i] < a)	y[i] = (k * (x[i] - a)) + a;
		else				y[i] = x[i];
	}
*/
	LAErrorCode code = LACLAMP_PARTIAL_WRAP(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

LAErrorCode LAClampSymmetricalLogistic(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double b = LA_GET_PARAMETER(1, params, paramCount);
	double k = LA_GET_PARAMETER(2, params, paramCount);
/*	double p = midpoint(a, b);
	double q = midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
	
		y[i] = -q + p + ((2*q) / (double) (1 + exp(-k * ((x[i] - p) / (double) q))));
	}
*/
	LAErrorCode code = LACLAMP_SYMMETRICAL_LOGISTIC(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

static inline double r3tkpmr3k(double t, double k){
	return pow(t + k, 1 / (double) 3) - pow(k, 1 / (double) 3);
}

LAErrorCode LAClampSCurve(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double b = LA_GET_PARAMETER(1, params, paramCount);
	double k = LA_GET_PARAMETER(2, params, paramCount);
/*	double p = midpoint(a, b);
	double q = midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		if(x[i] <= p)	y[i] = -r3tkpmr3k(-(x[i] - p) / (double) q, k);
		else			y[i] =  r3tkpmr3k( (x[i] - p) / (double) q, k);

		y[i] = q *( y[i] / (double) r3tkpmr3k(1, k)) + p;
	}
*/
	LAErrorCode code = LACLAMP_S_CURVE(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

static inline double expGate(double t, double k, double q){
	return t * (1 + exp(-k *(q - fabs(t)))) / (double) 2;
}

LAErrorCode LAClampExponentialGate(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double b = LA_GET_PARAMETER(1, params, paramCount);
	double k = LA_GET_PARAMETER(2, params, paramCount);
/*	double p = midpoint(a, b);
	double q = midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
	
		y[i] = q*expGate((x[i] - p) / (double) q, k, q) + p;
	}
*/
	LAErrorCode code = LACLAMP_EXPONENTIAL_GATE(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

static inline double compRatio(double t, double k){
	return log(1 + k*t) / (double) k;
}

LAErrorCode LAClampSoftKnee(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double b = LA_GET_PARAMETER(1, params, paramCount);
	double k = LA_GET_PARAMETER(2, params, paramCount);
/*	double p = midpoint(a, b);
	double q = midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;

		if(x[i] > b)		y[i] = b + compRatio(x[i] - b, k);
		else if (x[i] < a)	y[i] = a - compRatio(-(x[i] - a), k);
		else				y[i] = x[i];
	}
*/
	LAErrorCode code = LACLAMP_SOFT_KNEE(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

LAErrorCode LAClampSaturatedLogisticMap(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double b = LA_GET_PARAMETER(1, params, paramCount);
	double k = LA_GET_PARAMETER(2, params, paramCount);
/*	double p = midpoint(a, b);
	double q = midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		// WIP	
	}
*/
	LAErrorCode code = LACLAMP_SATURATED_LOGISTIC_MAP(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

static inline double tentMap_fu(double t, double mu){
	return mu * min(t, 1 - t);
}

LAErrorCode LAClampTentMap(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a  = LA_GET_PARAMETER(0, params, paramCount);
	double b  = LA_GET_PARAMETER(1, params, paramCount);
	double mu = LA_GET_PARAMETER(2, params, paramCount);
	double y0 = LA_GET_PARAMETER(3, params, paramCount);
/*	double p = midpoint(a, b);
	double q = midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		if(i == 0){
			y[i] = y0;
		} else {
			y[i] = q * tentMap_fu((((x[i] + y[i-1]) / (double) 2) - p) / (double) q, mu) + p;
		}
	}
*/
	LAErrorCode code = LACLAMP_TENT_MAP(x, &sizeX, y, &sizeY, a, b, mu, y0);
	return code;
}

LAErrorCode LAClampBJTSaturation(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double b = LA_GET_PARAMETER(1, params, paramCount);
	double k = LA_GET_PARAMETER(2, params, paramCount);
/*	double p = midpoint(a, b);
	double q = midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		y[i] = p + q * (1 - exp(-k * (x[i] - p)));
	}
*/
	LAErrorCode code = LACLAMP_BJT_SATURATION(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

LAErrorCode LAClampSlewRateSaturation(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double s 	= LA_GET_PARAMETER(2, params, paramCount);
	double v0 	= LA_GET_PARAMETER(3, params, paramCount);
/*	double p 	= midpoint(a, b);
	double q 	= midsize(a, b);

	double dx	= 0;

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		if(i == 0){
			y[i] = v0;
		} else {
			dx = x[i] - y[i-1];
			if(dx > s) 			dx = s;
			else if (dx < -s)	dx = -s;
			y[i] = y[i-1] + dx;
		}

		if(y[i] > b)			y[i] = b;
		else if (y[i] < a) 		y[i] = a;
	}
*/
	LAErrorCode code = LACLAMP_SLEW_RATE_SATURATION(x, &sizeX, y, &sizeY, s, v0);
	return code;
}

LAErrorCode LAClampSchmittTrigger(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a 	= LA_GET_PARAMETER(0, params, paramCount);
	double b 	= LA_GET_PARAMETER(1, params, paramCount);
	double t 	= LA_GET_PARAMETER(2, params, paramCount);
	double v0 	= LA_GET_PARAMETER(3, params, paramCount);
/*	double p 	= midpoint(a, b);
	double q 	= midsize(a, b);

	double y1	= 0;
	double x1	= 0;
	t = t / (double) 2;

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		if(i == 0){
			y[i] = v0;
		} else {
			y1 = y[i-1];
			x1 = (x[i] - p) / (double) q;

			if      (y1 <= p && x1 >= (1-t))	y[i] = b;
			else if (y1 >  p && x1 <= (t)  )	y[i] = a;
			else 				  				y[i] = y1;
		}

	}
*/
	LAErrorCode code = LACLAMP_SCHMITT_TRIGGER(x, &sizeX, y, &sizeY, a, b, t, v0);
	return code;
}

static inline double rcFunc(double vi, double q, double r, double c){
	return (vi - (c*q)) / (double) r;
}


LAErrorCode LAClampRCLowPass(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a 	= LA_GET_PARAMETER(0, params, paramCount);
	double b 	= LA_GET_PARAMETER(1, params, paramCount);
	double r 	= LA_GET_PARAMETER(2, params, paramCount);
	double c 	= LA_GET_PARAMETER(3, params, paramCount);
	double y0 	= LA_GET_PARAMETER(4, params, paramCount);
/*
	double h	= 1 / (double) 2;
	double q    = y0;
	double k1,k2,k3,k4;
	double x1,x2,x3,x4;

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		x1 = x[i];
		x2 = x[i] + ((i+1) >= sizeX) ? x[i] : x[i+1];
		x2 = x2 / (double) 2;
		x3 = x2;
		x4 = ((i+1) >= sizeX) ? x[i] : x[i+1];

		k1 = rcFunc(x1, q, 			r, c);
		k2 = rcFunc(x2, q +   h*k1,	r, c);
		k3 = rcFunc(x3, q +   h*k2,	r, c);
		k4 = rcFunc(x4, q + 2*h*k3,	r, c);

		q += (k1 + 2*k2 + 2*k3 + k4) * (2*h / (double) 6);

		if(i > 0){
			y[i] = c * (y[i] - y[i-1]) ; 
		} else {
			y[i] =  q;
		}
	}
*/
	LAErrorCode code = LACLAMP_RC_LOW_PASS(x, &sizeX, y, &sizeY, a, b, r, c, y0);
	return code;
}

static inline double schokley(double t, double k, double is){
	return k * log((t / (double) is) + 1);
}

LAErrorCode LAClampSymmetricalDiode(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a 	= LA_GET_PARAMETER(0, params, paramCount);
	double b 	= LA_GET_PARAMETER(1, params, paramCount);
	double is 	= LA_GET_PARAMETER(2, params, paramCount);
	double k 	= LA_GET_PARAMETER(3, params, paramCount);
/*	double p 	= midpoint(a, b);
	double q 	= midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		if(x[i] > p)	y[i] =  q * schokley((x[i] - p) / (double) q, k, is) + p;
		else			y[i] = -q * schokley(-(x[i] - p) / (double) q, k, is) + p;
	}
*/
	LAErrorCode code = LACLAMP_SYMMETRICAL_DIODE(x, &sizeX, y, &sizeY, a, b, is, k);
	return code;
}

LAErrorCode LAClampUnilateralDiode(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a 	= LA_GET_PARAMETER(0, params, paramCount);
	double b 	= LA_GET_PARAMETER(1, params, paramCount);
	double is 	= LA_GET_PARAMETER(2, params, paramCount);
	double k 	= LA_GET_PARAMETER(3, params, paramCount);
/*	double p 	= midpoint(a, b);
	double q 	= midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		if(x[i] > p)	y[i] =  q * schokley((x[i] - p) / (double) q, k, is) + p;
		else			y[i] = p;
	}
*/
	LAErrorCode code = LACLAMP_UNILATERAL_DIODE(x, &sizeX, y, &sizeY, a, b, is, k);
	return code;
}

LAErrorCode LAClampDifferenciator(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double y0 	= LA_GET_PARAMETER(2, params, paramCount);
	double h 	= LA_GET_PARAMETER(3, params, paramCount);
/*	double p 	= midpoint(a, b);
	double q 	= midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		if(i == 0){
			y[i] = y0;
		} else {
			y[i] = (x[i] - x[i-1]) / h;
		}
	}
*/
	LAErrorCode code = LACLAMP_DIFFERENTIATOR(x, &sizeX, y, &sizeY, y0, h);
	return code;
}

LAErrorCode LAClampIntegrator(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double y0 	= LA_GET_PARAMETER(2, params, paramCount);
	double h 	= LA_GET_PARAMETER(3, params, paramCount);
/*	double p 	= midpoint(a, b);
	double q 	= midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		if(i == 0){
			y[i] = y0;
		} else {
			y[i] = y[i-1] + ((x[i] + x[i-1]) * h / (double) 2);
		}
	}
*/
	LAErrorCode code = LACLAMP_INTEGRATOR(x, &sizeX, y, &sizeY, y0, h);
	return code;
}

LAErrorCode LAClampStochastic(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a 	= LA_GET_PARAMETER(0, params, paramCount);
	double b 	= LA_GET_PARAMETER(1, params, paramCount);
/*	double p 	= midpoint(a, b);
	double q 	= midsize(a, b);

	srand(s * RAND_MAX);
	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		y[i] = x[i] * (rand() / (double) RAND_MAX);
		if(y[i] < a)		y[i] = a;
		else if(y[i] > b)	y[i] = b;
	}
*/
	LAErrorCode code = LACLAMP_STOCHASTIC(x, &sizeX, y, &sizeY, a, b);
	return code;
}

/*static double evenPower(double t, double k){
	double value = 0;
	for(size_t i=1; i <= floor(k); i++){
		value += pow(t, 2*i);
	}
	return 1 - (value / (double) floor(k));
}*/

LAErrorCode LAClampEvenPowerLimiter(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a 	= LA_GET_PARAMETER(0, params, paramCount);
	double b 	= LA_GET_PARAMETER(1, params, paramCount);
	double k 	= LA_GET_PARAMETER(2, params, paramCount);
/*	double p 	= midpoint(a, b);
	double q 	= midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		if(x[i] <= a)					y[i] = a;
		else if (x[i] >= b)				y[i] = b;
		else if (x[i] < p && x[i] > a)	y[i] = - q * evenPower(-(x[i]-a) / (double) q, k);
		else 							y[i] =   q * evenPower( (x[i]-b) / (double) q, k);
	}
*/
	LAErrorCode code = LACLAMP_EVEN_POWER_LIMITER(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

LAErrorCode LAClampEvenPowerFolder(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a 	= LA_GET_PARAMETER(0, params, paramCount);
	double b 	= LA_GET_PARAMETER(1, params, paramCount);
	double k 	= LA_GET_PARAMETER(2, params, paramCount);
/*	double p 	= midpoint(a, b);
	double q 	= midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		if (x[i] < p)	y[i] = - q * evenPower(-(x[i]-a) / (double) q, k);
		else 			y[i] =   q * evenPower( (x[i]-b) / (double) q, k);
	}
*/
	LAErrorCode code = LACLAMP_EVEN_POWER_FOLDOVER(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

static inline double snroot(double t, double k){
	return sign(t) * pow(fabs(t), 1 / (double) k);
}

LAErrorCode LAClampNroot(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a 	= LA_GET_PARAMETER(0, params, paramCount);
	double b 	= LA_GET_PARAMETER(1, params, paramCount);
	double k 	= LA_GET_PARAMETER(2, params, paramCount);
/*	double p 	= midpoint(a, b);
	double q 	= midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		y[i] = q*snroot((x[i] - p) / (double) q, k) + p;
	}
*/
	LAErrorCode code = LACLAMP_NROOT(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

static inline double softAbs(double t, double k){
	return (sqrt(t*t + k*k) - k) / (double) (sqrt(1 + k*k) - k);
}

LAErrorCode LAClampSoftAbs(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a 	= LA_GET_PARAMETER(0, params, paramCount);
	double b 	= LA_GET_PARAMETER(1, params, paramCount);
	double k 	= LA_GET_PARAMETER(2, params, paramCount);
/*	double p 	= midpoint(a, b);
	double q 	= midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		y[i] = q*softAbs((x[i] - p) / (double) q, k) + p;
	}
*/
	LAErrorCode code = LACLAMP_SOFT_ABS(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

static inline double rootSine(double t, double k){
	return sin((M_PI / (double) 2) * sqrt(fabs(pow(t,k)))) * sign(t);
}

LAErrorCode LAClampRootSine(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a 	= LA_GET_PARAMETER(0, params, paramCount);
	double b 	= LA_GET_PARAMETER(1, params, paramCount);
	double k 	= LA_GET_PARAMETER(2, params, paramCount);
/*	double p 	= midpoint(a, b);
	double q 	= midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		y[i] = q*rootSine((x[i] - p) / (double) q, k) + p;
	}
*/
	LAErrorCode code = LACLAMP_ROOT_SINE(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

LAErrorCode LAClampHardGauss1(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a 	= LA_GET_PARAMETER(0, params, paramCount);
	double b 	= LA_GET_PARAMETER(1, params, paramCount);
	double k 	= LA_GET_PARAMETER(2, params, paramCount);
/*	double p 	= midpoint(a, b);
	double q 	= midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		if(x[i] < a)		y[i] = a;
		else if (x[i] > b)	y[i] = b;
		else				y[i] = a + 2 * q * (x[i] - a) / (double) (b - a);

		y[i] = y[i] + q * exp(-pow(k * (x[i] - p) / (double) q, 2));
	}
*/
	LAErrorCode code = LACLAMP_HARD_GAUSS_1(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

LAErrorCode LAClampHardGauss2(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a 	= LA_GET_PARAMETER(0, params, paramCount);
	double b 	= LA_GET_PARAMETER(1, params, paramCount);
	double k 	= LA_GET_PARAMETER(2, params, paramCount);
/*	double p 	= midpoint(a, b);
	double q 	= midsize(a, b);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		if(x[i] < a)		y[i] = a;
		else if (x[i] > b)	y[i] = b;
		else				y[i] = a + 2 * q * (x[i] - a) / (double) (b - a);

		y[i] = y[i] * (1 + q * exp(-pow(k * (x[i] - p) / (double) q, 2)));
	}
*/
	LAErrorCode code = LACLAMP_HARD_GAUSS_2(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

static inline double bitCrusher(double t, double n){
	return floor(t * n) / (double) n;
}

LAErrorCode LAClampBitCrush(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a 	= LA_GET_PARAMETER(0, params, paramCount);
	double b 	= LA_GET_PARAMETER(1, params, paramCount);
	double k 	= LA_GET_PARAMETER(2, params, paramCount);

	printf("i: %li\n k: %lf\n", paramCount, k);
/*	double p 	= midpoint(a, b);
	double q 	= midsize(a, b);
	double n	= pow(2, k - 1);

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;
		
		if(x[i] < a)		y[i] = a;
		else if (x[i] > b)	y[i] = b;
		else				y[i] = q * bitCrusher((x[i] - p) / (double) q, n) + p;
	}
*/
	LAErrorCode code = LACLAMP_BIT_CRUSH(x, &sizeX, y, &sizeY, a, b, k);
	return code;
}

LAErrorCode LAClampTransientLimiter(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(x, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, 	LA_PROPAGATE_ERROR);

	double a 	= LA_GET_PARAMETER(0, params, paramCount);
	double b 	= LA_GET_PARAMETER(1, params, paramCount);
	double k 	= LA_GET_PARAMETER(2, params, paramCount);
	double y0 	= LA_GET_PARAMETER(3, params, paramCount);
	double h 	= LA_GET_PARAMETER(4, params, paramCount);
/*	double p 	= midpoint(a, b);
	double q 	= midsize(a, b);

	double dx  	= 0;
	double dxp 	= 0;

	for(size_t i = 0; i < sizeX; i++){
		if(i >= sizeY) break;

		if(i == 0){
			y[i] = y0;
			continue;
		}
		
		dx = (x[i] - y[i-1]) / (double) h;
		dxp = q * tanh(k * (dx - p) / (double) q) + p;

		y[i] = y[i-1] + h * dxp;
	}
*/
	LAErrorCode code = LACLAMP_TRANSIENT_LIMITER(x, &sizeX, y, &sizeY, a, b, k, y0, h);
	return code;
}
