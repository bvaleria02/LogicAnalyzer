#ifndef LA_MATRIX_OPENGL_H
#define LA_MATRIX_OPENGL_H

#include "../liblogicanalyzer.h"
#include "matrix.h"
#include <stdlib.h>
#include <stdbool.h>

#define LA_VEC_WIDTH  1

#define LA_VEC_2_SIZE 2
#define LA_VEC_3_SIZE 3
#define LA_VEC_4_SIZE 4

#define LA_MAT_2_SIZE 2
#define LA_MAT_3_SIZE 3
#define LA_MAT_4_SIZE 4

#define LA_MAT_2_SIZE_2 (LA_MAT_2_SIZE * LA_MAT_2_SIZE)
#define LA_MAT_3_SIZE_2 (LA_MAT_3_SIZE * LA_MAT_3_SIZE)
#define LA_MAT_4_SIZE_2 (LA_MAT_4_SIZE * LA_MAT_4_SIZE)

typedef float LAVec2_t[LA_VEC_2_SIZE];
typedef float LAVec3_t[LA_VEC_3_SIZE];
typedef float LAVec4_t[LA_VEC_4_SIZE];

typedef float LAMat2_t[LA_MAT_2_SIZE_2];
typedef float LAMat3_t[LA_MAT_3_SIZE_2];
typedef float LAMat4_t[LA_MAT_4_SIZE_2];

LAErrorCode LAMatCreateMat2Flags(LAMat_t *m, LAMatFlags flags);
LAErrorCode LAMatCreateMat3Flags(LAMat_t *m, LAMatFlags flags);
LAErrorCode LAMatCreateMat4Flags(LAMat_t *m, LAMatFlags flags);
LAErrorCode LAMatCreateMat2(LAMat_t *m);
LAErrorCode LAMatCreateMat3(LAMat_t *m);
LAErrorCode LAMatCreateMat4(LAMat_t *m);

LAErrorCode LAMatCreateVec2Flags(LAMat_t *m, LAMatFlags flags);
LAErrorCode LAMatCreateVec3Flags(LAMat_t *m, LAMatFlags flags);
LAErrorCode LAMatCreateVec4Flags(LAMat_t *m, LAMatFlags flags);
LAErrorCode LAMatCreateVec2(LAMat_t *m);
LAErrorCode LAMatCreateVec3(LAMat_t *m);
LAErrorCode LAMatCreateVec4(LAMat_t *m);

LAErrorCode LAMatExportMat2(LAMat_t *m, LAMat2_t o);
LAErrorCode LAMatExportMat3(LAMat_t *m, LAMat3_t o);
LAErrorCode LAMatExportMat4(LAMat_t *m, LAMat4_t o);

LAErrorCode LAMatExportVec2(LAMat_t *m, LAVec2_t o);
LAErrorCode LAMatExportVec3(LAMat_t *m, LAVec3_t o);
LAErrorCode LAMatExportVec4(LAMat_t *m, LAVec4_t o);

LAErrorCode LAMatGLTraslation(LAMat_t *m, double x, double y, double z);
LAErrorCode LAMatGLScale(LAMat_t *m, double x, double y, double z);
LAErrorCode LAMatGLRotationX(LAMat_t *m, double phi);
LAErrorCode LAMatGLRotationY(LAMat_t *m, double theta);
LAErrorCode LAMatGLRotationZ(LAMat_t *m, double psi);
LAErrorCode LAMatGLRotationXYZ(LAMat_t *m, double phi, double theta, double psi);

#endif //LA_MATRIX
