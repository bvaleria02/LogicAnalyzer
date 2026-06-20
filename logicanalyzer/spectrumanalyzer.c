//#include "glad/glad/glad.h"
#include <epoxy/gl.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "liblogicanalyzer.h"
#include <time.h>
#include <pthread.h>
#include <math.h>
#include "dataLoader/dataLoader.h"
#include "numericMethods/dft.h"
#include "numericMethods/fft.h"
#include "shaders/loader.h"

#define SPECTRUM_ANALYZER_WIDTH 768
#define SPECTRUM_ANALYZER_HEIGHT 512
#define SPECTRUM_ANALYZER_PADDING 64

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)
#define LA_SA_GL_BIN_SIZE 4096
#define LA_SA_GL_PADDING 0.0f
#define LA_SA_GL_ZOOM 1.0f


typedef struct {
	GtkWidget *vbox;
	GtkWidget *label;
	GtkWidget *entry;
	double x;
	double y;
} LASpectrumAnalyzerPeak;

LAErrorCode LASpectrumAnalyzerPeakInit(LASpectrumAnalyzerPeak *las, char *name){
	LA_HANDLE_NULLPTR(las,	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(name,	LA_PROPAGATE_ERROR);

	las->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	
	las->label 		= gtk_label_new(name);
	las->entry		= gtk_entry_new();
	gtk_editable_set_editable(GTK_EDITABLE(las->entry), FALSE);

	gtk_container_add(GTK_CONTAINER(las->vbox), GTK_WIDGET(las->label));
	gtk_container_add(GTK_CONTAINER(las->vbox), GTK_WIDGET(las->entry));

	las->x = 0;
	las->y = 0;
	return LA_NO_ERROR;
}

LAErrorCode LASpectrumAnalyzerPeakAdd(LASpectrumAnalyzerPeak *las, GtkWidget *container){
	LA_HANDLE_NULLPTR(las,			LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(container,	LA_PROPAGATE_ERROR);

	gtk_container_add(GTK_CONTAINER(container), GTK_WIDGET(las->vbox));

	return LA_NO_ERROR;
}

typedef struct {
	GtkWidget *window;
	GtkWidget *content;

	GtkWidget *vbox;
	GtkWidget *hbox;

	GtkWidget *vboxL;
	GtkWidget *vboxR;

	GtkWidget *spectrumBox;
	GtkWidget *spectrogram;
	GtkWidget *viewHbox;
	LABinarySizeWidget binarySize;
	LASpectrumAnalyzerPeak cursor;
	LASpectrumAnalyzerPeak peak;

	GtkWidget *loadData;
	GtkWidget *exportData;

	double *data;
	size_t dataSize;
	double sampleRate;

	LAFilterWindowWidget windowWidget;

	unsigned int shaderProgram; 
	float fftBins[LA_SA_GL_BIN_SIZE];
	unsigned int uniform_1d;
	unsigned int FFTBins;
	unsigned int VAO;
} LASpectrumAnalyzerWindow;


LAErrorCode LASpectrumAnalyzerDrawBG(cairo_t *cr){
	LA_HANDLE_NULLPTR(cr, TRUE);
	
	double x0 = SPECTRUM_ANALYZER_PADDING / 2;
	double y0 = SPECTRUM_ANALYZER_PADDING / 2;
	double  w = SPECTRUM_ANALYZER_WIDTH;
	double  h = SPECTRUM_ANALYZER_HEIGHT;

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, x0, y0, w, h);
	cairo_fill(cr);
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, 1);
	cairo_rectangle(cr, x0, y0, w, h);
	cairo_stroke(cr);

	return LA_NO_ERROR;
}

