#ifndef LA_FILTER_WINDOW
#define LA_FILTER_WINDOW

// filterwindow.c
LAErrorCode LAFilterGraphDrawBG(cairo_t *cr, double width, double height, bool midLine);
LAErrorCode LAFilterGenerateFIRBuffer(LAFilterValue *laf, double **buffer, size_t *size);
LAErrorCode LAFilterGenerateFIRSinc(double *buffer, size_t size, double f, int mode);
// LAErrorCode LAFilterGraphDrawArray(cairo_t *cr, double *array, size_t size, double width, double height, double min, double max, bool changeColor);
LAErrorCode LAFilterCompute(double *x, size_t nx, double *y, size_t ny, LAFilterValue *laf, LAFilterWindowWidget *windowWidget);
void LAOnFilterButtonWave(GtkWidget *widget, LAWaveformEditor *lae);

// filteriir.c
LAErrorCode LAFilterIIRCompile(double *x, size_t nx, double *y, size_t ny, double *a, size_t na, double *b, size_t nb);
LAErrorCode LAFilterMovingAverageCompile(double *x, size_t nx, double *y, size_t ny, size_t l);

#endif //LA_FILTER_WINDOW
