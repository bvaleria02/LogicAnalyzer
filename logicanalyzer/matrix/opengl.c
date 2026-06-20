#include "../liblogicanalyzer.h"
#include "matrix.h"
#include "opengl.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

LAErrorCode LAMatCreateMat2Flags(LAMat_t *m, LAMatFlags flags){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	return LAMatCreateDynamicFlags(m, LA_MAT_2_SIZE, LA_MAT_2_SIZE, flags);
}

LAErrorCode LAMatCreateMat3Flags(LAMat_t *m, LAMatFlags flags){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	return LAMatCreateDynamicFlags(m, LA_MAT_3_SIZE, LA_MAT_3_SIZE, flags);
}

LAErrorCode LAMatCreateMat4Flags(LAMat_t *m, LAMatFlags flags){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	return LAMatCreateDynamicFlags(m, LA_MAT_4_SIZE, LA_MAT_4_SIZE, flags);
}

LAErrorCode LAMatCreateMat2(LAMat_t *m){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	return LAMatCreateDynamic(m, LA_MAT_2_SIZE, LA_MAT_2_SIZE);
}

LAErrorCode LAMatCreateMat3(LAMat_t *m){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	return LAMatCreateDynamic(m, LA_MAT_3_SIZE, LA_MAT_3_SIZE);
}

LAErrorCode LAMatCreateMat4(LAMat_t *m){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	return LAMatCreateDynamic(m, LA_MAT_4_SIZE, LA_MAT_4_SIZE);
}

LAErrorCode LAMatCreateVec2Flags(LAMat_t *m, LAMatFlags flags){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	return LAMatCreateDynamicFlags(m, LA_VEC_2_SIZE, LA_VEC_WIDTH, flags);
}

LAErrorCode LAMatCreateVec3Flags(LAMat_t *m, LAMatFlags flags){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	return LAMatCreateDynamicFlags(m, LA_VEC_3_SIZE, LA_VEC_WIDTH, flags);
}

LAErrorCode LAMatCreateVec4Flags(LAMat_t *m, LAMatFlags flags){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	return LAMatCreateDynamicFlags(m, LA_VEC_4_SIZE, LA_VEC_WIDTH, flags);
}

LAErrorCode LAMatCreateVec2(LAMat_t *m){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	return LAMatCreateDynamic(m, LA_VEC_2_SIZE, LA_VEC_WIDTH);
}

LAErrorCode LAMatCreateVec3(LAMat_t *m){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	return LAMatCreateDynamic(m, LA_VEC_3_SIZE, LA_VEC_WIDTH);
}

LAErrorCode LAMatCreateVec4(LAMat_t *m){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	return LAMatCreateDynamic(m, LA_VEC_4_SIZE, LA_VEC_WIDTH);
}

