#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "liblogicanalyzer.h"
#include "compiler.h"
#include "fixedpoint/fixedpoint.h"
#include <math.h>
#include "numericMethods/dft.h"
#include "numericMethods/fft.h"
#include "numericMethods/normalize.h"
#include "numericMethods/convolve.h"
#include <stdbool.h>
#include "pzmap/pzmap.h"
#include "laComplex/laComplex.h"
#include "filter/circuitsim.h"
#include "draw/fftfreq.h"

#define LA_DFT_BORDER 48
#define LA_CS_TYPES 6

typedef struct {
	const char *name;
	double min;
	double max;
	double value;
	double inc;
	double incSmall;
	size_t decimal;
} LACircuitSimDefault;

const LACircuitSimDefault LACSParams[LA_CS_COUNT] = {
	{"R1", 			0.0,	1e9,	1e3,	1.0,	0.1,	2},
	{"R2", 			0.0,	1e9,	1e3,	1.0,	0.1,	2},
	{"R3", 			0.0,	1e9,	1e3,	1.0,	0.1,	2},
	{"RH", 			0.0,	1e9,	1e4,	1.0,	0.1,	2},
	{"RL", 			0.0,	1e9,	1e4,	1.0,	0.1,	2},
	{"C1", 			1e-12,	1e6,	1e-6,	1e-6,	1e-12,	12},
	{"C2", 			1e-12,	1e6,	1e-6,	1e-6,	1e-12,	12},
	{"C3", 			1e-12,	1e6,	1e-6,	1e-6,	1e-12,	12},
	{"V0 C1", 		-1e9,	1e9,	1e-2,	1,		1e-2,	3},
	{"V0 C2", 		-1e9,	1e9,	1e-2,	1,		1e-2,	3},
	{"V0 C3", 		-1e9,	1e9,	1e-2,	1,		1e-2,	3},
	{"Slew rate", 	0.0,	1e3,	1.0,	1e-3,	1e-6,	6},
	{"Sample rate", 0.0,	1e12,	44100,	1,		1e-3,	3},
	{"Input gain",  -1e6,	1e6,	1.0,	1,		1e-3,	3},
	{"Output gain",  -1e6,	1e6,	1.0,	1,		1e-3,	3},
};

bool LACSParamEnable[LA_CS_TYPES][LA_CS_COUNT] = {
	{true,	false,	false,	false,	false,	true,	false,	false,	true,	false,	false,	false,	true, 	true, 	true},
	{true,	false,	false,	false,	false,	true,	false,	false,	true,	false,	false,	false,	true, 	true, 	true},
	{true,	true,	false,	true, 	true, 	true,	true,	false,	true,	true,	false,	true,	true, 	true, 	true},
	{true,	true,	false,	true, 	true, 	true,	true,	false,	true,	true,	false,	true,	true, 	true, 	true},
	{true,	true,	true,	true, 	true, 	true,	true,	false,	true,	true,	false,	true,	true, 	true, 	true},
	{true,	true,	true,	true, 	true, 	true,	true,	true,	true,	true,	true,	true,	true, 	true, 	true}
};

typedef struct {
	GtkWidget *window;
	GtkWidget *content;
	GtkWidget *vbox;

	GtkWidget *hboxMain;
	GtkWidget *mainTypeLabel;
	GtkWidget *mainTypeSelector;
	GtkWidget *mainSizeLabel;
	GtkWidget *mainSizeSelector;
	GtkWidget *mainOpenPreset;
	GtkWidget *mainPresetName;

	GtkWidget 		*hbox;
	GtkWidget 		*hbox2;
	GtkWidget 		*hboxFIR;
	GtkWidget 		*vboxFIR1;
	GtkWidget 		*vboxFIR2;
	GtkWidget 		*firLabel;
	GtkWidget 		*firTypeLabel;
	GtkWidget 		*firTypeSelector;
	GtkWidget 		*firFreqLabel;
	GtkAdjustment 	*firFreqAdj;
	GtkWidget 		*firFreqSB;
	GtkWidget 		*firFreqGraph;
	GtkWidget 		*firWindowLabel;
	GtkWidget 		*firWindowSelector;
	GtkWidget 		*firWindowGraph;
	GtkWidget 		*firParam1Label;
	GtkAdjustment 	*firParam1Adj;
	GtkWidget 		*firParam1SB;
	GtkWidget 		*firParam2Label;
	GtkAdjustment 	*firParam2Adj;
	GtkWidget 		*firParam2SB;
	GtkWidget 		*firParam3Label;
	GtkAdjustment 	*firParam3Adj;
	GtkWidget 		*firParam3SB;
	LAFilterWindowWidget windowWidget;

	GtkWidget 		*vboxIIR;
	GtkWidget 		*hboxIIR;
	GtkWidget 		*vboxIIR1;
	GtkWidget 		*vboxIIR2;
	GtkWidget 		*iirLabel;
	GtkWidget 		*iirAHbox[LA_IIR_LENGTH_A];
	GtkWidget 		*iirALabel[LA_IIR_LENGTH_A];
	GtkAdjustment 	*iirAAdj[LA_IIR_LENGTH_A];
	GtkWidget 		*iirASB[LA_IIR_LENGTH_A];
	GtkWidget 		*iirBHbox[LA_IIR_LENGTH_A];
	GtkWidget 		*iirBLabel[LA_IIR_LENGTH_B];
	GtkAdjustment 	*iirBAdj[LA_IIR_LENGTH_B];
	GtkWidget 		*iirBSB[LA_IIR_LENGTH_B];
	GtkWidget 		*iirFormula;
	GtkWidget 		*iirPZMap;

	GtkWidget 		*vboxMA;
	GtkWidget 		*maLabel;
	GtkWidget 		*maLengthLabel;
	GtkAdjustment 	*maLengthAdj;
	GtkWidget 		*maLengthSB;

	GtkWidget 		*vboxCS;
	GtkWidget 		*csLabel;
	GtkWidget 		*csType;
	GtkWidget 		*csHbox[LA_CS_COUNT];
	GtkWidget 		*csParLabel[LA_CS_COUNT];
	GtkWidget 		*csParSB[LA_CS_COUNT];
	GtkAdjustment 	*csParAdj[LA_CS_COUNT];
	GtkWidget 		*csSimLabel;
	GtkWidget 		*csSimType;

	GtkWidget 		*prevNote;
	GtkWidget 		*vboxIR;
	GtkWidget 		*irLabel;
	GtkWidget 		*irGraph;
	GtkWidget 		*irLabel2;
	GtkWidget 		*irDFT;

	GtkWidget 		*vboxCfg;
	GtkWidget 		*cfgLabel;
	GtkWidget 		*cfgTest;
	GtkWidget 		*cfgIn;
	GtkWidget 		*cfgOut;
	GtkWidget 		*cfgNorm;

	double			irData[LA_BUFFER_SIZE];
	double			irDataIn[LA_BUFFER_SIZE];
	size_t			irDataSize;
	LAFilterValue value;
	bool 			uiReady;

} LAFilterWindow;

