#ifndef LIB_LOGIC_ANALYZER_DATA_LOADER
#define LIB_LOGIC_ANALYZER_DATA_LOADER

#include "../liblogicanalyzer.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define CLAMP_GRAPH_WIDTH 384
#define CLAMP_GRAPH_HEIGHT 192
#define CLAMP_INFO_GRAPH_WIDTH 384
#define CLAMP_INFO_GRAPH_HEIGHT 64

typedef enum {
	LA_CLAMP_INST_RAMP		= 0,
	LA_CLAMP_INST_IMPULSE	= 1,
	LA_CLAMP_INST_STEP		= 2,
	LA_CLAMP_INST_SINE		= 3,
	LA_CLAMP_INST_PULSE		= 4,
	LA_CLAMP_INST_NOISE		= 5
} LAClampInst;

#define LA_CLAMP_PARAMETER_COUNT 5
#define LA_CLAMP_TYPE_COUNT 36

typedef enum {
	LA_CLAMP_FLAG_SLOW = 0,
	LA_CLAMP_FLAG_FAST = 1
} LAClampFlags;

typedef struct {
	char *name;
	LAFilterWindowParameter param[LA_CLAMP_PARAMETER_COUNT];
	LAClampFlags flags;
	LAErrorCode (*callable)(double *, size_t, double *, size_t, double *, size_t);
} LADataLoaderClampType;

extern LADataLoaderClampType LAClampDetails[LA_CLAMP_TYPE_COUNT];

#define LA_NORM_PARAMETER_COUNT 3
#define LA_NORM_TYPE_COUNT 6

typedef struct {
	char *name;
	LAFilterWindowParameter param[LA_NORM_PARAMETER_COUNT];
	LAErrorCode (*callable)(double *, size_t, double *, size_t);
} LADataLoaderNormType;

extern LADataLoaderNormType LANormDetails[LA_NORM_TYPE_COUNT];

typedef struct {
	GtkWidget *window;
	GtkWidget *content;
	GtkWidget *vbox;
	GtkWidget *hbox;

	GtkWidget *frameSource;
	GtkWidget *sourceVbox;
	GtkWidget *sourceRadioCircBuffer;
	GtkWidget *sourceRadioFile;
	GtkWidget *sourceHboxFile;
	GtkWidget *sourceOpenFileButton;
	GtkWidget *sourceFileLabel;

	GtkWidget *frameData;
	GtkWidget *dataVbox;
	GtkWidget *dataLabel;
	GtkWidget *dataComboBox;
	GtkWidget *endianLabel;
	GtkWidget *endianComboBox;
	GtkWidget *dataGrid;
	GtkWidget *fileSizeLabel;
	GtkWidget *fileSizeValue;
	GtkWidget *sampleCountLabel;
	GtkWidget *sampleCountValue;
	GtkWidget *dataRecalculate;
	LALabelSpinCombo offset;
	LALabelSpinCombo sampleCount;

	GtkWidget *frameClamp;
	GtkWidget *clampHbox;
	GtkWidget *clampVboxL;
	GtkWidget *clampVboxR;
	GtkWidget *clampGraph;
	GtkWidget *clampLabel;
	GtkWidget *clampComboBox;
	GtkWidget *clampInstLabel;
	GtkWidget *clampInstComboBox;
	LALabelSpinCombo clampParam[LA_CLAMP_PARAMETER_COUNT];
	LALabelSpinCombo clampGain;
	GtkWidget *clampDFTLabel;
	GtkWidget *clampGraphDFT;
	GtkWidget *clampGraphInfo;

	GtkWidget *frameNorm;
	GtkWidget *normVbox;
	GtkWidget *normLabel;
	GtkWidget *normComboBox;
	LALabelSpinCombo normParam[LA_NORM_PARAMETER_COUNT];

	GtkWidget *frameMisc;
	GtkWidget *miscVbox;

} LADataLoaderWindow;

// dataloader.c
LAErrorCode LAOpenDataLoader(LADataLoaderWindow *lad, LAWindow *law);

// format.c
LAErrorCode LADataLoaderComboBoxFiller(GtkWidget *widget);

// Endianness
LAErrorCode LADataLoaderComboBoxEndiannessFiller(GtkWidget *widget);

// norm.c
LAErrorCode LADataLoaderComboBoxNormFiller(GtkWidget *widget);
gboolean LAOnDataLoaderNormChanged(GtkWidget *widget, LADataLoaderWindow *lad);

