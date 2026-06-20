#ifndef LA_SHADER_LOADER_H
#define LA_SHADER_LOADER_H

#include "../liblogicanalyzer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

LAErrorCode LAShaderLoader(const char *path, uint8_t **output, size_t *size);

#endif //LA_SHADER_LOADER_H
