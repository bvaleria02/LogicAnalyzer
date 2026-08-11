#ifndef FIXED_CALCULATIONS_H
#define FIXED_CALCULATIONS_H

#include <stdint.h>

typedef double (*mathFunc)(double, double);

typedef struct{
	double lowerLimit;
	double upperLimit;
	uint64_t steps;
	double q;
} IntegratorDetails;

typedef struct{
	IntegratorDetails integratorDetails;
	double lowerQLimit;
	double upperQLimit;
	uint8_t maxIterQ;
} OptimizerDetails;

double integrator(mathFunc func, double param, double lowerLimit, double upperLimit, uint64_t steps);
double optimizer(mathFunc realFunction, mathFunc approximation, double lowerIntegrationLimit, double upperIntegrationLimit, uint64_t integrationSteps, double lowerQLimit, double upperQLimit, uint8_t maxIterQ);

double integratorUnit(IntegratorDetails *integratorDetails, mathFunc func);
double optimizerUnit(OptimizerDetails *optimizerDetails, mathFunc realFunction, mathFunc approximation);

void testIntegrator_sine();
void calculateRecpCorrectionFactor();
void calculateRSqrtCorrectionFactor();
void calculateSqrtCorrectionFactor();

#endif // FIXED_CALCULATIONS_H