// clamp.c
LAErrorCode LAClampEvaluate(double *x, size_t sizeX, double *y, size_t sizeY, size_t clampType, double *params, size_t paramCount);
LAErrorCode LADataLoaderComboBoxClampFiller(GtkWidget *widget);
gboolean LAOnDataLoaderClampChanged(GtkWidget *widget, LADataLoaderWindow *lad);
gboolean LAOnDataLoaderClampDraw(GtkWidget *widget, cairo_t *cr, LADataLoaderWindow *lad);
gboolean LAOnDataLoaderClampDFTDraw(GtkWidget *widget, cairo_t *cr, LADataLoaderWindow *lad);
gboolean LAOnDataLoaderClampInfoDraw(GtkWidget *widget, cairo_t *cr, LADataLoaderWindow *lad);

// clampfuncs.c
LAErrorCode LAClampNoClamping(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampHardClipping(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampSoftClipping(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampHardWavefolding(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampSoftWavefolding(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampModulo(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampSoftSign(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampCubicClamping(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampSimpleFoldover(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampDoubleCuspFold(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampExponentialFold(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampPartialWrap(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampSymmetricalLogistic(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampSCurve(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampExponentialGate(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampSoftKnee(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampSaturatedLogisticMap(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampTentMap(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampBJTSaturation(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampSlewRateSaturation(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampSchmittTrigger(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampRCLowPass(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampSymmetricalDiode(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampUnilateralDiode(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampDifferenciator(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampIntegrator(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampStochastic(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampEvenPowerLimiter(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampEvenPowerFolder(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampNroot(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampRootSine(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampSoftAbs(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampHardGauss1(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampHardGauss2(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampBitCrush(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);
LAErrorCode LAClampTransientLimiter(double *x, size_t sizeX, double *y, size_t sizeY, double *params, size_t paramCount);

// clampInst.c
gboolean LAOnDataLoaderClampInstChanged(GtkWidget *widget, LADataLoaderWindow *lad);
LAErrorCode LADataLoaderComboBoxClampInstFiller(GtkWidget *widget);
LAErrorCode LAClampInstCreate(double *x, size_t size, double min, double max, size_t inst);
LAErrorCode LAClampInstImpulse(double *x, size_t size, double min, double max);
LAErrorCode LAClampInstStep(double *x, size_t size, double min, double max);
LAErrorCode LAClampInstSine(double *x, size_t size, double min, double max);
LAErrorCode LAClampInstPulse(double *x, size_t size, double min, double max);
LAErrorCode LAClampInstNoise(double *x, size_t size, double min, double max);

// clampfuncs.f90
extern LAErrorCode LACLAMP_NO_CLAMP(double *, size_t *, double *, size_t *);
extern LAErrorCode LACLAMP_HARD_CLIP(double *, size_t *, double *, size_t *, double , double );
extern LAErrorCode LACLAMP_SOFT_CLIP(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_HARD_FOLD(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_SOFT_FOLD(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_MODULO(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_SOFT_SIGN(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_CUBIC_CLAMP(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_SIMPLE_FOLDOVER(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_DOUBLE_CUSP_FOLD(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_EXPONENTIAL_FOLD(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_PARTIAL_WRAP(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_SYMMETRICAL_LOGISTIC(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_S_CURVE(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_EXPONENTIAL_GATE(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_SOFT_KNEE(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_SATURATED_LOGISTIC_MAP(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_TENT_MAP(double *, size_t *, double *, size_t *, double, double, double, double);
extern LAErrorCode LACLAMP_BJT_SATURATION(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_SLEW_RATE_SATURATION(double *, size_t *, double *, size_t *, double, double);
extern LAErrorCode LACLAMP_SCHMITT_TRIGGER(double *, size_t *, double *, size_t *, double, double, double, double);
extern LAErrorCode LACLAMP_RC_LOW_PASS(double *, size_t *, double *, size_t *, double, double, double, double, double);
extern LAErrorCode LACLAMP_SYMMETRICAL_DIODE(double *, size_t *, double *, size_t *, double, double, double, double);
extern LAErrorCode LACLAMP_UNILATERAL_DIODE(double *, size_t *, double *, size_t *, double, double, double, double);
extern LAErrorCode LACLAMP_DIFFERENTIATOR(double *, size_t *, double *, size_t *, double, double);
extern LAErrorCode LACLAMP_INTEGRATOR(double *, size_t *, double *, size_t *, double, double);
extern LAErrorCode LACLAMP_STOCHASTIC(double *, size_t *, double *, size_t *, double, double);
extern LAErrorCode LACLAMP_EVEN_POWER_LIMITER(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_EVEN_POWER_FOLDOVER(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_NROOT(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_ROOT_SINE(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_SOFT_ABS(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_HARD_GAUSS_1(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_HARD_GAUSS_2(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_BIT_CRUSH(double *, size_t *, double *, size_t *, double, double, double);
extern LAErrorCode LACLAMP_TRANSIENT_LIMITER(double *, size_t *, double *, size_t *, double, double, double, double, double);

#endif // LIB_LOGIC_ANALYZER_DATA_LOADER