typedef enum {
	LA_TAG_FILTER_TYPE				=	0x0010,
	LA_TAG_FILTER_FIR_SIZE			=	0x0021,
	LA_TAG_FILTER_FIR_TYPE			=	0x0022,
	LA_TAG_FILTER_FIR_FREQUENCY		=	0x0023,
	LA_TAG_FILTER_FIR_WINDOW_TYPE	=	0x0024,
	LA_TAG_FILTER_FIR_PARAM1_VALUE	=	0x0025,
	LA_TAG_FILTER_FIR_PARAM2_VALUE	=	0x0026,
	LA_TAG_FILTER_FIR_PARAM3_VALUE	=	0x0027,
	LA_TAG_FILTER_IIR_A0			=	0x0040,
	LA_TAG_FILTER_IIR_A1			=	0x0041,
	LA_TAG_FILTER_IIR_A2			=	0x0042,
	LA_TAG_FILTER_IIR_A3			=	0x0043,
	LA_TAG_FILTER_IIR_B1			=	0x0051,
	LA_TAG_FILTER_IIR_B2			=	0x0052,
	LA_TAG_FILTER_IIR_B3			=	0x0053,
	LA_TAG_FILTER_MA_LENGTH			=	0x0061
} LA_TAG_FILTER;

LAErrorCode LAFilterValueInit(LAFilterValue *laf){
	LA_HANDLE_NULLPTR(laf, LA_PROPAGATE_ERROR);

	laf->filterType = 0;
	laf->FIRFilterSize = 0;
	laf->FIRFilterType = 0;
	laf->FIRFrequency = 1.0;
	laf->FIRParam1 = 0.0;
	laf->FIRParam2 = 0.0;
	laf->FIRParam3 = 0.0;

	for(uint8_t i = 0; i < LA_IIR_LENGTH_A; i++) laf->IIRa[i] = 0.0;
	for(uint8_t i = 0; i < LA_IIR_LENGTH_B; i++) laf->IIRb[i] = 0.0;

	laf->MALength = 2;

	laf->csType = 0;
	for(uint8_t i = 0; i < LA_CS_COUNT; i++) laf->csParam[i] = LACSParams[i].value;
	laf->csSolver = 2;

	laf->cfgTest = 0;
	laf->cfgIn = false;
	laf->cfgOut = true;
	laf->cfgNorm = false;

	return LA_NO_ERROR;
}

LAErrorCode LAFilterCopyValue(LAFilterWindow *laf, LAPresetFile *preset){
	LA_HANDLE_NULLPTR(laf, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(preset, LA_PROPAGATE_ERROR);

	LAPFDataNode *node = preset->dataStart;

	// No data, preset is blank, valid
	if(node == NULL) return LA_NO_ERROR;

	uint64_t rawValue = 0;
	while(node != NULL){
		rawValue = LAParseIntFromArray(node->value, node->length);
		if(node->tag == LA_TAG_END) break;

		switch(node->tag){
			case LA_TAG_FILTER_TYPE:				laf->value.filterType = rawValue;
													break;

			case LA_TAG_FILTER_FIR_SIZE:			laf->value.FIRFilterSize = rawValue;
													break;
			case LA_TAG_FILTER_FIR_TYPE:			laf->value.FIRFilterType = rawValue;
													break;
			case LA_TAG_FILTER_FIR_FREQUENCY:		laf->value.FIRFrequency = convertF32ToDouble(rawValue);
													break;
			case LA_TAG_FILTER_FIR_WINDOW_TYPE:		laf->value.FIRWindowType = rawValue;
													break;
			case LA_TAG_FILTER_FIR_PARAM1_VALUE:	laf->value.FIRParam1 = convertF32ToDouble(rawValue);
													break;
			case LA_TAG_FILTER_FIR_PARAM2_VALUE:	laf->value.FIRParam2 = convertF32ToDouble(rawValue);
													break;
			case LA_TAG_FILTER_FIR_PARAM3_VALUE:	laf->value.FIRParam3 = convertF32ToDouble(rawValue);
													break;

			case LA_TAG_FILTER_IIR_A0:				laf->value.IIRa[0] = convertF32ToDouble(rawValue);
													break;
			case LA_TAG_FILTER_IIR_A1:				laf->value.IIRa[1] = convertF32ToDouble(rawValue);
													break;
			case LA_TAG_FILTER_IIR_A2:				laf->value.IIRa[2] = convertF32ToDouble(rawValue);
													break;
			case LA_TAG_FILTER_IIR_A3:				laf->value.IIRa[3] = convertF32ToDouble(rawValue);
													break;
			case LA_TAG_FILTER_IIR_B1:				laf->value.IIRb[0] = convertF32ToDouble(rawValue);
													break;
			case LA_TAG_FILTER_IIR_B2:				laf->value.IIRb[1] = convertF32ToDouble(rawValue);
													break;
			case LA_TAG_FILTER_IIR_B3:				laf->value.IIRb[2] = convertF32ToDouble(rawValue);
													break;

			case LA_TAG_FILTER_MA_LENGTH:			laf->value.MALength = rawValue;
													break;

			default:								// TAG is invalid or nop, skipped
		}

		node = node->next;
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterUpdateUI(LAFilterWindow *laf){
	LA_HANDLE_NULLPTR(laf, LA_PROPAGATE_ERROR);

	gtk_combo_box_set_active(GTK_COMBO_BOX(laf->mainTypeSelector), laf->value.filterType);

	gtk_combo_box_set_active(GTK_COMBO_BOX(laf->mainSizeSelector), 	laf->value.FIRFilterSize);
	gtk_combo_box_set_active(GTK_COMBO_BOX(laf->firTypeSelector), 	laf->value.FIRFilterType);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(laf->firFreqSB), laf->value.FIRFrequency);
	/*
	gtk_combo_box_set_active(GTK_COMBO_BOX(laf->firWindowSelector), laf->value.FIRWindowType);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(laf->firParam1SB), laf->value.FIRParam1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(laf->firParam2SB), laf->value.FIRParam2);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(laf->firParam3SB), laf->value.FIRParam3);
*/
	for(uint8_t i = 0; i < LA_IIR_LENGTH_A; i++){
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(laf->iirASB[i]), laf->value.IIRa[i]);
	}

	for(uint8_t i = 0; i < LA_IIR_LENGTH_B; i++){
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(laf->iirBSB[i]), laf->value.IIRb[i]);
	}

	gtk_spin_button_set_value(GTK_SPIN_BUTTON(laf->maLengthSB), laf->value.MALength);

	gtk_widget_queue_draw(laf->firFreqGraph);
	//gtk_widget_queue_draw(laf->firWindowGraph);
	gtk_widget_queue_draw(laf->irGraph);
	return LA_NO_ERROR;
}