LAErrorCode LAMatExportOpenGL_backend(LAMat_t *m, float *o, size_t size, size_t targetR, size_t targetC){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(o, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;	
	
	if(!(LAMatIsValid(m)))					  return LA_ERROR_MATRIX;
	if(!(LAMatMatchDim(m, targetR, targetC))) return LA_ERROR_NONMATCHING_DIMENSION;

	double value = 0.0;
	if((targetC * targetR) > size) 		  return LA_ERROR_NONMATCHING_DIMENSION;

	if(targetC == LA_VEC_WIDTH){
		for(size_t h = 0; h < targetR; h++){
			code = LAMatGet(m, h, 0, &value);
			if(code) return code;
			o[h] = (float) value;
		}
	} else {
		for(size_t w = 0; w < targetC; w++){
			for(size_t h = 0; h < targetR; h++){
				code = LAMatGet(m, h, w, &value);
				if(code) return code;
				o[w * targetC + h] = (float) value;
			}
		}
	}

	return code;
}

LAErrorCode LAMatExportMat2(LAMat_t *m, LAMat2_t o){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(o, LA_PROPAGATE_ERROR);
	return LAMatExportOpenGL_backend(m, (float *)o, LA_MAT_2_SIZE_2, LA_MAT_2_SIZE, LA_MAT_2_SIZE);
}

LAErrorCode LAMatExportMat3(LAMat_t *m, LAMat3_t o){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(o, LA_PROPAGATE_ERROR);
	return LAMatExportOpenGL_backend(m, (float *)o, LA_MAT_3_SIZE_2, LA_MAT_3_SIZE, LA_MAT_3_SIZE);
}

LAErrorCode LAMatExportMat4(LAMat_t *m, LAMat4_t o){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(o, LA_PROPAGATE_ERROR);
	return LAMatExportOpenGL_backend(m, (float *)o, LA_MAT_4_SIZE_2, LA_MAT_4_SIZE, LA_MAT_4_SIZE);
}

LAErrorCode LAMatExportVec2(LAMat_t *m, LAVec2_t o){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(o, LA_PROPAGATE_ERROR);
	return LAMatExportOpenGL_backend(m, (float *)o, LA_VEC_2_SIZE, LA_VEC_2_SIZE, LA_VEC_WIDTH);
}

LAErrorCode LAMatExportVec3(LAMat_t *m, LAVec3_t o){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(o, LA_PROPAGATE_ERROR);
	return LAMatExportOpenGL_backend(m, (float *)o, LA_VEC_3_SIZE, LA_VEC_3_SIZE, LA_VEC_WIDTH);
}

LAErrorCode LAMatExportVec4(LAMat_t *m, LAVec4_t o){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(o, LA_PROPAGATE_ERROR);
	return LAMatExportOpenGL_backend(m, (float *)o, LA_VEC_4_SIZE, LA_VEC_4_SIZE, LA_VEC_WIDTH);
}

LAErrorCode LAMatGLTraslation(LAMat_t *m, double x, double y, double z){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m)))			return LA_ERROR_MATRIX;
	if(!(LAMatMatchDim(m, 4, 4)))	return LA_ERROR_NONMATCHING_DIMENSION;

	LAErrorCode code = LA_NO_ERROR;

	code = LAMatEye(m);
	if(code) return code;

	code = LAMatSet(m, 0, 3, x);
	if(code) return code;
	code = LAMatSet(m, 1, 3, y);
	if(code) return code;
	code = LAMatSet(m, 2, 3, z);
	if(code) return code;

	return LA_NO_ERROR;
}

LAErrorCode LAMatGLScale(LAMat_t *m, double x, double y, double z){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m)))			return LA_ERROR_MATRIX;
	if(!(LAMatMatchDim(m, 4, 4)))	return LA_ERROR_NONMATCHING_DIMENSION;

	LAErrorCode code = LA_NO_ERROR;

	code = LAMatZeros(m);
	if(code) return code;

	code = LAMatSet(m, 0, 0, x);
	if(code) return code;
	code = LAMatSet(m, 1, 1, y);
	if(code) return code;
	code = LAMatSet(m, 2, 2, z);
	if(code) return code;
	code = LAMatSet(m, 3, 3, 1.0);
	if(code) return code;

	return LA_NO_ERROR;
}

LAErrorCode LAMatGLRotationX(LAMat_t *m, double phi){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m)))			return LA_ERROR_MATRIX;
	if(!(LAMatMatchDim(m, 4, 4)))	return LA_ERROR_NONMATCHING_DIMENSION;

	LAErrorCode code = LA_NO_ERROR;

	code = LAMatZeros(m);
	if(code) return code;

	double cos_phi = cos(phi);
	double sin_phi = sin(phi);

	code = LAMatSet(m, 0, 0, 1.0);
	if(code) return code;
	code = LAMatSet(m, 1, 1,  cos_phi);
	if(code) return code;
	code = LAMatSet(m, 1, 2, -sin_phi);
	if(code) return code;
	code = LAMatSet(m, 2, 1,  sin_phi);
	if(code) return code;
	code = LAMatSet(m, 2, 2,  cos_phi);
	if(code) return code;
	code = LAMatSet(m, 3, 3, 1.0);
	if(code) return code;

	return LA_NO_ERROR;
}

