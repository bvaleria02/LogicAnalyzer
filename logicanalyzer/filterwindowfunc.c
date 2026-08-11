#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "liblogicanalyzer.h"
#include "error.h"
#include "utils.h"
#include "filter/windows.h"
#include "filter/filter.h"
#include "fixedpoint/fixedpoint.h"
#include <math.h>


/*
	{Window name, {
		// parameter
		{Parameter name,	default,	min,	max,	step,	page, digits (decimal)},
		callable function
	}},
*/

LAFilterWindowDetails LAWindowTypeDetails[LA_FILTER_WINDOW_COUNT] = {
	{"Rectangle", {
		{"Duty cycle (%)",	0,		0,		1,		0.001,	0.1,	3}, 	
		{"Sample Offset",	0,		-1,		1,		0.001,	0.1,	3}, 		
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateRectWindow
	},
	{"Triangle", {
		{"Assymetry",		0,		-0.5,	0.5,	0.001,	0.01,	3}, 		
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateTriWindow
	},
	{"Welch", {
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateWelchWindow
	},
	{"Hanning",	{
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateHannWindow
	},
	{"Hamming",	{
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateHammingWindow
	},
	{"Trapezoidal", {
		{"Flat top (%)",	0.5,	0,		1,		0.001,	0.1,	3},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateTrapzWindow
	},
	{"Circular", {
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateCircWindow
	},
	{"Sinc", {
		{"Frequency (𝑓)",	4,		0,		32768,	0.1,	1,		2},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateSincWindow
	},
	{"Impulse",	{
		{"Sample offset",	0,		-1,		1,		0.001,	0.1,	3},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateImpulseWindow
	},
	{"Blackman", {
		{"Alpha (α)",		0.5,	-1,		1,		0.001,	0.1,	3},		
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateBlackmanWindow
	},
	{"Blackman-Harris", {
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateBlackmanHarrisWindow
	},
	{"Kaiser", {
		{"A",				52,		0,		32767,	1,		10,		0},
		{"kmax",			8,		0,		13,		1,		10,		0},
		{"Beta (β), negative=auto",0.02, -1 ,1,		0.001,	0.01,	3}},
		LAFilterGenerateKaiserWindow
	},
	{"Gaussian", {
		{"Sigma (σ)",		0.2,	0,		32767,	0.01,	1,		2},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateGaussianWindow
	},
	{"Nuttall", {
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateNutallWindow
	},
	{"Flat top", {
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateFlattopWindow
	},
	{"Parzen", {
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateParzenWindow
	},
	{"Cosine sum", {
		{"a0",				0.4266,	-1,		1,		0.001,	0.1,	3},	
		{"a1",				0.4966,	-1,		1,		0.001,	0.1,	3},	
		{"a2",				0.0768,	-1,		1,		0.001,	0.1,	3}},
		LAFilterGenerateCosineSumWindow
	},
	{"Blackman-Nutall", {
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateBlackmanNutallWindow
	},
	{"Sine power", {
		{"Exponent",		2,		0,		32767,	0.01,	1,		2},
		{"Frequency",		1,	-32768,		32768,	0.01,	1,		2},
		{"Phase",			0,		-1,		1,		0.001,	0.1,	3}},
		LAFilterGenerateSinePowerWindow
	},
	{"Approximated confined gaussian", {
		{"Sigma (σ)",		0.2,	0,		32767,	0.01,	1,		2},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateApproxGaussianWindow
	},
	{"Tukey", {
		{"Alpha (α)",		0.5,	0,		1,		0.001,	0.1,	3},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateTukeyWindow
	},
	{"Plank-tapper", {
		{"Epsilon (ε)",		0.2,	0,		0.5,	0.001,	0.1,	3},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGeneratePlankTapperWindow
	},
	{"Poisson",	{
		{"tau",				4,		0,		32767,	0.01,	0.1,	2},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGeneratePoissonWindow
	},
	{"Lanczos",	{
		{"f",				1,		0,		32767,	1,		10,		0},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateLanczosWindow
	},
	{"Noise", {
		{"DC Offset",		-0.5,	-1,		1,		0.001,	0.1,	3},		
		{"Noise Amplitude",	1,		0,		1,		0.001,	0.1,	3},
		{"Seed",			0,		0,		1,		1e-4,	0.2,	4}},
		LAFilterGenerateNoiseWindow
	},
	{"Logistical", {
		{"Alpha (α)",		1,		0,		32767,	0.01,	1,		2},
		{"B",				0,		0,		32767,	0.01,	1,		2},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateLogisticalWindow
	},
	{"Logistical 2", {
		{"Alpha (α)",		10,		0,		32767,	0.01,	1,		2},		
		{"B",				1,		0,		32767,	0.01,	1,		2},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateLogistical2Window
	},
	{"Damped", {
		{"ωn",				50,		0,		32767,	0.001,	1,		3},
		{"ξ",				0.2,	0,		1,		1e-4,	1e-2,	4},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateDampedWindow
	},
	{"Gaussine", {
		{"Sigma (σ)", 		10,		0,		32767,	0.01,	1,		2},
		{"Frequency", 		5,		0,		32767,	0.01,	1,		2},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateGaussineWindow
	},
	{"Polynomian Chebyshev", {
		{"k",				4,		0,		32767,	0.01,	1,		2},
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGeneratePolyChebyshevWindow
	},
	{"Smooth Trapezoidal", {
		{"k",				0.25,	0,		0.5,	0.001,	0.01,	3},		
		{NULL,				0,		0,		0,		0,		0,		0},
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateSmoothTrapezoidalWindow
	},
	{"Root Chebyshev Smoothed", {
		{"k",				2,		0,		32767,	0.01,	1,		2},
		{"L",				0.3,	0,		1,		0.001,	0.01,	3},		
		{NULL,				0,		0,		0,		0,		0,		0}},
		LAFilterGenerateRootChebyshevSmoothWindow
	},
	{"Compact Sine", {
		{"Frequency",		2,		0,		32767,	0.01,	1,		2},
		{"Phase",			0,		-1,		1,		0.001,	0.01,	3},
		{"Duty Cycle",		0.5,	0,		1,		0.001,	0.01,	3}},
		LAFilterGenerateCompactSineWindow
	}
};

LAErrorCode LAFilterGenerateWindowArray(double *buffer, size_t size, LAFilterWindowType windowType, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(buffer,		LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	if(windowType >= 0 && windowType < LA_FILTER_WINDOW_COUNT){
		code = LAWindowTypeDetails[windowType].callable(buffer, size, params, paramCount);
	} else {
		// if invalid, use Hann window as fallback
		code = LAWindowTypeDetails[LA_FILTER_WINDOW_HANN].callable(buffer, size, params, paramCount);
	}
	return code;
}

LAErrorCode LAFilterGenerateRectWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double p1 = LA_GET_PARAMETER(0, params, paramCount);
	double p2 = LA_GET_PARAMETER(1, params, paramCount);

	LA_CLAMP_VALUE(p1, 0, 1);
	LA_CLAMP_VALUE(p2, -1, 1);

	double a = 0;
	double m  = p1 * (0.5);
	double l1 = m + p2;
	double l2 = (1 - m) + p2;

	for(size_t i = 0; i < size; i++){
		a = i / (double) size;

		if(a < l1)			array[i] = 0;
		else if (a > l2)	array[i] = 0;
		else				array[i] = 1;
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateTriWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double p1 = LA_GET_PARAMETER(0, params, paramCount);
	double s2 = size * (0.5 + p1);

	for(size_t i = 0; i < size; i++){
		if(i < s2)		array[i] = i / (double) s2;
		else			array[i] = 1 - (i - s2) / (double) (size - s2); 
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateWelchWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	size_t n = size - 1;

	for(size_t i = 0; i < size; i++){
		array[i] = ((4 * i) / (double) n) * (1 - i / (double) n);
	}

	(void) paramCount;
	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateHannWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	/*
		cos2(x) = (1 - cos(2x)) /2
	*/

	for(size_t i = 0; i < size; i++){
		array[i] = (1 - cos(2 * M_PI * i / (double) size)) / (double) 2;
	}

	(void) paramCount;
	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateHammingWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	for(size_t i = 0; i < size; i++){
		array[i] = 0.54 - 0.46 * cos(2 * M_PI * i / (double) size);
	}

	(void) paramCount;
	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateTrapzWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double p1 = LA_GET_PARAMETER(0, params, paramCount);

	LA_CLAMP_VALUE(p1, -1, 1);
	size_t s1 = (p1 / (double) 2) * size;
	size_t s2 = (1 - (p1 / (double) 2)) * size;

	for(size_t i = 0; i < size; i++){
		if(i < s1)			array[i] = i / (double) s1;
		else if (i > s2)	array[i] = 1 - (i - s2) / (double) s1;
		else				array[i] = 1;
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateCircWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	for(size_t i = 0; i < size; i++){
		array[i] = 2 * sqrt(i*(size - 1) - pow(i, 2)) / (size - 1);
	}

	(void) paramCount;
	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateSincWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double p1 = LA_GET_PARAMETER(0, params, paramCount);

	double t0 = size / (double) 2;
	double t = 0;

	for(size_t i = 0; i < size; i++){
		t = p1 * (i - t0);
		if(fabs(t) < LA_EPS)		array[i] = 1;
		else						array[i] = sin(t) / t;
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateImpulseWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double p1 = LA_GET_PARAMETER(0, params, paramCount);
	size_t midSample = (size * (0.5 + p1));

	for(size_t i = 0; i < size; i++){
		array[i] = (i == midSample) ? 1 : 0;
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateCosineWindow(double *array, size_t size, double a0, double a1, double a2, double a3, double a4){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);

	for(size_t i = 0; i < size; i++){
		array[i]  = a0;
		array[i] -= a1 * cos(2 * M_PI * i / (double) size);
		array[i] += a2 * cos(4 * M_PI * i / (double) size);
		array[i] -= a3 * cos(6 * M_PI * i / (double) size);
		array[i] += a4 * cos(8 * M_PI * i / (double) size);
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateBlackmanWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double p1 = LA_GET_PARAMETER(0, params, paramCount);

	double a0 = (1 - p1) / (double) 2;
	double a1 = 0.5;
	double a2 = p1 / (double) 2;

	LAErrorCode code =  LAFilterGenerateCosineWindow(array, size, a0, a1, a2, 0, 0);
	return code;
}

LAErrorCode LAFilterGenerateBlackmanHarrisWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double a0 = 0.35878;
	double a1 = 0.48829;
	double a2 = 0.14128;
	double a3 = 0.01668;

	LAErrorCode code =  LAFilterGenerateCosineWindow(array, size, a0, a1, a2, a3, 0);
	(void) paramCount;
	return code;
}

#define A_LIMIT_1 21
#define A_LIMIT_2 50
#define KMAX_MAX 13

const uint64_t LAFactLUT[KMAX_MAX + 1] = {
	1, 1, 2, 6, 24, 120, 720, 5040,
	40320, 362880, 3628800, 39916800, 479001600,
	6227020800
};

double LAFilterKaiserI0(double x, size_t kmax){
	double frac1 = 0.0;
	double frac2 = 0.0;
	double value = 0.0;
	double acc   = 0.0;

	if(kmax > KMAX_MAX) kmax = KMAX_MAX;

	for(size_t k = 0; k <= kmax; k++){
		frac1 = 1 / (double) LAFactLUT[k];
		frac1 = frac1 * frac1;
		if(fabs(frac1) < LA_EPS) break;

		frac2 = x / (double) 2;
		frac2 = frac2 * frac2;
		frac2 = pow(frac2, k);

		value = frac1 * (double) frac2;
		if(fabs(value) < LA_EPS) break;

		acc += value;
	}

	//g_print("x: %lf\tkmax: %li\tvalue: %lf\n", x, kmax, value);
	return value;
}

LAErrorCode LAFilterGenerateKaiserWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	// p1: A
	// p2: kmax
	// p3: beta
	double p1 = LA_GET_PARAMETER(0, params, paramCount);
	double p2 = LA_GET_PARAMETER(1, params, paramCount);
	double p3 = LA_GET_PARAMETER(2, params, paramCount);

	LA_CLAMP_VALUE(p1, 0, 100);	// p1: A
	LA_CLAMP_VALUE(p2, 0, 13);	// p2: kamx
	LA_CLAMP_VALUE(p3, -1, 1);	// p3: beta, negative for auto

	if(p3 < 0 && p1 < A_LIMIT_1)							p3 = 0;
	else if (p3 < 0 && p1 >= A_LIMIT_1 && p1 < A_LIMIT_2)	p3 = 0.5842 * pow(p1 - A_LIMIT_1, 0.4) + 0.07886 * (p1 - A_LIMIT_1);
	else if (p3 < 0 && p1 >= A_LIMIT_2)						p3 = 0.1102 * (p1 - 8.7);

	double i0beta = LAFilterKaiserI0(p3, ceil(p2)); 
	double c = 0.0;

	for(size_t i = 0; i < size; i++){
		c = p3 * sqrt(1 - pow(((2*i) / (double) (size - 1)) - 1, 2));
		array[i] = LAFilterKaiserI0(c, ceil(p2)) / (double) i0beta;
	//	g_print("i: %li\tv: %lf\n", i, array[i]);
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateGaussianWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double sigma = LA_GET_PARAMETER(0, params, paramCount);
	double n = size / (double) 2;

	for(size_t i = 0; i < size; i++){
		if(fabs(sigma) < LA_EPS)	array[i] = 0;
		else						array[i] = exp(-0.5 * pow((i - n) / (double) (sigma * n), 2));
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateNutallWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double a0 = 0.355768;
	double a1 = 0.487398;
	double a2 = 0.144232;
	double a3 = 0.010604;

	LAErrorCode code =  LAFilterGenerateCosineWindow(array, size, a0, a1, a2, a3, 0);
	(void) paramCount;
	return code;
}

LAErrorCode LAFilterGenerateFlattopWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double a0 = 0.21557895;
	double a1 = 0.41663158;
	double a2 = 0.277263158;
	double a3 = 0.083578947;
	double a4 = 0.006947368;

	LAErrorCode code =  LAFilterGenerateCosineWindow(array, size, a0, a1, a2, a3, a4);
	(void) paramCount;
	return code;
}

double _parzenW0(int64_t n, size_t L){
	double value = 0;
	if(abs((int32_t) n) < (L/2)){
		value = 1 - 6*pow(n / (double) (L/2), 2) * (1 - abs((int32_t) n) / (double) (L/2));
	} else if (L/4 < abs((int32_t) n) && abs((int32_t) n) <= L/2){
		value = 2 * pow(1 - abs((int32_t) n) / (double) (L/2),3);
	}

	return value;
}

LAErrorCode LAFilterGenerateParzenWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	for(size_t i = 0; i < size; i++){
		array[i] = _parzenW0(i - size/2, size);
	}

	(void) paramCount;
	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateCosineSumWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double p1 = LA_GET_PARAMETER(0, params, paramCount);
	double p2 = LA_GET_PARAMETER(1, params, paramCount);
	double p3 = LA_GET_PARAMETER(2, params, paramCount);

	LAErrorCode code =  LAFilterGenerateCosineWindow(array, size, p1, p2, p3, 0, 0);
	return code;
}

LAErrorCode LAFilterGenerateBlackmanNutallWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double a0 = 0.3635819;
	double a1 = 0.4891775;
	double a2 = 0.1365995;
	double a3 = 0.00106411;

	LAErrorCode code =  LAFilterGenerateCosineWindow(array, size, a0, a1, a2, a3, 0);
	(void) paramCount;
	return code;
}

LAErrorCode LAFilterGenerateSinePowerWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double p1 = LA_GET_PARAMETER(0, params, paramCount);
	double p2 = LA_GET_PARAMETER(1, params, paramCount);
	double p3 = LA_GET_PARAMETER(2, params, paramCount);

	for(size_t i = 0; i < size; i++){
		array[i] = pow(sin(p3 + p2 * M_PI * i / (double) size), p1);
	}

	return LA_NO_ERROR;
}

double _Gfunc(double x, size_t N, size_t L, double s){
	return exp(- pow((x - (N/2)) / (double) (2 * L * s), 2));
}

LAErrorCode LAFilterGenerateApproxGaussianWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double s = LA_GET_PARAMETER(0, params, paramCount);
	size_t N = size;
	size_t L = N+1;

	for(size_t i = 0; i < size; i++){
		array[i] = _Gfunc(i,N,L,s) - (_Gfunc(0.5,N,L,s) * _Gfunc(i+L,N,L,s) + _Gfunc(i-L,N,L,s)) / (double) (_Gfunc(-0.5+L,N,L,s) + _Gfunc(-0.5-L,N,L,s));
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateTukeyWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
	double N = size;

	for(size_t i = 0; i < size; i++){
		if(i < (a*N) / (double)2){
			array[i] = (1 - cos(2 * M_PI * i / (double) (a * N))) / (double) 2;
		} else if ((a*N) / (double) 2 <= i && i <= N/2){
			array[i] = 1;
		} else {
			array[i] = array[size - 1 - i];
		}
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGeneratePlankTapperWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double e = LA_GET_PARAMETER(0, params, paramCount);
	double N = size;

	for(size_t i = 0; i < size; i++){
		if(i == 0){
			array[i] = 0;
		} else if (1 <= i && i < e*N){
			array[i] = 1 / (double) (1 + exp(((e*N / (double) i) - (e*N) / (double) ((e*N) - i))));
		} else if (e*N <= i && i <= N/2){
			array[i] = 1;
		} else {
			array[i] = array[size - 1 - i];
		}
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGeneratePoissonWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double t = LA_GET_PARAMETER(0, params, paramCount);
	double N = size;

	for(size_t i = 0; i < size; i++){
		array[i] = exp(-fabs(i - N/2) * 1/(double)t);
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateLanczosWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double f = LA_GET_PARAMETER(0, params, paramCount);
	double N = size;
	double arg = 0;

	for(size_t i = 0; i < size; i++){
		arg = f * ((2*i / (double) N) - 1);
		if(fabs(arg) < LA_EPS){
			array[i] = 1;
		} else {
			array[i] = sin(M_PI * arg) / (double) (M_PI * arg);
		}
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateNoiseWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double offset 	= LA_GET_PARAMETER(0, params, paramCount);
	double a 		= LA_GET_PARAMETER(1, params, paramCount);
	double seed 	= LA_GET_PARAMETER(2, params, paramCount);

	// TODO: replace srand with and 64 bit LFSR
	srand(seed * RAND_MAX);

	for(size_t i = 0; i < size; i++){
		array[i] = offset + a * (rand() / (double) RAND_MAX);
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateLogisticalWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double a  = LA_GET_PARAMETER(0, params, paramCount);
	double b  = 0.5 + LA_GET_PARAMETER(1, params, paramCount);

	double norm = (2 / (double) (1 + exp(-a * b))) - 1;
	double n 	= 0;

	for(size_t i = 0; i < size; i++){
		n = i / (double) size;
		if(n < 0.5){
			array[i] = ((2 / (double) (1 + exp(-a * n))) - 1) / (double) norm;
		} else {
			array[i] = array[size - 1 - i];
		}
	}

	return LA_NO_ERROR;
}

double _logis(double x){
	return 1 / (double) (1 + exp(x));
}

LAErrorCode LAFilterGenerateLogistical2Window(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double a = LA_GET_PARAMETER(0, params, paramCount);
//	double b = 0.5 + LA_GET_PARAMETER(1, params, paramCount);

	double offset 	= _logis(a * 0.25);
	double norm 	= _logis(-a * (0.5 - 0.25));
	double n 	= 0;

	for(size_t i = 0; i < size; i++){
		n = i / (double) size;
		if(n < 0.5){
			array[i] = (_logis(-a*(n - 0.25)) - offset) / (double) norm;
		} else {
			array[i] = array[size - 1 - i];
		}
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateDampedWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double wn = LA_GET_PARAMETER(0, params, paramCount);
	double xi = LA_GET_PARAMETER(1, params, paramCount);

	double wd 		= wn * sqrt(1 - xi*xi);
	double n		= 0;

	for(size_t i = 0; i < size; i++){
		n = i / (double) size;
		if(n < 0.5){
			array[i] = 1 - (exp(-xi*wn*n) / (double) sqrt(1 -xi*xi)) * sin(wd*n + atan(sqrt(1 - xi*xi) / (double) xi));
		} else {
			array[i] = array[size - 1 - i];
		}
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateGaussineWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double s = LA_GET_PARAMETER(0, params, paramCount);
	double f = LA_GET_PARAMETER(1, params, paramCount);
	double n = 0;

	for(size_t i = 0; i < size; i++){
		n = i / (double) size;
		array[i] = exp(-s*pow(n - 0.5, 2)) * sin(M_PI*n*f);
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGeneratePolyChebyshevWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double k = LA_GET_PARAMETER(0, params, paramCount);
	double n = 0;

	for(size_t i = 0; i < size; i++){
		n = i / (double) size;
		array[i] = (1 - cos(k * acos(2*n - 1))) / (double) 2;
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateSmoothTrapezoidalWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double k = LA_GET_PARAMETER(0, params, paramCount);
	double n = 0;

	for(size_t i = 0; i < size; i++){
		n = i / (double) size;
		if(n <= k / (double) 2 && n <= 0.5)  	array[i] = (2*n) / (double) k;
		else if (k/(double)2 < n && n <= 0.5)   array[i] = 1 - ((n - (k / (double) 2)) / (0.5 - (k / (double) 2)));

		if(n <= 0.5){
			array[i] = pow(sin((M_PI / (double) 2) * array[i]), 2);
		} else {
			array[i] = array[size - 1 - i];
		}
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateRootChebyshevSmoothWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double k  = LA_GET_PARAMETER(0, params, paramCount);
	double p2 = LA_GET_PARAMETER(1, params, paramCount);
	double n 	= 0;

	ufixed32_t vf = convertDoubleToUF32(p2);
	ufixed32_t v2 = ufixedGamma32(vf);
	double 	   L  = convertUF32ToDouble(v2);

	for(size_t i = 0; i < size; i++){
		n = i / (double) size;
		if(n <= 0.5){
			array[i] = sin(k * asin(sqrt(n)));
			array[i] = pow(sin((M_PI / (double) 2) * array[i]), 2);
			array[i] = pow(sin(array[i] * L), 2);
		} else {
			array[i] = array[size - 1 - i];
		}
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateCompactSineWindow(double *array, size_t size, double *params, size_t paramCount){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(params, LA_PROPAGATE_ERROR);

	double a 	= LA_GET_PARAMETER(0, params, paramCount);
	double b 	= LA_GET_PARAMETER(1, params, paramCount);
	double k 	= LA_GET_PARAMETER(2, params, paramCount);

	double n 	= 0;
	double p	= 0;

	for(size_t i = 0; i < size; i++){
		n = i / (double) size;
		p = (n - k/(double)2) / (1 - k);

		if(n >= k/(double)2 && n < (1 - k/(double)2)){
			array[i] = sin(a * M_PI * p + b * M_PI);
		} else {
			array[i] = 0;
		}
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterWindowComboBox(GtkWidget **widget){
	LA_HANDLE_NULLPTR(widget, LA_PROPAGATE_ERROR);

	(*widget) = gtk_combo_box_text_new();
	for(size_t i = 0; i < LA_FILTER_WINDOW_COUNT; i++){
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT((*widget)), LAWindowTypeDetails[i].name);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX((*widget)), 0);

	return LA_NO_ERROR;
}