void LAFilterPreseteOpen(GtkWidget *widget, LAFilterWindow *laf){
	bool isFilenameValid 	= FALSE;
	bool isFileMapped 		= FALSE;
	bool isPresetParsed 	= FALSE;
	LAErrorCode code 		= LA_NO_ERROR;
	gchar *filename 		= NULL;
	LAMappedFile file;

	filename = LADialogOpenFile(lawp, "Open LA Preset File", ".lapf");
	if(filename == NULL) goto cleanup;
	isFilenameValid = TRUE;

	code = LAMemoryMapFile(filename, &(file));
	if(code) goto cleanup;
	isFileMapped = TRUE;

	uint8_t flags = 0;
	LAPresetFile preset;
	LAPresetFileInit(&preset);
	code = LAPresetParseFile(&preset, &file, &flags);
	if(code) goto cleanup;
	isPresetParsed = TRUE;

	code = LAFilterCopyValue(laf, &preset);
	if(code) goto cleanup;

	gtk_label_set_text(GTK_LABEL(laf->mainPresetName), (gchar *)preset.header.name);

	code = LAFilterUpdateUI(laf);
	if(code) goto cleanup;
	(void) widget;

cleanup:
	if(code)			LADialogErrorGeneric(lawp, "Error reading file");
	if(isPresetParsed)	LADeletePresetDataNodes(&(preset.dataStart));
	if(isFilenameValid)	g_free(filename);
	if(isFileMapped)	LAMemoryUnmapFile(&file);
}

LAErrorCode LAFilterGraphDrawBG(cairo_t *cr, double width, double height, bool midLine){
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
	cairo_move_to(cr, 0, 		height / 2);
	cairo_line_to(cr, width, 	height / 2);
	cairo_stroke(cr);

	if(midLine){
		cairo_move_to(cr, width / 2, 0);
		cairo_line_to(cr, width / 2, height);
		cairo_stroke(cr);
	}

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, 1);
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_stroke(cr);

	return LA_NO_ERROR;
}

