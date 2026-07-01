#ifndef LA_SHADER_H
#define LA_SHADER_H

#include "../liblogicanalyzer.h"
#include <stdint.h>
#include <stdlib.h>
#include "../glad/glad/glad.h"
#include "../matrix/matrix.h"
#include "../matrix/opengl.h"

#define LA_SHADER_TYPE_AMOUNT 13

typedef enum {
	LA_SHADER_TYPE_UINT8	= 0,
	LA_SHADER_TYPE_INT8		= 1,
	LA_SHADER_TYPE_UINT16	= 2,
	LA_SHADER_TYPE_INT16	= 3,
	LA_SHADER_TYPE_UINT32	= 4,
	LA_SHADER_TYPE_INT32	= 5,
	LA_SHADER_TYPE_FLOAT	= 6,
	LA_SHADER_TYPE_VEC2		= 7,
	LA_SHADER_TYPE_VEC3		= 8,
	LA_SHADER_TYPE_VEC4		= 9,
	LA_SHADER_TYPE_MAT2		= 10,
	LA_SHADER_TYPE_MAT3		= 11,
	LA_SHADER_TYPE_MAT4		= 12
} LAShaderType;

typedef enum {
	LA_SHADER_TEXTURE_1D		= 0,
	LA_SHADER_TEXTURE_2D		= 1,
	LA_SHADER_TEXTURE_CUBEMAP	= 2
} LAShaderTextureMode;

typedef enum {
	LA_SHADER_TEXTURE_UINT8		= 0,
	LA_SHADER_TEXTURE_FLOAT		= 1
} LAShaderTextureDataType;

typedef enum {
	LA_SHADER_TEXTURE_RGB		= 0,
	LA_SHADER_TEXTURE_RGBA		= 1,
	LA_SHADER_TEXTURE_R8		= 2
} LAShaderTextureType;

typedef struct _la_shader_texture {
	LAShaderTextureMode 	tMode; 	// 1d, 2d, cubemap
	LAShaderTextureDataType dType; 	// byte, float
	LAShaderTextureType 	tType;	// RGB, RGBA, R8
	bool dirty;
	GLuint id;						// OpenGL internal id
	size_t width;					// x
	size_t height;					// y
	void *data;						// cubemap ceil
	void *data2;					// cubemap side 
	void *data3;					// cubemap floor
	struct _la_shader_texture *next;// next texture
} LAShaderTexture;

typedef struct _la_shader_value {
	void *value;
	LAShaderType type;
	size_t amount;
	bool dirty;
	GLuint id;
	char *uniformName;
	struct _la_shader_value *next;
} LAShaderValue;

typedef union {
	float array[5];
	struct {
		float x;
		float y;
		float z;
		float u;
		float v;
	};
} LAShaderVertex;

typedef struct {
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;

	const char *vertexSource;
	const char *fragmentSource;

	bool useEBO;

	GLuint vertexMode;
	GLuint vertexCount;
	GLuint elementCount;

	LAShaderValue *enable;
	LAShaderValue *disable;
	LAShaderValue *uniform;
	LAShaderTexture *textures;

	LAShaderVertex *vertex;
	GLuint *elements;
} LAShader;

typedef LAErrorCode (*LAShaderValueTraversalCallback)(LAShaderValue *, size_t);
typedef LAErrorCode (*LAShaderTextureTraversalCallback)(LAShaderTexture *, size_t);

extern const size_t LAShaderTypeLength[LA_SHADER_TYPE_AMOUNT];

// lashadervalue.c
LAErrorCode LAShaderValueInit(LAShaderValue **lsv, LAShaderType type, size_t amount, const char *name);
LAErrorCode LAShaderValueCopy(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, void *value);
LAErrorCode LAShaderValueSetU8(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, uint8_t *value);
LAErrorCode LAShaderValueSetI8(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, int8_t *value);
LAErrorCode LAShaderValueSetU16(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, uint16_t *value);
LAErrorCode LAShaderValueSetI16(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, int16_t *value);
LAErrorCode LAShaderValueSetU32(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, uint32_t *value);
LAErrorCode LAShaderValueSetI32(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, int32_t *value);
LAErrorCode LAShaderValueSetF(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, float *value);
LAErrorCode LAShaderValueSetVec2(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, LAVec2_t *value);
LAErrorCode LAShaderValueSetVec3(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, LAVec3_t *value);
LAErrorCode LAShaderValueSetVec4(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, LAVec4_t *value);
LAErrorCode LAShaderValueSetMat2(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, LAMat2_t *value);
LAErrorCode LAShaderValueSetMat3(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, LAMat3_t *value);
LAErrorCode LAShaderValueSetMat4(LAShaderValue *lsv, size_t start, size_t length, size_t valueLength, LAMat4_t *value);