LAErrorCode LASpectrumAnalyzerDrawRuler(cairo_t *cr, LASpectrumAnalyzerWindow *las){
	LA_HANDLE_NULLPTR(cr, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(las, LA_PROPAGATE_ERROR);

	double x0 = SPECTRUM_ANALYZER_PADDING / 2;
	double dx = SPECTRUM_ANALYZER_WIDTH / 8;
	double y0 = SPECTRUM_ANALYZER_HEIGHT + (SPECTRUM_ANALYZER_PADDING / 2);
	double  h = 8;

	char buffer[LA_SMALL_BUFFER_SIZE];
	uint8_t ticks = 8;

	for(uint8_t i = 0; i <= ticks; i++){
		cairo_set_source_rgb(cr, 0, 0, 0);
		cairo_move_to(cr, x0 + i*dx, y0);
		cairo_line_to(cr, x0 + i*dx, y0 + h);
		cairo_stroke(cr);

		snprintf(buffer, LA_SMALL_BUFFER_SIZE, "%.0lf", (las->sampleRate / (double) (ticks * 2) * i));
		cairo_move_to(cr, x0 + i*dx - h, y0+3*h);
		cairo_show_text(cr, buffer);
	}

	return LA_NO_ERROR;
}

LAErrorCode LASpectrumAnalyzerDrawBins(cairo_t *cr, double *bins, size_t binCount){
	LA_HANDLE_NULLPTR(cr, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(bins, LA_PROPAGATE_ERROR);
		
	size_t downsample = 32;
	size_t ds_length = binCount / downsample;

	double maxValue = 0;
	for(size_t i = 0; i < ds_length; i++){
		double acc = 0;
		for(size_t j = 0; j < downsample; j++){
			acc += fabs(bins[i * downsample + j]);
		}
		if(acc > maxValue) maxValue = log10(acc);
	}

	double x = SPECTRUM_ANALYZER_PADDING / 2;
	double y = SPECTRUM_ANALYZER_HEIGHT + SPECTRUM_ANALYZER_PADDING / 2;
	cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
	cairo_set_source_rgb(cr, 1, 0, 0);
	cairo_move_to(cr, x, y);

	for(size_t i = 0; i < ds_length; i++){
		double acc = 0;
		for(size_t j = 0; j < downsample; j++){
			acc += fabs(bins[i * downsample + j]);
		}
		acc = acc / (double) downsample;
		acc = (acc < 1e-5) ? 0.0 : fmax(0.0, log10(acc));

		x = (SPECTRUM_ANALYZER_PADDING / 2) + SPECTRUM_ANALYZER_WIDTH * (i / (double) ds_length);
		y = (SPECTRUM_ANALYZER_PADDING / 2) + SPECTRUM_ANALYZER_HEIGHT * (1 - fmin(1.0, fmax(0.0, (acc / (double) maxValue))));
		cairo_line_to(cr, x, y);
	}

	cairo_line_to(cr, SPECTRUM_ANALYZER_WIDTH + SPECTRUM_ANALYZER_PADDING / 2, SPECTRUM_ANALYZER_HEIGHT + SPECTRUM_ANALYZER_PADDING / 2);
	cairo_line_to(cr, SPECTRUM_ANALYZER_PADDING / 2, SPECTRUM_ANALYZER_HEIGHT + SPECTRUM_ANALYZER_PADDING / 2);
	cairo_fill(cr);
	cairo_stroke(cr);

	return LA_NO_ERROR;
}

gboolean LAOnDrawSpectrumAnalyzer(GtkWidget *widget, cairo_t *cr, LASpectrumAnalyzerWindow *las){
	LA_HANDLE_NULLPTR(widget, TRUE);
	LA_HANDLE_NULLPTR(cr, TRUE);
	LA_HANDLE_NULLPTR(las, TRUE);

	LAErrorCode code = LA_NO_ERROR;

	code = LASpectrumAnalyzerDrawBG(cr);
	if(code) goto handle_error;

	code = LASpectrumAnalyzerDrawRuler(cr, las);
	if(code) goto handle_error;

	double bins[512];
	size_t binCount = 512;

	//for(size_t i = 0; i < binCount; i++) bins[i] = rand() / (double) RAND_MAX;

	LA_PROFILER(
		code = LAUnilateralDiscreteFTFortran(las->data, las->dataSize, bins, binCount, las->windowWidget.windowType, las->windowWidget.paramValue);,
		"Spectrum analyzer - Unilateral DFT <Fortran>"
	);
	if(code) goto handle_error;

	double *w = NULL;
	size_t nw = 0x0;

	LA_PROFILER({
		code = LAFFTWindow(las->data, las->dataSize, &w, &nw, las->windowWidget.windowType, las->windowWidget.paramValue);
		if(code) printf("Error code: %u\n", code);
	}, "Spectrum analyzer - FFT <C>");
	if(code) goto handle_error;

//	code = LASpectrumAnalyzerDrawBins(cr, bins, binCount);
	LA_PROFILER({
	code = LASpectrumAnalyzerDrawBins(cr, w, nw / 2);
	if(code) goto handle_error;
	}, "Spectrum analyzer - Cairo");

	goto cleanup;

handle_error:
	goto cleanup;

cleanup:
	if(w != NULL) free(w);
	return TRUE;
}

LAErrorCode LASpectrumAnalyzerComputeFFT(double *data, size_t size, float *output, size_t outSize, LAFilterWindowType windowType, double *windowParams, bool logX, bool logY, double limit){
	LA_HANDLE_NULLPTR(data, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(output, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(windowParams, LA_PROPAGATE_ERROR);

	double *w = NULL;
	size_t nw = 0x0;
	LAErrorCode code = LA_NO_ERROR;

	code = LAFFTWindow(data, size, &w, &nw, windowType, windowParams);
	if(code) goto handle_error;

	if(logX){
		//double log11_c = log(1 + 10);
		double e_1 = 2.0 * (M_E - 1);

		for(size_t i = 0; i < outSize; i++){
			//size_t lmin = floor(nw * log10(1.0 + 10.0 * (i       / (double) outSize)) / log11_c);
			//size_t lmax = floor(nw * log10(1.0 + 10.0 * ((i + 1) / (double) outSize)) / log11_c);
			size_t lmin = floor(nw * (exp(i       / (double) outSize) - 1) / e_1);
			size_t lmax = floor(nw * (exp((i + 1) / (double) outSize) - 1) / e_1);
//			double lmin = floor(exp(maxLog * ( i      / (double) outSize)));
//			double lmax = floor(exp(maxLog * ((i + 1) / (double) outSize)));
			lmin = (lmin >= nw) ? nw : lmin;
			lmax = (lmax >= nw) ? nw : lmax;

			double acc = 0.0;
			size_t count = (lmin >= lmax) ? 1 : lmax - lmin;
			for(size_t j = lmin; j < lmax; j++) acc += fabs(w[j]);
			acc /= (double) count;
			output[i] = acc;
		}
	} else {
		for(size_t i = 0; i < outSize; i++){
			size_t lmin = floor((nw * i    )   / (double) (2.0 * outSize));
			size_t lmax = floor((nw * (i + 1)) / (double) (2.0 * outSize));
			lmin = (lmin >= nw) ? nw : lmin;
			lmax = (lmax >= nw) ? nw : lmax;

			double acc = 0.0;
			size_t count = (lmin >= lmax) ? 1 : lmax - lmin;
			for(size_t j = lmin; j < lmax; j++) acc += w[j];
			acc /= (double) count;
			output[i] = acc;
		}
	}

	// Normalization + logY
	for(size_t i = 0; i < outSize; i++){
		double acc = output[i];
		acc = (logY) ? log10(fmax(acc, 1e-10)) : acc;
		acc = 0.5 + (acc / (2.0 * limit));
		output[i] = fmax(fmin(acc, 1.0), 0.0);
	}

//	for(size_t i = 0; i < outSize; i++) output[i] = data[i];
	goto cleanup;

handle_error:
	goto cleanup;

cleanup:
	if(w != NULL) free(w);
	return code;
}

gboolean LAOnRenderSpectrumAnalyzer(GtkWidget *widget, GdkGLContext *ctx, LASpectrumAnalyzerWindow *las){
	LA_HANDLE_NULLPTR(widget, TRUE);
	LA_HANDLE_NULLPTR(ctx, TRUE);
	LA_HANDLE_NULLPTR(las, TRUE);

	gtk_gl_area_make_current(GTK_GL_AREA(widget));
	if(gtk_gl_area_get_error(GTK_GL_AREA(widget))){
		printf("GLArea Error\n");
		return TRUE;
	}

	LAErrorCode code = LA_NO_ERROR;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	code = LASpectrumAnalyzerComputeFFT(las->data, las->dataSize, las->fftBins, LA_SA_GL_BIN_SIZE, las->windowWidget.windowType, las->windowWidget.paramValue, false, true, 4);
	if(code) return TRUE;

	glUseProgram(las->shaderProgram);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, LA_SA_GL_BIN_SIZE, GL_RED, GL_FLOAT, las->fftBins);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, las->FFTBins);
	glUniform1i(las->uniform_1d,	0);

	glBindVertexArray(las->VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDrawBuffer(GL_BACK);
	return TRUE;
}

LAErrorCode LASpectrumAnalyzerGLAreaInitBuffer(LASpectrumAnalyzerWindow *las){
	LA_HANDLE_NULLPTR(las, LA_PROPAGATE_ERROR);

	const float vertices[] = {
		-(1.0f - LA_SA_GL_PADDING),	   -(1.0f - LA_SA_GL_PADDING), 		0.0f,		 0.5 - (1/(2.0 * LA_SA_GL_ZOOM)),	 0.5 - (1/(2.0 * LA_SA_GL_ZOOM)),
		-(1.0f - LA_SA_GL_PADDING),	    (1.0f - LA_SA_GL_PADDING), 		0.0f,		 0.5 - (1/(2.0 * LA_SA_GL_ZOOM)),	 0.5 + (1/(2.0 * LA_SA_GL_ZOOM)),
		 (1.0f - LA_SA_GL_PADDING),	   -(1.0f - LA_SA_GL_PADDING), 		0.0f,		 0.5 + (1/(2.0 * LA_SA_GL_ZOOM)),	 0.5 - (1/(2.0 * LA_SA_GL_ZOOM)),
		 (1.0f - LA_SA_GL_PADDING),	    (1.0f - LA_SA_GL_PADDING), 		0.0f,		 0.5 + (1/(2.0 * LA_SA_GL_ZOOM)),	 0.5 + (1/(2.0 * LA_SA_GL_ZOOM))
	};

	const unsigned int indices[] = {
		0, 	1, 	2,
		1, 	2, 	3,
	};

	printf("Generating buffers\n");

	unsigned int VBO, EBO;
	glGenVertexArrays(1, &(las->VAO)); 
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	printf("Binding buffers\n");

	glBindVertexArray(las->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);  
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 
	
	printf("Attributes\n");

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);  
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);  

	printf("Textures\n");
	glGenTextures(1, &(las->FFTBins));
	glBindTexture(GL_TEXTURE_1D, las->FFTBins);
	
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	printf("Texture image\n");
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, LA_SA_GL_BIN_SIZE, 0, GL_RED, GL_FLOAT, las->fftBins);
	float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	las->uniform_1d = glGetUniformLocation(las->shaderProgram, "fftBins");
	glTexParameterfv(GL_TEXTURE_1D, GL_TEXTURE_BORDER_COLOR, borderColor);  

	return LA_NO_ERROR;
}