LAErrorCode LAFilterFIRSizeToSize(LAFilterValue *laf, size_t *size){
	LA_HANDLE_NULLPTR(laf, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(size, LA_PROPAGATE_ERROR);

	(*size) = pow(2, laf->FIRFilterSize + 1);
	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateFIRBuffer(LAFilterValue *laf, double **buffer, size_t *size){
	LA_HANDLE_NULLPTR(laf, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(buffer, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(size, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	code = LAFilterFIRSizeToSize(laf, size);
	if(code) return code;

	(*buffer) = (double *)malloc((*size) * sizeof(double));
	if((*buffer) == NULL){
		LA_RAISE_ERROR(LA_ERROR_MALLOC);
		return LA_ERROR_MALLOC;
	}
	
	return LA_NO_ERROR;
}

LAErrorCode LAFilterGenerateFIRSinc(double *buffer, size_t size, double f, int mode){
	LA_HANDLE_NULLPTR(buffer, LA_PROPAGATE_ERROR);

	double t0 = size / (double) 2;
	double t  = 0;

	for(size_t i = 0; i < size; i++){
		t = f * (i - t0);
		if(fabs(t) < LA_EPS){
			buffer[i] = 1;
		} else {
			buffer[i] = sin(t) / (double) t;
		}

		if(mode == 1 && i != t0){
			buffer[i] *= -1;
		}
	}

	return LA_NO_ERROR;
}
/*
LAErrorCode LAFilterGraphDrawArray(cairo_t *cr, double *array, size_t size, double width, double height, double min, double max, bool changeColor){
	LA_HANDLE_NULLPTR(cr, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);
	if(min >= max){
		LA_RAISE_ERROR(LA_ERROR_INCORRECTVALUE);
		return LA_ERROR_INCORRECTVALUE;
	}
	
	double x = 0;
	double y = 0;
	if(changeColor) cairo_set_source_rgb(cr, 0.8, 0.4, 0.2);
	cairo_set_line_width(cr, 2);

	for(size_t i = 0; i < size; i++){
		if(size > 1){
			x = (i / (double) (size - 1)) * width;
		} else {
			x = width;
		}

		y = (array[i] - min) / (double) (max - min);
		y = height / (double) 2 - (y - 0.5) * (height * 0.9);

		if(i == 0){
			cairo_move_to(cr, 0, y);
		}
		cairo_line_to(cr, x, y);

	}

	cairo_stroke(cr);
	return LA_NO_ERROR;
}
*/
gboolean LAOnDrawFilterFreqGraph(GtkWidget *widget, cairo_t *cr, LAFilterWindow *laf){
	LA_HANDLE_NULLPTR(widget, TRUE);
	LA_HANDLE_NULLPTR(cr, TRUE);
	LA_HANDLE_NULLPTR(laf, TRUE);
	LAErrorCode code = LA_NO_ERROR;

	code = LAFilterGraphDrawBG(cr, GRAPH_MINI_WIDTH, GRAPH_MINI_HEIGHT, FALSE);
	if(code) return TRUE;

	double *buffer = NULL;
	size_t size = 0;

	code = LAFilterGenerateFIRBuffer(&(laf->value), &buffer, &size);
	if(code) return TRUE;

	code = LAFilterGenerateFIRSinc(buffer, size, laf->value.FIRFrequency, laf->value.FIRFilterType);
	if(code) return TRUE;

	code = LAFilterGraphDrawArray(cr, buffer, size, GRAPH_MINI_WIDTH, GRAPH_MINI_HEIGHT, -1.0, 1.0, TRUE);
	if(code) return TRUE;
	
	if(buffer != NULL) free(buffer);
	return TRUE;
}


LAErrorCode LAFilterIRArrayInit(double *array, size_t size, size_t type){
	LA_HANDLE_NULLPTR(array, LA_PROPAGATE_ERROR);

	for(size_t i = 0; i < size; i++){
		switch(type){
			case 0:	array[i] = (i == 0) ? 1 : 0;
					break;
			case 1: array[i] = 1;
					break;
			case 2: array[i] = 2*(rand() / (double) RAND_MAX) - 1;
					break;
			case 3: array[i] = ((i & 0xFF) < 0x80) ? 1 : 0;
					break;
			case 4: array[i] = sin((i*2*M_PI) / 128);
					break;
			case 5: array[i] = 2*((i & 0xFF) / (double) 256.0) - 1;
					break;
		}
	}

	return LA_NO_ERROR;
}

LAErrorCode LAFilterCompute(double *x, size_t nx, double *y, size_t ny, LAFilterValue *laf, LAFilterWindowWidget *windowWidget){
	LA_HANDLE_NULLPTR(x, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(y, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(laf, 	LA_PROPAGATE_ERROR);

	double *xFIR;
	size_t nFIR = 0;;
	double *hFIR;
	LAErrorCode code = LA_NO_ERROR;

	switch(laf->filterType){
		case 0:		
				code = LAFilterGenerateFIRBuffer(laf, &xFIR, &nFIR);
				if(code) return TRUE;
				code = LAFilterGenerateFIRBuffer(laf, &hFIR, &nFIR);
				if(code) return TRUE;

				code = LAFilterGenerateFIRSinc(xFIR, nFIR, laf->FIRFrequency, laf->FIRFilterType);
				if(code) return TRUE;
				code = LAFilterGenerateWindowArray(hFIR, nFIR, windowWidget->windowType, windowWidget->paramValue, LA_FIR_FILTER_PARAMS);
				if(code) return TRUE;

				for(size_t n = 0; n < nFIR; n++)	hFIR[n] = xFIR[n] * hFIR[n];

				code = LAConvolveArray(x, nx, hFIR, nFIR, y, ny);
				if(code) return TRUE;

				if(xFIR != NULL) free(xFIR);
				if(hFIR != NULL) free(hFIR);
				break;

		case 1:
				code = LAFilterIIRCompile(x, nx, y, ny, laf->IIRa, LA_IIR_LENGTH_A, laf->IIRb, LA_IIR_LENGTH_B);
				if(code) return TRUE;
				break;

		case 2:
				code = LAFilterMovingAverageCompile(x, nx, y, ny, laf->MALength);
				if(code) return TRUE;
				break;

		case 3: 
				code = LAFilterCircuitSimSolve(x, nx, y, ny, laf);
				if(code) return TRUE;
				break;
	}

	return LA_NO_ERROR;
}

gboolean LAOnDrawFilterIRGraph(GtkWidget *widget, cairo_t *cr, LAFilterWindow *laf){
	LA_HANDLE_NULLPTR(widget, TRUE);
	LA_HANDLE_NULLPTR(cr, TRUE);
	LA_HANDLE_NULLPTR(laf, TRUE);
	LAErrorCode code = LA_NO_ERROR;

	code = LAFilterGraphDrawBG(cr, GRAPH_WIDTH, GRAPH_HEIGHT, FALSE);
	if(code) return TRUE;

	if(laf->value.cfgIn){
		cairo_set_source_rgb(cr, 0.3, 0.5, 1);
		code = LAFilterGraphDrawArray(cr, laf->irDataIn, laf->irDataSize, GRAPH_WIDTH, GRAPH_HEIGHT, -2.0, 2.0, FALSE);
		if(code) return TRUE;
	}

	if(laf->value.cfgOut){
		code = LAFilterIRArrayInit(laf->irDataIn, laf->irDataSize, laf->value.cfgTest);
		if(code) return TRUE;
		code = LAFilterCompute(laf->irDataIn, laf->irDataSize, laf->irData, laf->irDataSize, &(laf->value), &(laf->windowWidget));
		if(code) return TRUE;
		if(laf->value.cfgNorm){
			code = LANormalizeArrayMinMax(laf->irData, laf->irDataSize);
			if(code) return TRUE;
		}
		code = LAFilterGraphDrawArray(cr, laf->irData, laf->irDataSize, GRAPH_WIDTH, GRAPH_HEIGHT, -2.0, 2.0, TRUE);
		if(code) return TRUE;
	}

	gtk_widget_queue_draw(laf->irDFT);
	return TRUE;
}

LAErrorCode LAFilterGraphDrawDFTBG(cairo_t *cr, const size_t width, const size_t height, const size_t border, const double offsetX, const double offsetY){
	LA_HANDLE_NULLPTR(cr, LA_PROPAGATE_ERROR);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, 1);
	cairo_rectangle(cr, offsetX + border, offsetY + border, width - 2*border, height - 2*border);
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, offsetX + border, offsetY + border, width - 2*border, height - 2*border);
	cairo_fill(cr);
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
	cairo_move_to(cr, offsetX + width / 2, offsetY + border);
	cairo_line_to(cr, offsetX + width / 2, offsetY + height - border);
	cairo_stroke(cr);

	return LA_NO_ERROR;
}

LAErrorCode LAFilterGraphDrawArrayDFT(cairo_t *cr, double *bins, const size_t size, const size_t width, const size_t height, const size_t border){
	LA_HANDLE_NULLPTR(cr, LA_PROPAGATE_ERROR);

	double maxValue =  12;

	double x = 0;
	double y = 0;
	double logValue = 0;

	cairo_move_to(cr, border, height - border);
	for(size_t i = 0; i < size; i++){
		logValue = log10(bins[i]);

		x = border + (i / (double) (size-1)) * (width - 2*border);
		y = (height / (double) 2) - (logValue / (double) maxValue) * (width - 2*border) / (double) 2;

		y = (y > (height - border))	? (height - border) : y;
		y = (y < border)	        ? border            : y;
		cairo_line_to(cr, x, y);
	}

	cairo_line_to(cr, width - border, height - border);
	cairo_move_to(cr, border, height - border);
	cairo_set_source_rgb(cr, 0.7, 0.7, 1);
	cairo_fill_preserve(cr);
	cairo_set_source_rgb(cr, 0, 0, 1);
	cairo_stroke(cr);
	
	return LA_NO_ERROR;
}

LAErrorCode LAFilterGraphDrawArrayPhase(cairo_t *cr, double *phase, const size_t size, const size_t width, const size_t height, const size_t border, const double offsetX, const double offsetY){
	LA_HANDLE_NULLPTR(cr, LA_PROPAGATE_ERROR);

	double maxValue = M_PI;

	double x = 0;
	double y = 0;

	cairo_move_to(cr, offsetX + border, offsetY + (height / 2));
	for(size_t i = 0; i < size; i++){
		x = border + (i / (double) (size-1)) * (width - 2*border);
		y = (height / (double) 2) - (phase[i] / (double) maxValue) * ((height - 2*border) / (double) 2);

		y = (y > (height - border))	? (height - border) : y;
		y = (y < border)	        ? border            : y;

		cairo_line_to(cr, offsetX + x, offsetY + y);
	}

	cairo_line_to(cr, offsetX + width - border, offsetY + (height / 2));
	cairo_set_source_rgb(cr, 0.7, 0.9, 1);
	cairo_fill_preserve(cr);
	cairo_set_source_rgb(cr, 0, 0.6, 0.7);
	cairo_stroke(cr);
	
	return LA_NO_ERROR;
}


gboolean LAOnDrawFilterIRDFT(GtkWidget *widget, cairo_t *cr, LAFilterWindow *laf){
	LA_HANDLE_NULLPTR(widget, TRUE);
	LA_HANDLE_NULLPTR(cr, TRUE);
	LA_HANDLE_NULLPTR(laf, TRUE);
	LAErrorCode code = LA_NO_ERROR;

	// code = LAFilterGraphDrawBG(cr, GRAPH_WIDTH, GRAPH_HEIGHT, FALSE);
	// if(code) return TRUE;
	const size_t border 	= LA_DFT_BORDER;
	const size_t width  	= GRAPH_WIDTH;
	const size_t height 	= GRAPH_HEIGHT + 2*border;
	const size_t offsetY 	= GRAPH_HEIGHT + border;

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);
	cairo_stroke(cr);

	code = LAFilterGraphDrawDFTBG(cr, width, height, border, 0, 0);
	if(code) return TRUE;

	code = LAFilterGraphDrawDFTBG(cr, width, height, border, 0, offsetY);
	if(code) return TRUE;
	
	size_t binSize = 0x0;
	double *bins = NULL;
	double *phase = NULL;
	double binsIn[binSize];
	double phaseIn[binSize];
	double params[3] = {0, 0, 0};
	double limits = 4;

	if(laf->value.cfgIn){
		code = LADiscreteFT(laf->irDataIn, laf->irDataSize, binsIn, phaseIn, binSize, 0, params);
		if(code) goto cleanup;
		code = LAFilterGraphDrawArrayDFT(cr, binsIn, binSize, width, height, border);
		if(code) goto cleanup;
		code = LAFilterGraphDrawArrayPhase(cr, phaseIn, binSize, width, height, border, 0, offsetY);
		if(code) goto cleanup;
	}

	if(laf->value.cfgOut){
		code = LAFFTWindow2(laf->irData, laf->irDataSize, &bins, &phase, &binSize, 0, params);
		if(code) goto cleanup;

		cairo_set_source_rgb(cr, 0.7, 0.7, 1);
		code = LADrawFFTBilateral(cr, border, border, width - 2*border, height - 2*border, bins, binSize, -limits, limits, 1, false, true);
		cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
		cairo_set_line_width(cr, 1);
		cairo_set_source_rgb(cr, 0, 0, 1);
		cairo_stroke(cr);

		//code = LAFilterGraphDrawArrayDFT(cr, bins, binSize, width, height, border);
		if(code) goto cleanup;
		code = LAFilterGraphDrawArrayPhase(cr, phase, binSize, width, height, border, 0, offsetY);
		if(code) goto cleanup;
	}

cleanup:
	if(bins != NULL) free(bins);
	if(phase != NULL) free(phase);
	return TRUE;
}

gboolean LAOnDrawFilterIIRPZMap(GtkWidget *widget, cairo_t *cr, LAFilterWindow *laf){
	LA_HANDLE_NULLPTR(widget, TRUE);
	LA_HANDLE_NULLPTR(cr, TRUE);
	LA_HANDLE_NULLPTR(laf, TRUE);
	LAErrorCode code = LA_NO_ERROR;

	LAComplexDouble zeros[2];
	LAComplexDouble poles[2];

	code = LAComplexDoubleRootsArray(&(zeros[0]), &(zeros[1]), laf->value.IIRb, LA_IIR_LENGTH_B);
	if(code) return TRUE;
	code = LAComplexDoubleRootsArray(&(poles[0]), &(poles[1]), laf->value.IIRa, LA_IIR_LENGTH_A);
	if(code) return TRUE;

	code = LAPZMapDraw(cr, zeros, 2, poles, 2, LA_PZMAP_WIDTH, LA_PZMAP_HEIGHT, 32);
	if(code) return TRUE;

	return TRUE;
}

gboolean LAOnFilterTypeChange(GtkWidget *widget, LAFilterWindow *laf){
	guint value = gtk_combo_box_get_active(GTK_COMBO_BOX(laf->mainTypeSelector));
	laf->value.filterType = value;
	gtk_widget_queue_draw(laf->irGraph);
	(void) widget;
	return FALSE;
}


gboolean LAOnFilterFIRFreqChange(GtkWidget *widget, LAFilterWindow *laf){
	double value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(laf->firFreqSB));
	laf->value.FIRFrequency = value;
	gtk_widget_queue_draw(laf->firFreqGraph);
	gtk_widget_queue_draw(laf->irGraph);
	(void) widget;
	return FALSE;
}

gboolean LAOnFilterFIRTypeChange(GtkWidget *widget, LAFilterWindow *laf){
	double value = gtk_combo_box_get_active(GTK_COMBO_BOX(laf->firTypeSelector));
	laf->value.FIRFilterType= value;
	gtk_widget_queue_draw(laf->firFreqGraph);
	gtk_widget_queue_draw(laf->irGraph);
	(void) widget;
	return FALSE;
}

gboolean LAOnFilterFIRParam2Change(GtkWidget *widget, LAFilterWindow *laf){
	double value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(laf->firParam2SB));
	laf->value.FIRParam2 = value;
	gtk_widget_queue_draw(laf->firWindowGraph);
	gtk_widget_queue_draw(laf->irGraph);
	(void) widget;
	return FALSE;
}

gboolean LAOnFilterFIRParam3Change(GtkWidget *widget, LAFilterWindow *laf){
	double value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(laf->firParam3SB));
	laf->value.FIRParam3 = value;
	gtk_widget_queue_draw(laf->firWindowGraph);
	gtk_widget_queue_draw(laf->irGraph);
	(void) widget;
	return FALSE;
}

gboolean LAOnFilterFIRSizeChange(GtkWidget *widget, LAFilterWindow *laf){
	guint value = gtk_combo_box_get_active(GTK_COMBO_BOX(laf->mainSizeSelector));
	laf->value.FIRFilterSize = value;

	if(laf->uiReady){
		gtk_widget_queue_draw(laf->firFreqGraph);
		gtk_widget_queue_draw(laf->irGraph);
	}
	(void) widget;
	return FALSE;
}


gboolean LAOnFilterIIRAChange(GtkWidget *widget, LAFilterWindow *laf){
	double value = 0;

	for(uint8_t i = 0; i < LA_IIR_LENGTH_A; i++){
		value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(laf->iirASB[i]));
		laf->value.IIRa[i] = value;
	}

	if(laf->uiReady){
		gtk_widget_queue_draw(laf->irGraph);
		gtk_widget_queue_draw(laf->iirPZMap);
	}
	(void) widget;
	return FALSE;
}

