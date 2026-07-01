#include "../liblogicanalyzer.h"
#include "shaders.h"
#include <stdint.h>
#include <stdlib.h>
#include "../glad/glad/glad.h"

LAErrorCode LAShaderInit(LAShader *lse, const char *vertex, const char *fragment, const bool useEBO){
	LA_HANDLE_NULLPTR(lse, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(vertex, 	LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(fragment, LA_PROPAGATE_ERROR);

	lse->VAO = -1U;
	lse->VBO = -1U;
	lse->EBO = -1U;
	lse->vertexShader = -1U;
	lse->fragmentShader = -1U;
	lse->program = -1U;

	lse->vertexSource = vertex;
	lse->fragmentSource = fragment;
	
	lse->useEBO = useEBO;

	lse->vertexMode = GL_TRIANGLES;
	lse->vertexCount = 0;
	lse->elementCount = 0;

	lse->enable = NULL;
	lse->disable = NULL;
	lse->uniform = NULL;
	lse->vertex	= NULL;
	lse->elements = NULL;
	lse->textures= NULL;

	return LA_NO_ERROR;
}

LAErrorCode LAShaderCreateOpenGL(LAShader *lse){
	LA_HANDLE_NULLPTR(lse, 		LA_PROPAGATE_ERROR);

	glGenVertexArrays(1, &(lse->VAO)); 
	glGenBuffers(1, &(lse->VBO));
	if(lse->useEBO){
		glGenBuffers(1, &(lse->EBO));
	}
	
	lse->vertexShader 	= glCreateShader(GL_VERTEX_SHADER);
	lse->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	lse->program        = glCreateProgram();

	return LA_NO_ERROR;
}

LAErrorCode LAShaderCompile(LAShader *lse){
	LA_HANDLE_NULLPTR(lse, 		LA_PROPAGATE_ERROR);

	int  success;
	char infoLog[512];

	glShaderSource(lse->vertexShader, 1, (const char **)(&(lse->vertexSource)), NULL);
	glCompileShader(lse->vertexShader);
	glGetShaderiv(lse->vertexShader, GL_COMPILE_STATUS, &success);
	if(!success){
    	glGetShaderInfoLog(lse->vertexShader, 512, NULL, infoLog);
    	printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
		return LA_ERROR_OPENGL_COMPILE;
	}

	glShaderSource(lse->fragmentShader, 1, (const char **)(&(lse->fragmentSource)), NULL);
	glCompileShader(lse->fragmentShader);
	glGetShaderiv(lse->vertexShader, GL_COMPILE_STATUS, &success);
	if(!success){
    	glGetShaderInfoLog(lse->fragmentShader, 512, NULL, infoLog);
    	printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
		return LA_ERROR_OPENGL_COMPILE;
	}

	glAttachShader(lse->program, lse->vertexShader);
	glAttachShader(lse->program, lse->fragmentShader);
	glLinkProgram(lse->program);
	glGetProgramiv(lse->program, GL_LINK_STATUS, &success);
	if(!success) {
    	glGetProgramInfoLog(lse->program, 512, NULL, infoLog);
    	printf("ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n%s\n", infoLog);
	}

	return LA_NO_ERROR;
}

LAErrorCode LAShaderDestroy(LAShader *lse){
	LA_HANDLE_NULLPTR(lse, 		LA_PROPAGATE_ERROR);

	if(lse->VAO != -1U) glDeleteVertexArrays(1, &(lse->VAO));
	lse->VAO = -1U;

	if(lse->VBO != -1U) glDeleteBuffers(1, &(lse->VBO));
	lse->VBO = -1U;

	if(lse->EBO != -1U) glDeleteBuffers(1, &(lse->EBO));
	lse->EBO = -1U;

	if(lse->vertexShader != -1U) glDeleteShader(lse->vertexShader);
	lse->vertexShader = -1U;

	if(lse->fragmentShader != -1U) glDeleteShader(lse->fragmentShader);
	lse->fragmentShader = -1U;

	if(lse->program != -1U) glDeleteProgram(lse->program);
	lse->program = -1U;
	
	LAErrorCode code = LA_NO_ERROR;

	if(lse->enable != NULL){
		code = LAShaderValueDestroy(lse->enable, true);
		if(code) return code;
		lse->enable = NULL;
	}

	if(lse->disable != NULL){
		code = LAShaderValueDestroy(lse->disable, true);
		if(code) return code;
		lse->disable = NULL;
	}

	if(lse->uniform != NULL){
		code = LAShaderValueDestroy(lse->uniform, true);
		if(code) return code;
		lse->uniform = NULL;
	}

	if(lse->textures != NULL){
		code = LAShaderTextureDestroy(lse->textures, true);
		if(code) return code;
		lse->textures = NULL;
	}

	if(lse->vertex   != NULL){
		free(lse->vertex);
		lse->vertex = NULL;
	}

	if(lse->elements != NULL){
		free(lse->elements);
		lse->vertex = NULL;
	}

	return LA_NO_ERROR;
}

LAErrorCode LAShaderBind(LAShader *lse){
	LA_HANDLE_NULLPTR(lse, 		LA_PROPAGATE_ERROR);

	glBindVertexArray(lse->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, lse->VBO);

	return LA_NO_ERROR;
}

LAErrorCode LAShaderAppendEnable(LAShader *lse, size_t amount, GLuint *values, LAShaderValue **outLSV){
	LA_HANDLE_NULLPTR(lse, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(values, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	LAShaderValue *lsv = NULL;

	code = LAShaderValueAppend(&(lse->enable), LA_SHADER_TYPE_UINT32, amount, &(lsv), NULL);
	if(code) return code;

	code = LAShaderValueSetU32(lsv, 0, amount, amount, (uint32_t *)values);
	if(code) return code;

	if(outLSV != NULL) (*outLSV) = lsv;

	return code;
}

LAErrorCode LAShaderAppendDisable(LAShader *lse, size_t amount, GLuint *values, LAShaderValue **outLSV){
	LA_HANDLE_NULLPTR(lse, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(values, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	LAShaderValue *lsv = NULL;

	code = LAShaderValueAppend(&(lse->disable), LA_SHADER_TYPE_UINT32, amount, &(lsv), NULL);
	if(code) return code;

	code = LAShaderValueSetU32(lsv, 0, amount, amount, (uint32_t *)values);
	if(code) return code;

	if(outLSV != NULL) (*outLSV) = lsv;

	return code;
}

LAErrorCode LAShaderAppendUniform(LAShader *lse, LAShaderType type, size_t amount, void *value, const char *name, LAShaderValue **outLSV){
	LA_HANDLE_NULLPTR(lse, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(name, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	LAShaderValue *lsv = NULL;

	code = LAShaderValueAppend(&(lse->uniform), type, amount, &(lsv), name);
	if(code) return code;

	code = LAShaderValueCopy(lsv, 0, amount, amount, value);
	if(code) return code;

	lsv->id = glGetUniformLocation(lse->program, lsv->uniformName);

	if(outLSV != NULL) (*outLSV) = lsv;

	return code;
}

LAErrorCode LAShaderValueTraverse(LAShaderValue *lsv, LAShaderValueTraversalCallback callback){
	LA_HANDLE_NULLPTR(lsv, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(callback, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	for(size_t i = 0; lsv != NULL; i++){
		code = callback(lsv, i);
		if(code) return code;

		lsv = lsv->next;
	}

	return LA_NO_ERROR;
}

LAErrorCode LAShaderRender(LAShader *lse, bool clearVAO){
	LA_HANDLE_NULLPTR(lse, 		LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;

	glBindVertexArray(lse->VAO);
	
	if(lse->enable != NULL){
		code = LAShaderValueTraverse(lse->enable, LAShaderEnableCallback);
		if(code) return code;
	}

	if(lse->disable != NULL){
		code = LAShaderValueTraverse(lse->disable, LAShaderDisableCallback);
		if(code) return code;
	}

	glUseProgram(lse->program);

	if(lse->uniform != NULL){
		code = LAShaderValueTraverse(lse->uniform, LAShaderUniformCallback);
		if(code) return code;
	}

	if(lse->textures != NULL){
		code = LAShaderTextureTraverse(lse->textures, LAShaderTextureCallback);
		if(code) return code;
	}

	if(lse->useEBO){
		glDrawElements(lse->vertexMode, lse->elementCount, GL_UNSIGNED_INT, 0);
	} else {
		glDrawArrays(lse->vertexMode, 0, lse->vertexCount);
	}

	// Remove VAO (set to 0)
	if(clearVAO) glBindVertexArray(0);
	
	return LA_NO_ERROR;
}

LAErrorCode LAShaderSetVertex(LAShader *lse, LAShaderVertex *lsv, size_t vertexCount){
	LA_HANDLE_NULLPTR(lse, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(lsv, LA_PROPAGATE_ERROR);

	if(vertexCount == 0) return LA_ERROR_ZEROLENGTH;

	size_t size = sizeof(LAShaderVertex) * vertexCount;
	bool isLSV2Allocated = false;

	LAShaderVertex *lsv2 = (LAShaderVertex *)malloc(size);
	if(lsv2 == NULL) goto malloc_error;

	isLSV2Allocated = true;
	memcpy(lsv2, lsv, sizeof(LAShaderVertex) * vertexCount);

	if(lse->vertex != NULL) free(lse->vertex);
	lse->vertex = lsv2;
	lse->vertexCount = vertexCount; 

	glBindVertexArray(lse->VAO);  
	glBindBuffer(GL_ARRAY_BUFFER, lse->VBO);  
	glBufferData(GL_ARRAY_BUFFER, size, lse->vertex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);  
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);  

	return LA_NO_ERROR;

malloc_error:
	printf("Errored\n");
	if(isLSV2Allocated && (lsv2 != NULL)) free(lsv2);
	return LA_ERROR_MALLOC;
}

LAErrorCode LAShaderSetElement(LAShader *lse, GLuint *elements, size_t vertexCount){
	LA_HANDLE_NULLPTR(lse,      LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(elements, LA_PROPAGATE_ERROR);

	if(!(lse->useEBO))   return LA_ERROR_INVALIDVALUE;
	if(vertexCount == 0) return LA_ERROR_ZEROLENGTH;

	bool isEB2Allocated = false;
	size_t size = sizeof(GLuint) * vertexCount;
	GLuint *eb2 = (GLuint *)malloc(size);
	if(eb2 == NULL) goto malloc_error;

	isEB2Allocated = true;
	memcpy(eb2, elements, size);

	if(lse->elements != NULL) free(lse->elements);
	lse->elements = eb2;
	lse->elementCount = vertexCount; 

	glBindVertexArray(lse->VAO);  
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lse->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, lse->elements, GL_STATIC_DRAW); 

	return LA_NO_ERROR;

malloc_error:
	if(isEB2Allocated && (eb2 != NULL)) free(eb2);
	return LA_ERROR_MALLOC;
}

LAErrorCode LAShaderAppendTexture(LAShader *lse, LAShaderTextureMode tMode, LAShaderTextureDataType dType, LAShaderTextureType tType, size_t width, size_t height, LAShaderTexture **outLST, void *value1, void *value2, void *value3){
	LA_HANDLE_NULLPTR(lse, LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(value1, LA_PROPAGATE_ERROR);
	if(tMode == LA_SHADER_TEXTURE_CUBEMAP && (value2 == NULL || value3 == NULL)) return LA_ERROR_NULLPTR;

	GLuint gType = GL_UNSIGNED_BYTE;
	switch(dType){
		case LA_SHADER_TEXTURE_UINT8:	gType = GL_UNSIGNED_BYTE;
										break;
		case LA_SHADER_TEXTURE_FLOAT:	gType = GL_FLOAT;
										break;
	}

	GLuint gMode = GL_RGB;
	switch(tType){
		case LA_SHADER_TEXTURE_RGB:		gMode = GL_RGB;
										break;
		case LA_SHADER_TEXTURE_RGBA:	gMode = GL_RGBA;
										break;
		case LA_SHADER_TEXTURE_R8:		gMode = GL_RED;
										break;
	}

	GLuint gFormat = GL_RGB;
	switch(tType){
		case LA_SHADER_TEXTURE_RGB:		gFormat = GL_RGB;
										break;
		case LA_SHADER_TEXTURE_RGBA:	gFormat = GL_RGBA;
										break;
		case LA_SHADER_TEXTURE_R8:		gFormat = (gType == GL_FLOAT) ? GL_R32F : GL_R8;
										break;
	}

	LAErrorCode code = LA_NO_ERROR;
	LAShaderTexture *lst = NULL;

	code = LAShaderTextureAppend(&(lse->textures), tMode, dType, tType, width, height, &(lst));
	if(code) return code;

	code = LAShaderBind(lse);
	if(code) return code;

	glGenTextures(1, &(lst->id));

	switch(tMode){
		case LA_SHADER_TEXTURE_1D:		code = LAShaderTextureSet1d(lst, 0, width, width, value1);
										glBindTexture(GL_TEXTURE_1D, lst->id);
										glTexImage1D(GL_TEXTURE_1D, 0, gFormat, width, 0, gMode, gType, lst->data);
										break;

		case LA_SHADER_TEXTURE_2D:		code = LAShaderTextureSet2d(lst, 0, 0, width, height, width, height, value1);
										glBindTexture(GL_TEXTURE_2D, lst->id);

										glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
										glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
										glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
										glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

										glTexImage2D(GL_TEXTURE_2D, 0, gFormat, width, height, 0, gMode, gType, lst->data);
										break;

		case LA_SHADER_TEXTURE_CUBEMAP:	code = LAShaderTextureSetCubemap(lst, 0, 0, width, height, width, height, value1, value2, value3);
										glBindTexture(GL_TEXTURE_CUBE_MAP, lst->id);
										
										glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
										glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
										glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, 	GL_CLAMP_TO_EDGE);
										glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, 	GL_CLAMP_TO_EDGE);
										glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, 	GL_CLAMP_TO_EDGE);

										glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, lst->data2);
										glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, lst->data2);
										glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, lst->data2);
										glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, lst->data2);
										glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, lst->data);
										glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, lst->data3);
										break;
	}

	if(outLST != NULL) (*outLST) = lst;

	(void) gFormat;
	(void) gType;
	(void) gMode;
	return code;
}

LAErrorCode LAShaderTextureTraverse(LAShaderTexture *lst, LAShaderTextureTraversalCallback callback){
	LA_HANDLE_NULLPTR(lst, 		LA_PROPAGATE_ERROR);
	LA_HANDLE_NULLPTR(callback, LA_PROPAGATE_ERROR);

	LAErrorCode code = LA_NO_ERROR;
	for(size_t i = 0; lst != NULL; i++){
		code = callback(lst, i);
		if(code) return code;

		lst = lst->next;
	}

	return LA_NO_ERROR;
}


