#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "fixedpoint.h"
#include "fixedcalculations.h"

double integrator(mathFunc func, double param, double lowerLimit, double upperLimit, uint64_t steps){
	if(func == NULL){
		return 0;
	}

	if(steps == 0){
		return 0;
	}

	/*
		Uses Simpson 3/8 rule for integration

		int from a to b f(x,q) dx approx h * 3/8 * (f(x_0) + 3f(x_1) + 3f(x_2) + 2f(x_3) + ... + f(x_n))
		where:
			a = lowerLimit
			b = upperLimit
			h = (upperLimit - lowerLimit) / steps
			n = steps
			x_0 = lowerLimit
			x_n = upperLimit
	*/

	// steps correction to be multiple of 3
	if((steps % 3) != 0){
		steps = steps + (3 - (steps % 3));
	}

	double h 			= (upperLimit - lowerLimit) / (double) steps;
	double x 			= lowerLimit;
	double y 			= func(x, param);
	double accumulator 	= y;

	for(uint64_t i = 1; i < steps; i++){
		x = lowerLimit + (i * h);
		y = func(x, param);

		if((i % 3) == 0){
			accumulator += (2 * y);
		} else {
			accumulator += (3 * y);
		}

	}

	// Add the last term
	x = upperLimit;
	y = func(x, param);
	accumulator += y;

	return (3 * h * accumulator) / (double) 8 ;
}

double integratorUnit(IntegratorDetails *integratorDetails, mathFunc func){
	if(integratorDetails == NULL || func == NULL){
		return 0;
	}

	return integrator(
		func,
		integratorDetails->q,
		integratorDetails->lowerLimit,
		integratorDetails->upperLimit,
		integratorDetails->steps
	);
}

double optimizer(mathFunc realFunction, mathFunc approximation, double lowerIntegrationLimit, double upperIntegrationLimit, uint64_t integrationSteps, double lowerQlimit, double upperQlimit, uint8_t maxIterQ){
	if(realFunction == NULL || approximation == NULL){
		return 0;
	}

	double lowerQ 		= lowerQlimit;
	double upperQ 		= upperQlimit;
	double limitsError[2] = {0, 0};
	double qFactor		= 0;

	double errorSquaredFunc(double x, double q){
		return pow(
			((realFunction(x, q) - approximation(x, q)) / ((double) realFunction(x, q))),
			2
		);
	}

	for(uint8_t i = 0; i < maxIterQ; i++){
		qFactor = upperQ;
		limitsError[1] = integrator(errorSquaredFunc, qFactor, lowerIntegrationLimit, upperIntegrationLimit, integrationSteps);

		qFactor = lowerQ;
		limitsError[0] = integrator(errorSquaredFunc, qFactor, lowerIntegrationLimit, upperIntegrationLimit, integrationSteps);

		//printf("q: [%.12lf, %.12lf]\n", lowerQ, upperQ);

		if(limitsError[0] < limitsError[1]){
			upperQ = (upperQ + lowerQ) / (double) 2;
		} else if(limitsError[1] < limitsError[0]){
			lowerQ = (upperQ + lowerQ) / (double) 2;
		}

		//printf("q: [%lf: %lf, %lf: %lf]\n\n", lowerQ, limitsError[0], upperQ, limitsError[1]);
	}

	return (upperQ + lowerQ) / (double) 2;
}

double optimizerUnit(OptimizerDetails *optimizerDetails, mathFunc realFunction, mathFunc approximation){
	if(optimizerDetails == NULL || realFunction == NULL || approximation == NULL){
		return 0;
	}

	return optimizer(
		realFunction,
		approximation,
		optimizerDetails->integratorDetails.lowerLimit,
		optimizerDetails->integratorDetails.upperLimit,
		optimizerDetails->integratorDetails.steps,
		optimizerDetails->lowerQLimit,
		optimizerDetails->upperQLimit,
		optimizerDetails->maxIterQ
	);
}