gboolean LAOnFilterIIRBChange(GtkWidget *widget, LAFilterWindow *laf){
	double value = 0;

	for(uint8_t i = 0; i < LA_IIR_LENGTH_B; i++){
		value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(laf->iirBSB[i]));
		laf->value.IIRb[i] = value;
	}

	if(laf->uiReady){
		gtk_widget_queue_draw(laf->irGraph);
		gtk_widget_queue_draw(laf->iirPZMap);
	}
	(void) widget;
	return FALSE;
}

gboolean LAOnFilterMALengthChange(GtkWidget *widget, LAFilterWindow *laf){
	double value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(laf->maLengthSB));
	laf->value.MALength = value;
	gtk_widget_queue_draw(laf->irGraph);
	(void) widget;
	return FALSE;
}

gboolean LAOnFilterCSChange(GtkWidget *widget, LAFilterWindow *laf){
	double value = 0;

	for(uint8_t i = 0; i < LA_CS_COUNT; i++){
		value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(laf->csParSB[i]));
		laf->value.csParam[i] = value;
	}

	if(laf->uiReady){
		gtk_widget_queue_draw(laf->irGraph);
	}

	(void) widget;
	return FALSE;
}

gboolean LAOnFilterCSTypeChange(GtkWidget *widget, LAFilterWindow *laf){
	laf->value.csType = gtk_combo_box_get_active(GTK_COMBO_BOX(laf->csType));

	for(uint8_t i = 0; i < LA_CS_COUNT; i++){
		gtk_widget_set_visible(laf->csHbox[i], LACSParamEnable[laf->value.csType][i]);
	}

	if(laf->uiReady){
		gtk_widget_queue_draw(laf->irGraph);
	}

	(void) widget;
	return FALSE;
}

gboolean LAOnFilterCSSimTypeChange(GtkWidget *widget, LAFilterWindow *laf){
	laf->value.csSolver = gtk_combo_box_get_active(GTK_COMBO_BOX(laf->csSimType));

	if(laf->uiReady){
		gtk_widget_queue_draw(laf->irGraph);
	}

	(void) widget;
	return FALSE;
}

gboolean LAOnFilterCfgTestChange(GtkWidget *widget, LAFilterWindow *laf){
	laf->value.cfgTest = gtk_combo_box_get_active(GTK_COMBO_BOX(laf->cfgTest));

	if(laf->uiReady){
		gtk_widget_queue_draw(laf->irGraph);
	}

	(void) widget;
	return FALSE;
}

gboolean LAOnFilterCfgInClick(GtkWidget *widget, LAFilterWindow *laf){
	laf->value.cfgIn = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(laf->cfgIn));

	if(laf->uiReady){
		gtk_widget_queue_draw(laf->irGraph);
	}

	(void) widget;
	return FALSE;
}

