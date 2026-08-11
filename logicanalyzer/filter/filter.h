#ifndef LA_FILTER
#define LA_FILTER

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
	char *name;
	double value;
	double min;
	double max;
	double stepIncrement;
	double pageIncrement;
	double digits;
} LAFilterWindowParameter;

typedef struct {
	char *name;
	LAFilterWindowParameter param[LA_FIR_FILTER_PARAMS];
	LAErrorCode (*callable)(double *, size_t, double *, size_t);
} LAFilterWindowDetails;

extern LAFilterWindowDetails LAWindowTypeDetails[LA_FILTER_WINDOW_COUNT];

#define LA_IIR_LENGTH_A 3
#define LA_IIR_LENGTH_B 3
#define LA_CS_COUNT 15

typedef struct {
	uint8_t filterType;

	uint8_t FIRFilterSize;
	uint8_t FIRFilterType;
	double	FIRFrequency;
	uint8_t FIRWindowType;
	double 	FIRParam1;
	double 	FIRParam2;
	double 	FIRParam3;

	double 	IIRa[LA_IIR_LENGTH_A];
	double 	IIRb[LA_IIR_LENGTH_B];

	uint64_t MALength;

	uint8_t csType;
	double csParam[LA_CS_COUNT];
	uint8_t csSolver;

	size_t cfgTest;
	bool cfgIn;
	bool cfgOut;
	bool cfgNorm;
} LAFilterValue;

#endif //LA_FILTER
