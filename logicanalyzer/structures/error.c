#include "../error.h"
#include "../liblogicanalyzer.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "error.h"
#include <stdio.h>

LAErrorCode LALogStructureErrorBase(void *structure, const char *structname, const char *funcname){
    printf("[Error] Structure \"%s\" at %p takes the base case for %s\n", structname, structure, funcname);
    return LA_ERROR_VTABLE_BASE;
};
