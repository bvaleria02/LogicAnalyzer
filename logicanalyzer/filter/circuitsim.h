#ifndef LA_FILTER_CIRCUIT_SIM
#define LA_FILTER_CIRCUIT_SIM

#include "filter.h"
#include "../liblogicanalyzer.h"
#include <stdlib.h>

typedef struct{
	double rc;
	double vc0;

	size_t i;
	double *vs;
	size_t vsSize;
	double *y;
	size_t ySize;
	double inputGain;
} LASolverParameterRC;

typedef struct{
	double dx[2];
	size_t index;

	double *vs;
	size_t vsSize;
	double *y;
	size_t ySize;

	double a3;
	double b3;
	double slewRate;
	size_t i;
	double inputGain;
} LASolverParameterSK;

typedef enum {
	LA_CS_PARAM_R1			= 0,
	LA_CS_PARAM_R2			= 1,
	LA_CS_PARAM_R3			= 2,
	LA_CS_PARAM_RH			= 3,
	LA_CS_PARAM_RL			= 4,
	LA_CS_PARAM_C1			= 5,
	LA_CS_PARAM_C2			= 6,
	LA_CS_PARAM_C3			= 7,
	LA_CS_PARAM_V0C1		= 8,
	LA_CS_PARAM_V0C2		= 9,
	LA_CS_PARAM_V0C3		= 10,
	LA_CS_PARAM_SLEWRATE	= 11,
	LA_CS_PARAM_SAMPLERATE	= 12,
	LA_CS_PARAM_INPUTGAIN	= 13,
	LA_CS_PARAM_OUTPUTGAIN	= 14
} LACircuitSimParam;

LAErrorCode LAFilterCircuitSimSolve(double *x, size_t nx, double *y, size_t ny, LAFilterValue *value);

double LASolverRCFunc(double t, double vc, void *ptr, double blend);
double LASolverSKFunc(double t, double vc, void *ptr, double blend);

#endif //LA_FILTER_CIRCUIT_SIM