void calculateRecpCorrectionFactor(){
	double realFunction(double x, double q){
		(void) q;
		return 1 / x;
	}

	double approximation1_sqrt2(double x, double qA){
		return 1 - (x-1) * (45 / (double) 64) - qA;
	}

	double approximationsqrt2_2(double x, double qB){
		return (sqrt(2) / (double) 2) - (x - sqrt(2)) * (45 / (double) 128) - qB;
	}

	OptimizerDetails optimizerDetails;
	
	optimizerDetails.lowerQLimit =  -1;
	optimizerDetails.upperQLimit =   1;
	optimizerDetails.maxIterQ 	 =  40;

	optimizerDetails.integratorDetails.steps	  = 8192;
	optimizerDetails.integratorDetails.lowerLimit = 1;
	optimizerDetails.integratorDetails.upperLimit = sqrt(2);

	double qOutput[2]		= {0, 0};

	qOutput[0] = optimizerUnit(&optimizerDetails, realFunction, approximation1_sqrt2);

	optimizerDetails.integratorDetails.lowerLimit = sqrt(2);
	optimizerDetails.integratorDetails.upperLimit = 2;
	qOutput[1] = optimizerUnit(&optimizerDetails, realFunction, approximationsqrt2_2);

/*
		float value	    fixed-point q32.32

	Output:
		qA: 0.017507 -> 0x00000000047B5223
		qB: 0.012380 -> 0x00000000032B4741
*/
	printf("Output:\n\tqA: %lf -> 0x%016lX\n\tqB: %lf -> 0x%016lX\n", qOutput[0], convertDoubleToUF64(qOutput[0]), qOutput[1], convertDoubleToUF64(qOutput[1]));
}

void calculateRSqrtCorrectionFactor(){
	double realFunction(double x, double q){
		(void) q;
		return 1 / sqrt(x);
	}

	double approximation1_2(double x, double qA){
		return 1 - (x-1) * (19 / (double) 64) - qA;
	}

	double approximation2_4(double x, double qB){
		return (sqrt(2) / (double) 2) - (x - 2) * (13 / (double) 128) - qB;
	}

	OptimizerDetails optimizerDetails;
	
	optimizerDetails.lowerQLimit =  -1;
	optimizerDetails.upperQLimit =   1;
	optimizerDetails.maxIterQ 	 =  40;

	optimizerDetails.integratorDetails.steps	  = 8192;
	optimizerDetails.integratorDetails.lowerLimit = 1;
	optimizerDetails.integratorDetails.upperLimit = 2;

	double qOutput[2]		= {0, 0};

	qOutput[0] = optimizerUnit(&optimizerDetails, realFunction, approximation1_2);

	optimizerDetails.integratorDetails.lowerLimit = 2;
	optimizerDetails.integratorDetails.upperLimit = 4;
	qOutput[1] = optimizerUnit(&optimizerDetails, realFunction, approximation2_4);

/*
		float value	    	  fixed-point q32.32

	Output:
		qA: 0.022436403319 -> 0x0000000005BE6462
		qB: 0.019640825470 -> 0x0000000005072E5F
*/
	printf("Output:\n\tqA: %.12lf -> 0x%016lX\n\tqB: %.12lf -> 0x%016lX\n", qOutput[0], convertDoubleToUF64(qOutput[0]), qOutput[1], convertDoubleToUF64(qOutput[1]));
}

void testIntegrator_sine(){
	IntegratorDetails integratorDetails;

	integratorDetails.lowerLimit 	= 0;
	integratorDetails.upperLimit 	= M_PI;
	integratorDetails.steps 		= 1024;
	integratorDetails.q				= 10;

	double f_sine(double x, double q){
		(void) q;
		return sin(x);
	}

	double value = integratorUnit(&integratorDetails, f_sine);
	printf("Int: %.8lf\n", value);
}

void calculateSqrtCorrectionFactor(){
	double realFunction(double x, double q){
		(void) q;
		return sqrt(x);
	}

	double approximation1_2(double x, double qA){
		return 1 + (x-1) * (53 / (double) 128) + qA;
	}

	double approximation2_4(double x, double qB){
		return sqrt(2) + (x - 2) * (75 / (double) 256) + qB;
	}

	OptimizerDetails optimizerDetails;
	
	optimizerDetails.lowerQLimit =  -1;
	optimizerDetails.upperQLimit =   1;
	optimizerDetails.maxIterQ 	 =  40;

	optimizerDetails.integratorDetails.steps	  = 8192;
	optimizerDetails.integratorDetails.lowerLimit = 1;
	optimizerDetails.integratorDetails.upperLimit = 2;

	double qOutput[2]		= {0, 0};

	qOutput[0] = optimizerUnit(&optimizerDetails, realFunction, approximation1_2);

	optimizerDetails.integratorDetails.lowerLimit = 2;
	optimizerDetails.integratorDetails.upperLimit = 4;
	qOutput[1] = optimizerUnit(&optimizerDetails, realFunction, approximation2_4);

/*
		float value	    	  fixed-point q32.32
	Output:dd
		qA: 0.011864289237 -> 0x00000000030989BE
		qB: 0.016617189394 -> 0x000000000441062D
*/
	printf("Output:\n\tqA: %.12lf -> 0x%016lX\n\tqB: %.12lf -> 0x%016lX\n", qOutput[0], convertDoubleToUF64(qOutput[0]), qOutput[1], convertDoubleToUF64(qOutput[1]));
}

