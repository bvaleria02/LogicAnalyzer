#ifndef LA_DRAW_FFTFREQ_H
#define LA_DRAW_FFTFREQ_H

#include "../liblogicanalyzer.h"
#include <cairo.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

LAErrorCode LAFilterGraphDrawArray(cairo_t *cr, double *array, size_t size, double width, double height, double min, double max, bool changeColor);
LAErrorCode LADrawFFTBilateral(cairo_t *cr, const size_t x, const size_t y, const size_t w, const size_t h, double *data, const size_t n, const double vmin, const double vmax, const size_t downsampler, const bool logX, const bool logY);

#endif //LA_DRAW_FFTFREQ_H