LAErrorCode LAMatGLRotationY(LAMat_t *m, double theta){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m)))			return LA_ERROR_MATRIX;
	if(!(LAMatMatchDim(m, 4, 4)))	return LA_ERROR_NONMATCHING_DIMENSION;

	LAErrorCode code = LA_NO_ERROR;

	code = LAMatZeros(m);
	if(code) return code;

	double cos_theta = cos(theta);
	double sin_theta = sin(theta);

	code = LAMatSet(m, 0, 0,  cos_theta);
	if(code) return code;
	code = LAMatSet(m, 0, 2,  sin_theta);
	if(code) return code;
	code = LAMatSet(m, 1, 1, 1.0);
	if(code) return code;
	code = LAMatSet(m, 2, 0, -sin_theta);
	if(code) return code;
	code = LAMatSet(m, 2, 2,  cos_theta);
	if(code) return code;
	code = LAMatSet(m, 3, 3, 1.0);
	if(code) return code;

	return LA_NO_ERROR;
}

LAErrorCode LAMatGLRotationZ(LAMat_t *m, double psi){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m)))			return LA_ERROR_MATRIX;
	if(!(LAMatMatchDim(m, 4, 4)))	return LA_ERROR_NONMATCHING_DIMENSION;

	LAErrorCode code = LA_NO_ERROR;

	code = LAMatZeros(m);
	if(code) return code;

	double cos_psi = cos(psi);
	double sin_psi = sin(psi);

	code = LAMatSet(m, 0, 0,  cos_psi);
	if(code) return code;
	code = LAMatSet(m, 0, 1, -sin_psi);
	if(code) return code;
	code = LAMatSet(m, 1, 0,  sin_psi);
	if(code) return code;
	code = LAMatSet(m, 1, 1,  cos_psi);
	if(code) return code;
	code = LAMatSet(m, 2, 2, 1.0);
	if(code) return code;
	code = LAMatSet(m, 3, 3, 1.0);
	if(code) return code;

	return LA_NO_ERROR;
}

LAErrorCode LAMatGLRotationXYZ(LAMat_t *m, double phi, double theta, double psi){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m)))			return LA_ERROR_MATRIX;
	if(!(LAMatMatchDim(m, 4, 4)))	return LA_ERROR_NONMATCHING_DIMENSION;

	LAErrorCode code = LA_NO_ERROR;

	code = LAMatZeros(m);
	if(code) return code;

	double Cx = cos(phi);
	double Sx = sin(phi);
	double Cy = cos(theta);
	double Sy = sin(theta);
	double Cz = cos(psi);
	double Sz = sin(psi);

	code = LAMatSet(m, 0, 0,  Cy*Cz);
	if(code) return code;
	code = LAMatSet(m, 0, 1, -Cy*Sz);
	if(code) return code;
	code = LAMatSet(m, 0, 2,  Sy);
	if(code) return code;
	code = LAMatSet(m, 1, 0,  Sx*Sy*Cz + Cx*Sz);
	if(code) return code;
	code = LAMatSet(m, 1, 1, -Sx*Sy*Sz + Cx*Cz);
	if(code) return code;
	code = LAMatSet(m, 1, 2, -Sx*Cy);
	if(code) return code;
	code = LAMatSet(m, 2, 0, -Cx*Sy*Cz + Sx*Sz);
	if(code) return code;
	code = LAMatSet(m, 2, 1,  Cx*Sy*Sz + Sx*Cz);
	if(code) return code;
	code = LAMatSet(m, 2, 2,  Cx*Cy);
	if(code) return code;
	code = LAMatSet(m, 3, 3, 1.0);
	if(code) return code;

	return LA_NO_ERROR;
}
