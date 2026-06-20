#ifndef LA_PZMAP
#define LA_PZMAP

#define LA_PZMAP_WIDTH 320
#define LA_PZMAP_HEIGHT 320

#include "../liblogicanalyzer.h"
#include "../laComplex/laComplex.h"

LAErrorCode LAPZMapDraw(cairo_t *cr, LAComplex *zeros, const size_t zn, LAComplex *poles, const size_t pn, const size_t width, const size_t height, const size_t border);

#endif // LA_PZMAP