LAErrorCode LASpectrumAnalyzerGLAreaInitShaders(LASpectrumAnalyzerWindow *las){
	LA_HANDLE_NULLPTR(las, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	unsigned int vertexShader;
	unsigned int fragmentShader;
	int  success;
	char infoLog[512];
	char *vertexShaderSource = NULL;
	char *fragmentShaderSource = NULL;

	code = LAShaderLoader("./logicanalyzer/shaders/spectrumAnalyzer_vertex.glsl", (uint8_t **)&vertexShaderSource, NULL);
	if(code) goto handle_loader_error;

	printf("Vertex\n");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	printf("Vertex create\n");
	glShaderSource(vertexShader, 1, (const char **)&vertexShaderSource, NULL);
	printf("Vertex compiling\n");
	glCompileShader(vertexShader);
	printf("Vertex done\n");
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(!success){
    	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    	printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
		goto handle_opengl_shader_error;
	}

	code = LAShaderLoader("./logicanalyzer/shaders/spectrumAnalyzer_fragment.glsl", (uint8_t **)&fragmentShaderSource, NULL);
	if(code) goto handle_loader_error;
/*
*/
	printf("Fragment\n");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char **)&fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if(!success){
    	glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    	printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
		goto handle_opengl_shader_error;
	}

	printf("Shader\n");
	las->shaderProgram = glCreateProgram();
	glAttachShader(las->shaderProgram, vertexShader);
	glAttachShader(las->shaderProgram, fragmentShader);
	glLinkProgram(las->shaderProgram);
	glGetProgramiv(las->shaderProgram, GL_LINK_STATUS, &success);
	if(!success) {
    	glGetProgramInfoLog(las->shaderProgram, 512, NULL, infoLog);
    	printf("ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n%s\n", infoLog);
		goto handle_opengl_shader_error;
	}

	(void) code;
	goto cleanup;

handle_loader_error:
	goto cleanup;

handle_opengl_shader_error:
	code = LA_ERROR_OPENGL_SHADERS;
	goto cleanup;

cleanup:
	if(vertexShaderSource != NULL) 		free(vertexShaderSource);
	if(fragmentShaderSource != NULL) 	free(fragmentShaderSource);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
	return LA_NO_ERROR;
}