gboolean LAOnFilterCfgOutClick(GtkWidget *widget, LAFilterWindow *laf){
	laf->value.cfgOut = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(laf->cfgOut));

	if(laf->uiReady){
		gtk_widget_queue_draw(laf->irGraph);
	}

	(void) widget;
	return FALSE;
}

gboolean LAOnFilterCfgNormClick(GtkWidget *widget, LAFilterWindow *laf){
	laf->value.cfgNorm = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(laf->cfgNorm));

	if(laf->uiReady){
		gtk_widget_queue_draw(laf->irGraph);
	}

	(void) widget;
	return FALSE;
}

LAErrorCode LAOpenFilterWindow(LAFilterWindow *laf, int *response){
	LA_HANDLE_NULLPTR(laf, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(response, LA_PROPAGATE_ERROR);

	laf->window = gtk_dialog_new_with_buttons(
					"Filter Editor",
					GTK_WINDOW(lawp->window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					"_Apply", GTK_RESPONSE_ACCEPT,
					"_Quit", GTK_RESPONSE_CANCEL,
					NULL
				);

	laf->irDataSize = LA_BUFFER_SIZE;
	laf->uiReady	= false;

	gtk_window_set_modal(GTK_WINDOW(laf->window), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(laf->window), 8);
	laf->content = gtk_dialog_get_content_area(GTK_DIALOG(laf->window));

	laf->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(laf->content), GTK_WIDGET(laf->vbox));

	laf->hboxMain = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(laf->vbox), GTK_WIDGET(laf->hboxMain));

	//laf->hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	laf->hbox  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	laf->hbox2 = gtk_notebook_new();
	gtk_container_add(GTK_CONTAINER(laf->hbox), GTK_WIDGET(laf->hbox2));
	gtk_container_add(GTK_CONTAINER(laf->vbox), GTK_WIDGET(laf->hbox));

	laf->hboxFIR = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_notebook_append_page(GTK_NOTEBOOK(laf->hbox2), laf->hboxFIR, gtk_label_new("FIR"));
	//gtk_container_add(GTK_CONTAINER(laf->hbox), GTK_WIDGET(laf->vboxFIR));
	laf->vboxIIR = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_notebook_append_page(GTK_NOTEBOOK(laf->hbox2), laf->vboxIIR, gtk_label_new("IIR"));
	//gtk_container_add(GTK_CONTAINER(laf->hbox), GTK_WIDGET(laf->vboxIIR));
	laf->vboxMA  = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_notebook_append_page(GTK_NOTEBOOK(laf->hbox2), laf->vboxMA, gtk_label_new("Moving Average"));
	laf->vboxCS  = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_notebook_append_page(GTK_NOTEBOOK(laf->hbox2), laf->vboxCS, gtk_label_new("Circuit Simulation"));
	//gtk_container_add(GTK_CONTAINER(laf->hbox), GTK_WIDGET(laf->vboxMA));
	laf->prevNote = gtk_notebook_new();
	laf->vboxIR  = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_notebook_append_page(GTK_NOTEBOOK(laf->prevNote), laf->vboxIR, gtk_label_new("Impulse response"));
	laf->vboxCfg  = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_notebook_append_page(GTK_NOTEBOOK(laf->prevNote), laf->vboxCfg, gtk_label_new("Config"));
	gtk_container_add(GTK_CONTAINER(laf->hbox), GTK_WIDGET(laf->prevNote));

	laf->vboxFIR1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(laf->hboxFIR), GTK_WIDGET(laf->vboxFIR1));
	laf->vboxFIR2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(laf->hboxFIR), GTK_WIDGET(laf->vboxFIR2));
	laf->hboxIIR = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	laf->vboxIIR1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	laf->vboxIIR2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(laf->hboxIIR), GTK_WIDGET(laf->vboxIIR1));
	gtk_container_add(GTK_CONTAINER(laf->hboxIIR), GTK_WIDGET(laf->vboxIIR2));

	laf->mainTypeLabel 			= gtk_label_new("Filter type:");
	laf->mainTypeSelector 		= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainTypeSelector), "FIR");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainTypeSelector), "IIR");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainTypeSelector), "Moving Average");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainTypeSelector), "Circuit Simulation");
	gtk_combo_box_set_active(GTK_COMBO_BOX(laf->mainTypeSelector), 0);
	laf->mainSizeLabel 			= gtk_label_new("Filter size:");
	laf->mainSizeSelector 		= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "2 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "4 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "8 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "16 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "32 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "64 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "128 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "256 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "512 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "1024 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "2048 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "4096 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "8192 samples");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->mainSizeSelector), "16384 samples");
	gtk_combo_box_set_active(GTK_COMBO_BOX(laf->mainSizeSelector), 0);
	laf->mainOpenPreset			= gtk_button_new_with_label("Open preset");
	laf->mainPresetName			= gtk_label_new("Not selected");
	gtk_container_add(GTK_CONTAINER(laf->hboxMain), GTK_WIDGET(laf->mainTypeLabel));
	gtk_container_add(GTK_CONTAINER(laf->hboxMain), GTK_WIDGET(laf->mainTypeSelector));
	gtk_container_add(GTK_CONTAINER(laf->hboxMain), GTK_WIDGET(laf->mainOpenPreset));
	gtk_container_add(GTK_CONTAINER(laf->hboxMain), GTK_WIDGET(laf->mainPresetName));

	laf->firLabel				= gtk_label_new("Finite Impulse Response (FIR)");
	laf->firTypeLabel			= gtk_label_new("Filter type:");
	laf->firTypeSelector 		= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firTypeSelector), "Low pass");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->firTypeSelector), "High pass");
	gtk_combo_box_set_active(GTK_COMBO_BOX(laf->firTypeSelector), 0);
	laf->firFreqLabel			= gtk_label_new("Frequency:");
	laf->firFreqAdj				= gtk_adjustment_new(1, -32768, 32768, 0.01, 1, 0);
	laf->firFreqSB				= gtk_spin_button_new(laf->firFreqAdj, 1, 2);
	laf->firFreqGraph			= gtk_drawing_area_new();
	gtk_widget_set_size_request(laf->firFreqGraph, GRAPH_MINI_WIDTH, GRAPH_MINI_HEIGHT);
	LAWindowWidgetInit(&(laf->windowWidget));

	gtk_container_add(GTK_CONTAINER(laf->vboxFIR1), GTK_WIDGET(laf->firLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR1), GTK_WIDGET(laf->mainSizeLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR1), GTK_WIDGET(laf->mainSizeSelector));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR1), GTK_WIDGET(laf->firTypeLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR1), GTK_WIDGET(laf->firTypeSelector));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR1), GTK_WIDGET(laf->firFreqLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR1), GTK_WIDGET(laf->firFreqSB));
	gtk_container_add(GTK_CONTAINER(laf->vboxFIR1), GTK_WIDGET(laf->firFreqGraph));
	LAWindowWidgetAdd(laf->vboxFIR2, &(laf->windowWidget));

	laf->iirLabel				= gtk_label_new("Infinite Impulse Response (IIR)");

	gtk_container_add(GTK_CONTAINER(laf->vboxIIR), GTK_WIDGET(laf->iirLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxIIR), GTK_WIDGET(laf->hboxIIR));

	char smallBuffer[LA_SMALL_BUFFER_SIZE];
	for(uint8_t i = 0; i < LA_IIR_LENGTH_A; i++){
		laf->iirAHbox[i] 	= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
		gtk_container_add(GTK_CONTAINER(laf->vboxIIR1), GTK_WIDGET(laf->iirAHbox[i]));
		snprintf(smallBuffer, LA_SMALL_BUFFER_SIZE, "a%i", i);
		laf->iirALabel[i] 	= gtk_label_new(smallBuffer);
		laf->iirAAdj[i] 	= gtk_adjustment_new(0.0, -5.0, 5.0, 0.01, 0.1, 0);
		laf->iirASB[i]		= gtk_spin_button_new(laf->iirAAdj[i], 1, 4);
		gtk_widget_set_size_request(laf->iirALabel[i], 64,  32);
		gtk_widget_set_size_request(laf->iirASB[i],    192, 32);
		gtk_container_add(GTK_CONTAINER(laf->iirAHbox[i]), GTK_WIDGET(laf->iirALabel[i]));
		gtk_container_add(GTK_CONTAINER(laf->iirAHbox[i]), GTK_WIDGET(laf->iirASB[i]));
		g_signal_connect(laf->iirASB[i], "value-changed", G_CALLBACK(LAOnFilterIIRAChange), laf);
	}

	for(uint8_t i = 0; i < LA_IIR_LENGTH_B; i++){
		laf->iirBHbox[i] 	= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
		gtk_container_add(GTK_CONTAINER(laf->vboxIIR2), GTK_WIDGET(laf->iirBHbox[i]));
		snprintf(smallBuffer, LA_SMALL_BUFFER_SIZE, "b%i", i);
		laf->iirBLabel[i] 	= gtk_label_new(smallBuffer);
		laf->iirBAdj[i] 	= gtk_adjustment_new(0.0, -5.0, 5.0, 0.01, 0.1, 0);
		laf->iirBSB[i]		= gtk_spin_button_new(laf->iirBAdj[i], 1, 4);
		gtk_widget_set_size_request(laf->iirBLabel[i], 64,  32);
		gtk_widget_set_size_request(laf->iirBSB[i],    192, 32);
		gtk_container_add(GTK_CONTAINER(laf->iirBHbox[i]), GTK_WIDGET(laf->iirBLabel[i]));
		gtk_container_add(GTK_CONTAINER(laf->iirBHbox[i]), GTK_WIDGET(laf->iirBSB[i]));
		g_signal_connect(laf->iirBSB[i], "value-changed", G_CALLBACK(LAOnFilterIIRBChange), laf);
	}

	gtk_spin_button_set_value(GTK_SPIN_BUTTON(laf->iirASB[0]), 1.0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(laf->iirBSB[0]), 1.0);

	laf->iirFormula				= gtk_image_new_from_file("./assets/iirformula_small.png");
	gtk_widget_set_size_request(laf->iirFormula, GRAPH_MINI_WIDTH, GRAPH_MINI_HEIGHT);
	gtk_container_add(GTK_CONTAINER(laf->vboxIIR), GTK_WIDGET(laf->iirFormula));
	laf->iirPZMap				= gtk_drawing_area_new();
	gtk_widget_set_size_request(laf->iirPZMap, LA_PZMAP_WIDTH, LA_PZMAP_HEIGHT);
	gtk_container_add(GTK_CONTAINER(laf->vboxIIR), GTK_WIDGET(laf->iirPZMap));

	laf->maLabel				= gtk_label_new("Moving Average (MA)");
	laf->maLengthLabel			= gtk_label_new("Length:");
	laf->maLengthAdj			= gtk_adjustment_new(2, 1, 16384, 1, 16, 0);
	laf->maLengthSB				= gtk_spin_button_new(laf->maLengthAdj, 1, 1);

	gtk_container_add(GTK_CONTAINER(laf->vboxMA), GTK_WIDGET(laf->maLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxMA), GTK_WIDGET(laf->maLengthLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxMA), GTK_WIDGET(laf->maLengthSB));

	laf->csLabel				= gtk_label_new("Circuit Simulation (CS)");
	laf->csType 				= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csType), "1st order RC Low pass");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csType), "1st order RC High pass");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csType), "2nd order Sallen Key Low pass");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csType), "2nd order Sallen Key High pass");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csType), "Multiple Feedback Band pass");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csType), "Twin T Band stop");
	gtk_combo_box_set_active(GTK_COMBO_BOX(laf->csType), 0);
	gtk_container_add(GTK_CONTAINER(laf->vboxCS), GTK_WIDGET(laf->csLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxCS), GTK_WIDGET(laf->csType));
	
	for(size_t i = 0; i < LA_CS_COUNT; i++){
		laf->csHbox[i] 	= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
		gtk_container_add(GTK_CONTAINER(laf->vboxCS), GTK_WIDGET(laf->csHbox[i]));
		laf->csParLabel[i]	= gtk_label_new(LACSParams[i].name);
		laf->csParAdj[i] 	= gtk_adjustment_new(LACSParams[i].value, LACSParams[i].min, LACSParams[i].max, LACSParams[i].incSmall, LACSParams[i].inc, 0);
		laf->csParSB[i]		= gtk_spin_button_new(laf->csParAdj[i], 1, LACSParams[i].decimal);
		gtk_container_add(GTK_CONTAINER(laf->csHbox[i]), GTK_WIDGET(laf->csParLabel[i]));
		gtk_container_add(GTK_CONTAINER(laf->csHbox[i]), GTK_WIDGET(laf->csParSB[i]));
		g_signal_connect(laf->csParSB[i], "value-changed", G_CALLBACK(LAOnFilterCSChange), laf);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(laf->csParSB[i]), LACSParams[i].value);
	}

	laf->csSimLabel				= gtk_label_new("Simulation Engine");
	laf->csSimType 				= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Euler (fast, less precise, unstable)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Euler Implicit (Less precise, A-stable for stiff systems)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Trapezoidal (May oscillate, good, A-stable for stiff systems)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Backward Differentiation Formula order 2  (precise, A-stable, stiff systems)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Backward Differentiation Formula order 3  (precise, stiff systems)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Runge-Kutta 2 Midpoint (balanced)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Runge-Kutta 2 Heun (balanced)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Runge-Kutta 3 (balanced)");	
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Runge-Kutta 4 (slow, precise, stable)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Runge-Kutta 4 3/8 (slow, precise, stable)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Runge-Kutta 5 (even slower, precise, unstable if stiff)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Runge-Kutta 6 (even slower, more precise, unstable if stiff)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Runge-Kutta 8 (slowest, most precise, unstable if stiff)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "ode12 (adaptive)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Bogacki–Shampine 2,3 (slow, adaptive)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Runge-Kutta Fehlberg 4,5 (slow, adaptive)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Runge-Kutta Fehlberg 7,8 (very slow, adaptive)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Dormand-Prince 4,5 (slow, adaptive)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Runge-Kutta Cash-Karp 4,5 (slow, adaptive)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Adams-Bashforth 2nd order (fast, stable)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Adams-Bashforth 3rd order (fast)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Adams-Bashforth 4th order (fast)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Adams-Bashforth 5th order (medium, unstable)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Adams-Bashforth 6th order (medium, somewhat stable)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Adams-Bashforth 7th order (slow, unstable, may oscillate)");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Gauss-Legendre 4th order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Gauss-Legendre 6th order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Radau IA 3rd order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Radau IA 5th order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Radau IIA 3rd order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Radau IIA 5th order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Lobatto IIIA 2nd order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Lobatto IIIA 4th order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Lobatto IIIA 6th order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Lobatto IIIA 8th order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Lobatto IIIB 2nd order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Lobatto IIIB 4th order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Lobatto IIIB 6th order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Lobatto IIIB 8th order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Lobatto IIIC 2nd order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Lobatto IIIC 4th order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Lobatto IIIC 6th order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Lobatto IIIC 8th order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Adams-Moulton 2nd order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Adams-Moulton 3rd order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Adams-Moulton 4th order");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->csSimType), "Adams-Moulton 5th order");
	gtk_combo_box_set_active(GTK_COMBO_BOX(laf->csSimType), 2);
	gtk_container_add(GTK_CONTAINER(laf->vboxCS), GTK_WIDGET(laf->csSimLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxCS), GTK_WIDGET(laf->csSimType));


	laf->irLabel				= gtk_label_new("Impulse Response - Time domain");
	laf->irLabel2				= gtk_label_new("Impulse Response - Frequency domain");
	laf->irGraph				= gtk_drawing_area_new();
	gtk_widget_set_size_request(laf->irGraph, GRAPH_WIDTH, GRAPH_HEIGHT);
	laf->irDFT					= gtk_drawing_area_new();
	gtk_widget_set_size_request(laf->irDFT, GRAPH_WIDTH, 2*GRAPH_HEIGHT + 3*LA_DFT_BORDER);

	gtk_container_add(GTK_CONTAINER(laf->vboxIR), GTK_WIDGET(laf->irLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxIR), GTK_WIDGET(laf->irGraph));
	gtk_container_add(GTK_CONTAINER(laf->vboxIR), GTK_WIDGET(laf->irLabel2));
	gtk_container_add(GTK_CONTAINER(laf->vboxIR), GTK_WIDGET(laf->irDFT));

	laf->cfgLabel				= gtk_label_new("Preview config");
	laf->cfgTest 				= gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->cfgTest), "Impulse");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->cfgTest), "Step");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->cfgTest), "Noise");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->cfgTest), "Pulse train");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->cfgTest), "Sine");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(laf->cfgTest), "Saw");
	gtk_combo_box_set_active(GTK_COMBO_BOX(laf->cfgTest), 0);
	laf->cfgIn					= gtk_check_button_new_with_label("Draw Input");
	laf->cfgOut					= gtk_check_button_new_with_label("Draw Output");
	laf->cfgNorm				= gtk_check_button_new_with_label("Normalize output");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(laf->cfgIn), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(laf->cfgOut), TRUE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(laf->cfgNorm), FALSE);

	gtk_container_add(GTK_CONTAINER(laf->vboxCfg), GTK_WIDGET(laf->cfgLabel));
	gtk_container_add(GTK_CONTAINER(laf->vboxCfg), GTK_WIDGET(laf->cfgTest));
	gtk_container_add(GTK_CONTAINER(laf->vboxCfg), GTK_WIDGET(laf->cfgIn));
	gtk_container_add(GTK_CONTAINER(laf->vboxCfg), GTK_WIDGET(laf->cfgOut));
	gtk_container_add(GTK_CONTAINER(laf->vboxCfg), GTK_WIDGET(laf->cfgNorm));

	g_signal_connect(laf->mainOpenPreset,	"clicked", 			G_CALLBACK(LAFilterPreseteOpen), 		laf);
	g_signal_connect(laf->firFreqGraph,		"draw", 			G_CALLBACK(LAOnDrawFilterFreqGraph), 	laf);
	g_signal_connect(laf->firTypeSelector,	"changed", 			G_CALLBACK(LAOnFilterFIRTypeChange), 	laf);
	g_signal_connect(laf->irGraph,			"draw", 			G_CALLBACK(LAOnDrawFilterIRGraph), 		laf);
	g_signal_connect(laf->iirPZMap,			"draw", 			G_CALLBACK(LAOnDrawFilterIIRPZMap), 	laf);
	g_signal_connect(laf->irDFT,			"draw", 			G_CALLBACK(LAOnDrawFilterIRDFT), 		laf);
	g_signal_connect(laf->firFreqSB,		"value-changed", 	G_CALLBACK(LAOnFilterFIRFreqChange), 	laf);
	g_signal_connect(laf->mainSizeSelector,	"changed", 			G_CALLBACK(LAOnFilterFIRSizeChange), 	laf);
	g_signal_connect(laf->mainTypeSelector,	"changed", 			G_CALLBACK(LAOnFilterTypeChange), 		laf);
	g_signal_connect(laf->maLengthSB,		"value-changed", 	G_CALLBACK(LAOnFilterMALengthChange), 	laf);
	g_signal_connect(laf->csType, 			"changed", 			G_CALLBACK(LAOnFilterCSTypeChange), 	laf);
	g_signal_connect(laf->csSimType, 		"changed", 			G_CALLBACK(LAOnFilterCSSimTypeChange), 	laf);
	g_signal_connect(laf->cfgTest, 			"changed", 			G_CALLBACK(LAOnFilterCfgTestChange), 	laf);
	g_signal_connect(laf->cfgIn,			"clicked", 			G_CALLBACK(LAOnFilterCfgInClick), 		laf);
	g_signal_connect(laf->cfgOut,			"clicked", 			G_CALLBACK(LAOnFilterCfgOutClick), 		laf);
	g_signal_connect(laf->cfgNorm,			"clicked", 			G_CALLBACK(LAOnFilterCfgNormClick), 	laf);
	LAWindowWidgetConnect(&(laf->windowWidget), laf->irGraph);

	laf->uiReady = true;
	LAFilterUpdateUI(laf);
	gtk_widget_show_all(laf->window);
	(*response) = gtk_dialog_run(GTK_DIALOG(laf->window));
	gtk_widget_destroy(laf->window);

	return LA_NO_ERROR;
}

void LAOnFilterButtonWave(GtkWidget *widget, LAWaveformEditor *lae){
	if(lae->currentwave == NULL){
		LADialogErrorGeneric(lawp, "Can't apply filter to non existing wave");
		return;
	}

	double x[256];
	for(size_t i = 0; i < 256; i++) x[i] = (lae->currentwave->data[i] - 0x80) / (double) 0x7F;

	double y[256];

	LAFilterWindow lafv;
	LAFilterValueInit(&(lafv.value));
	int response = 0;

	LAErrorCode code = LAOpenFilterWindow(&lafv, &response);
	if(code){
		LADialogErrorGeneric(lawp, "An error ocurred on filter window");
		return;
	}
	
	if(response == GTK_RESPONSE_ACCEPT){
		code = LAFilterCompute(x, 256, y, 256, &(lafv.value), &(lafv.windowWidget));
		for(size_t i = 0; i < lae->currentwave->size; i++) lae->currentwave->data[i] = (y[i] * 0x7F) + 0x80;
	}

	(void) widget;
}
