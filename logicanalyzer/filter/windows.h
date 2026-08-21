#ifndef LA_FILTER_WINDOWS
#define LA_FILTER_WINDOWS

#include <stddef.h>
#include <stdint.h>

typedef enum {
	LA_FILTER_WINDOW_RECTANGULAR		= 0,
	LA_FILTER_WINDOW_TRIANGULAR			= 1,
	LA_FILTER_WINDOW_WELCH				= 2,
	LA_FILTER_WINDOW_HANN				= 3,
	LA_FILTER_WINDOW_HAMMING			= 4,
	LA_FILTER_WINDOW_TRAPZ				= 5,
	LA_FILTER_WINDOW_CIRCULAR			= 6,
	LA_FILTER_WINDOW_SINC				= 7,
	LA_FILTER_WINDOW_IMPULSE			= 8,
	LA_FILTER_WINDOW_BLACKMAN			= 9,
	LA_FILTER_WINDOW_BLACKMAN_HARRIS	= 10,
	LA_FILTER_WINDOW_KAISER				= 11,
	LA_FILTER_WINDOW_GAUSSIAN			= 12,
	LA_FILTER_WINDOW_NUTALL				= 13,
	LA_FILTER_WINDOW_FLATTOP			= 14,
	LA_FILTER_WINDOW_PARZEN				= 15,
	LA_FILTER_WINDOW_COSINESUM			= 16,
	LA_FILTER_WINDOW_BLACKMAN_NUTALL	= 17,
	LA_FILTER_WINDOW_SINEPOWER			= 18,
	LA_FILTER_WINDOW_APPROX_GAUSSIAN	= 19,
	LA_FILTER_WINDOW_TUKEY				= 20,
	LA_FILTER_WINDOW_PLANK_TAPPER		= 21,
	LA_FILTER_WINDOW_POISSON			= 22,
	LA_FILTER_WINDOW_LANCZOS			= 23,
	LA_FILTER_WINDOW_NOISE				= 24,
	LA_FILTER_WINDOW_LOGISTICAL			= 25,
	LA_FILTER_WINDOW_LOGISTICAL_2		= 26,
	LA_FILTER_WINDOW_DAMPED				= 27,
	LA_FILTER_WINDOW_GAUSSINE			= 28,
	LA_FILTER_WINDOW_POLY_CHEBYSHEV		= 29,
	LA_FILTER_WINDOW_SMOOTH_TRAPZ		= 30,
	LA_FILTER_WINDOW_ROOT_CHEBYSHEV		= 31,
	LA_FILTER_WINDOW_COMPACT_SIN		= 32,
	LA_FILTER_WINDOW_MODIFIED_COSC		= 33
} LAFilterWindowType;

#define LA_FILTER_WINDOW_COUNT 34

LAErrorCode LAFilterGenerateWindowArray(double *buffer, size_t size, LAFilterWindowType windowType, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateRectWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateTriWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateWelchWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateHannWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateHammingWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateTrapzWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateCircWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateSincWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateImpulseWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateBlackmanWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateBlackmanHarrisWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateKaiserWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateGaussianWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateNutallWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateFlattopWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateParzenWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateCosineSumWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateBlackmanNutallWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateSinePowerWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateApproxGaussianWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateTukeyWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGeneratePlankTapperWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGeneratePoissonWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateLanczosWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateNoiseWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateLogisticalWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateLogistical2Window(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateDampedWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateGaussineWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGeneratePolyChebyshevWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateSmoothTrapezoidalWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateRootChebyshevSmoothWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateCompactSineWindow(double *array, size_t size, double *params, size_t paramCount);
LAErrorCode LAFilterGenerateModifiedCoscWindow(double *array, size_t size, double *params, size_t paramCount);

#endif //LA_FILTER_WINDOWS
