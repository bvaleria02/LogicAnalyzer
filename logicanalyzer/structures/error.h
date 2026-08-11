#ifndef LA_STRUCT_ERROR_H
#define LA_STRUCT_ERROR_H

#include "../liblogicanalyzer.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

LAErrorCode LALogStructureErrorBase(void *structure, const char *structname, const char *funcname);

#endif //LA_STRUCT_ERROR_H