void LAOnRealizeSpectrumAnalyzer(GtkGLArea *area, LASpectrumAnalyzerWindow *las){
	if(area == NULL) return;
	if(las == NULL) return;

	
	gtk_gl_area_make_current(GTK_GL_AREA(area));
	if(gtk_gl_area_get_error(GTK_GL_AREA(area))){
		printf("GLArea Error\n");
		return;
	}

	const GLubyte *vendor   = glGetString(GL_VENDOR);
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version  = glGetString(GL_VERSION);

	printf("Vendor   : %s\n", vendor);
	printf("Renderer : %s\n", renderer);
	printf("Version  : %s\n", version);

	LAErrorCode code = LA_NO_ERROR;

	srand(time(NULL));
	for(size_t i = 0; i < LA_SA_GL_BIN_SIZE; i++){
		las->fftBins[i] = rand() / (double) RAND_MAX;
	}

	printf("Creating shaders\n");
  	code = LASpectrumAnalyzerGLAreaInitShaders(las);
	if(code) return;

	printf("Creating buffers\n");
  	code = LASpectrumAnalyzerGLAreaInitBuffer(las);
	if(code) return;

	printf("Context done\n");

	return;
}

GdkGLContext *LAOnCreateContextSpectrumAnalyzer(GtkGLArea *area, LASpectrumAnalyzerWindow *las){
	if(area == NULL) return NULL;
	if(las == NULL) return NULL;
	

	GdkWindow *window = gtk_widget_get_window(GTK_WIDGET(area));
    GError *error = NULL;

    // 1. Attempt to create a context using GDK
    GdkGLContext *context = gdk_window_create_gl_context (window, &error);
	printf("Context: %p\n", context);

    // Enforce specific version (e.g., OpenGL 3.3 Core)
    gdk_gl_context_set_required_version (context, 3, 3);
    gdk_gl_context_set_debug_enabled (context, TRUE);
    gdk_gl_context_set_forward_compatible (context, TRUE);

	return context;
}