// lashader.c
LAErrorCode LAShaderInit(LAShader *lse, const char *vertex, const char *fragment, const bool useEBO);
LAErrorCode LAShaderCreateOpenGL(LAShader *lse);
LAErrorCode LAShaderCompile(LAShader *lse);
LAErrorCode LAShaderDestroy(LAShader *lse);
LAErrorCode LAShaderBind(LAShader *lse);
LAErrorCode LAShaderValueAppend(LAShaderValue **lsv, LAShaderType type, size_t amount, LAShaderValue **value, const char *name);
LAErrorCode LAShaderAppendEnable(LAShader *lse, size_t amount, GLuint *values, LAShaderValue **lsv);
LAErrorCode LAShaderAppendDisable(LAShader *lse, size_t amount, GLuint *values, LAShaderValue **lsv);
LAErrorCode LAShaderAppendUniform(LAShader *lse, LAShaderType type, size_t amount, void *value, const char *name, LAShaderValue **lsv);
LAErrorCode LAShaderValueTraverse(LAShaderValue *lsv, LAShaderValueTraversalCallback callback);
LAErrorCode LAShaderRender(LAShader *lse, bool clearVAO);
LAErrorCode LAShaderValueDestroy(LAShaderValue *lsv, bool destroyAll);
LAErrorCode LAShaderSetVertex(LAShader *lse, LAShaderVertex *lsv, size_t vertexCount);
LAErrorCode LAShaderSetElement(LAShader *lse, GLuint *elements, size_t vertexCount);
LAErrorCode LAShaderAppendTexture(LAShader *lse, LAShaderTextureMode tMode, LAShaderTextureDataType dType, LAShaderTextureType tType, size_t width, size_t height, LAShaderTexture **outLST, void *value1, void *value2, void *value3);
LAErrorCode LAShaderTextureTraverse(LAShaderTexture *lst, LAShaderTextureTraversalCallback callback);

// callbacks.c
LAErrorCode LAShaderEnableCallback(LAShaderValue *lsv, size_t n);
LAErrorCode LAShaderDisableCallback(LAShaderValue *lsv, size_t n);
LAErrorCode LAShaderUniformCallback(LAShaderValue *lsv, size_t n);
LAErrorCode LAShaderTextureCallback(LAShaderTexture *lst, size_t n);

// lashadertexture.c
LAErrorCode LAShaderTextureInit(LAShaderTexture **lst, LAShaderTextureMode mode, LAShaderTextureDataType dType, LAShaderTextureType tType, size_t width, size_t height);
LAErrorCode LAShaderTextureValueCopy(LAShaderTexture *lst, size_t index, size_t x0, size_t y0, size_t width, size_t height, size_t valueWidth, size_t valueHeight, void *value);
LAErrorCode LAShaderTextureSet1d(LAShaderTexture *lst, size_t x0, size_t width, size_t valueWidth, void *value);
LAErrorCode LAShaderTextureSet2d(LAShaderTexture *lst, size_t x0, size_t y0, size_t width, size_t height, size_t valueWidth, size_t valueHeight, void *value);
LAErrorCode LAShaderTextureSetCubemap(LAShaderTexture *lst, size_t x0, size_t y0, size_t width, size_t height, size_t valueWidth, size_t valueHeight, void *value1, void *value2, void *value3);
LAErrorCode LAShaderTextureAppend(LAShaderTexture **lst, LAShaderTextureMode tMode, LAShaderTextureDataType dType, LAShaderTextureType tType, size_t width, size_t height, LAShaderTexture **value);
LAErrorCode LAShaderTextureDestroy(LAShaderTexture *lst, bool destroyAll);

#endif //LA_SHADER_H