void LAOnResizeSpectrumAnalyzer(GtkGLArea *area, int width, int height, LASpectrumAnalyzerWindow *las){
	if(area == NULL) return;
	if(las == NULL) return;

	gtk_gl_area_make_current(GTK_GL_AREA(area));
	if(gtk_gl_area_get_error(GTK_GL_AREA(area))){
		printf("GLArea Error\n");
		return;
	}

	printf("W: %i\tH: %i\n", width, height);

	return;
}

gboolean LAOnSpectrumAnalyzerLoadData(GtkWidget *widget, LASpectrumAnalyzerWindow *las){
	LADataLoaderWindow lad;
	LAOpenDataLoader(&lad, lawp);
	(void) widget;
	(void) las;
	return TRUE;
}

LAErrorCode LAOpenSpectrumAnalyzerWindow(LASpectrumAnalyzerWindow *las, double *buffer, size_t size){
	LA_HANDLE_NULLPTR(las, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(buffer, 	LA_PROPAGATE_ERROR);

	// LAErrorCode code = LA_NO_ERROR;

	las->window = gtk_dialog_new_with_buttons(
					"Spectrum Analyzer",
					GTK_WINDOW(lawp->window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					"_Quit", GTK_RESPONSE_CANCEL,
					NULL
				);
	
	gtk_window_set_modal(GTK_WINDOW(las->window), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(las->window), 8);
	las->content = gtk_dialog_get_content_area(GTK_DIALOG(las->window));

	las->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(las->content), GTK_WIDGET(las->vbox));

	las->hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(las->vbox), GTK_WIDGET(las->hbox));

	las->vboxL = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_widget_set_hexpand(las->vboxL, TRUE);
	gtk_widget_set_vexpand(las->vboxL, TRUE);
	gtk_container_add(GTK_CONTAINER(las->hbox), GTK_WIDGET(las->vboxL));
	las->vboxR = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_container_add(GTK_CONTAINER(las->hbox), GTK_WIDGET(las->vboxR));

	las->spectrumBox = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(las->spectrumBox), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
	gtk_widget_set_size_request(las->spectrumBox, 1400, 400);
//	gtk_widget_set_hexpand(las->spectrumBox, TRUE);
//	gtk_widget_set_vexpand(las->spectrumBox, TRUE);
	gtk_container_add(GTK_CONTAINER(las->vboxL), GTK_WIDGET(las->spectrumBox));
	las->spectrogram = gtk_gl_area_new();
	gtk_gl_area_set_has_depth_buffer(GTK_GL_AREA(las->spectrogram), FALSE);
	gtk_gl_area_set_has_stencil_buffer(GTK_GL_AREA(las->spectrogram), FALSE);
	gtk_widget_set_size_request(las->spectrogram, 4096, 400);
	gtk_container_add(GTK_CONTAINER(las->spectrumBox), GTK_WIDGET(las->spectrogram));

	las->viewHbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_container_add(GTK_CONTAINER(las->vboxL), GTK_WIDGET(las->viewHbox));

	LASpectrumAnalyzerPeakInit(&(las->cursor), "Cursor");
	LASpectrumAnalyzerPeakAdd(&(las->cursor), las->viewHbox);
	LASpectrumAnalyzerPeakInit(&(las->peak), "Peak");
	LASpectrumAnalyzerPeakAdd(&(las->peak), las->viewHbox);

	LABinarySizeWidgetInit(&(las->binarySize), "Window Length", 1, 15, "samples");
	LABinarySizeWidgetAdd(&(las->binarySize), las->viewHbox);

	las->loadData 	= gtk_button_new_with_label("Load data");
	las->exportData = gtk_button_new_with_label("Export Spectrum");
	gtk_container_add(GTK_CONTAINER(las->viewHbox), GTK_WIDGET(las->loadData));
	gtk_container_add(GTK_CONTAINER(las->viewHbox), GTK_WIDGET(las->exportData));

	LAWindowWidgetInit(&(las->windowWidget));
	LAWindowWidgetAdd(las->vboxR, &(las->windowWidget));
	LAWindowWidgetConnect(&(las->windowWidget), las->spectrogram);

	//g_signal_connect(las->spectrogram,	"draw",	    G_CALLBACK(LAOnDrawSpectrumAnalyzer), las);
	g_signal_connect(las->spectrogram,	"render",			G_CALLBACK(LAOnRenderSpectrumAnalyzer), 		las);
	g_signal_connect(las->spectrogram,	"create-context",  	G_CALLBACK(LAOnCreateContextSpectrumAnalyzer), 	las);
	g_signal_connect(las->spectrogram,	"realize",  		G_CALLBACK(LAOnRealizeSpectrumAnalyzer), 		las);
	//g_signal_connect(las->spectrogram,	"resize",   G_CALLBACK(LAOnResizeSpectrumAnalyzer), las);
	g_signal_connect(las->loadData, 	"clicked",			G_CALLBACK(LAOnSpectrumAnalyzerLoadData), 		las);

	gtk_widget_show_all(las->window);
	gtk_dialog_run(GTK_DIALOG(las->window));
	gtk_widget_destroy(las->window);

	(void) size;
	return LA_NO_ERROR;
}

gboolean LAOpenSpectrumAnalyzer(GtkWidget *widget, LAWindow *law){
	LASpectrumAnalyzerWindow las;

	size_t size = LA_LARGE_BUFFER_SIZE;
	double *buffer = (double *)malloc(sizeof(double) * size);

	for(size_t i = 0; i < size; i++) buffer[i] = -1 + 2 *(law->dataBuffer[i] / (double) 256); 

	las.data = buffer;
	las.dataSize = size;
	las.sampleRate = 1 / 0.0001;

	LAErrorCode code = LAOpenSpectrumAnalyzerWindow(&las, buffer, size);
	if(code) goto cleanup;
	(void) widget;

cleanup:
	free(buffer);

	return TRUE;
}
