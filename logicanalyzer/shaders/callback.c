#include "../liblogicanalyzer.h"
#include "shaders.h"
#include <stdint.h>
#include <stdlib.h>
#include "../glad/glad/glad.h"

LAErrorCode LAShaderEnableCallback(LAShaderValue *lsv, size_t n){
	LA_HANDLE_NULLPTR(lsv, LA_PROPAGATE_ERROR);

	GLuint *values = (GLuint *)lsv->value;
	for(size_t i = 0; i < lsv->amount; i++) glEnable(values[i]);

	(void) n;
	return LA_NO_ERROR;
}

LAErrorCode LAShaderDisableCallback(LAShaderValue *lsv, size_t n){
	LA_HANDLE_NULLPTR(lsv, LA_PROPAGATE_ERROR);

	GLuint *values = (GLuint *)lsv->value;
	for(size_t i = 0; i < lsv->amount; i++) glDisable(values[i]);

	(void) n;
	return LA_NO_ERROR;
}

LAErrorCode LAShaderUniformCallback(LAShaderValue *lsv, size_t n){
	LA_HANDLE_NULLPTR(lsv, LA_PROPAGATE_ERROR);

	if(!(lsv->dirty)) return LA_NO_ERROR;
	
	switch(lsv->type){
		case LA_SHADER_TYPE_UINT8	  : glUniform1ui(lsv->id, (GLuint) ((uint8_t *)lsv->value)[0]);
										break;
		case LA_SHADER_TYPE_INT8	  : glUniform1i(lsv->id, (GLint) ((int8_t *)lsv->value)[0]);
										break;
		case LA_SHADER_TYPE_UINT16	  : glUniform1ui(lsv->id, (GLuint) ((uint16_t *)lsv->value)[0]);
										break;
		case LA_SHADER_TYPE_INT16	  : glUniform1i(lsv->id, (GLint) ((int16_t *)lsv->value)[0]);
										break;
		case LA_SHADER_TYPE_UINT32	  : glUniform1ui(lsv->id, ((GLuint *)lsv->value)[0]);
										break;
		case LA_SHADER_TYPE_INT32	  : glUniform1i(lsv->id, ((GLint *)lsv->value)[0]);
										break;
		case LA_SHADER_TYPE_FLOAT	  : glUniform1f(lsv->id, ((GLfloat *)lsv->value)[0]);
										break;
		case LA_SHADER_TYPE_VEC2	  : glUniform2fv(lsv->id, lsv->amount, ((GLfloat *)lsv->value));
										break;
		case LA_SHADER_TYPE_VEC3	  : glUniform3fv(lsv->id, lsv->amount, ((GLfloat *)lsv->value));
										break;
		case LA_SHADER_TYPE_VEC4	  : glUniform4fv(lsv->id, lsv->amount, ((GLfloat *)lsv->value));
										break;
		case LA_SHADER_TYPE_MAT2	  : glUniformMatrix2fv(lsv->id, lsv->amount, GL_FALSE, ((GLfloat *)lsv->value));
										break;
		case LA_SHADER_TYPE_MAT3	  : glUniformMatrix3fv(lsv->id, lsv->amount, GL_FALSE, ((GLfloat *)lsv->value));
										break;
		case LA_SHADER_TYPE_MAT4	  : glUniformMatrix4fv(lsv->id, lsv->amount, GL_FALSE, ((GLfloat *)lsv->value));
										break;
	}

	lsv->dirty = false;
	(void) n;
	return LA_NO_ERROR;
}

LAErrorCode LAShaderTextureCallback(LAShaderTexture *lst, size_t n){
	LA_HANDLE_NULLPTR(lst, LA_PROPAGATE_ERROR);

	glActiveTexture(GL_TEXTURE0);
	switch(lst->tMode){
		case LA_SHADER_TEXTURE_1D		: glBindTexture(GL_TEXTURE_1D, lst->id);
									  	  break;
		case LA_SHADER_TEXTURE_2D		: glBindTexture(GL_TEXTURE_2D, lst->id);
									  	  break;
		case LA_SHADER_TEXTURE_CUBEMAP	: glBindTexture(GL_TEXTURE_CUBE_MAP, lst->id);
									  	  break;
	}

	// Handle update
	if(!(lst->dirty)) return LA_NO_ERROR;

	printf("Update\n");
	
	GLuint gType = GL_UNSIGNED_BYTE;
	switch(lst->dType){
		case LA_SHADER_TEXTURE_UINT8:	gType = GL_UNSIGNED_BYTE;
										break;
		case LA_SHADER_TEXTURE_FLOAT:	gType = GL_FLOAT;
										break;
	}

	GLuint gMode = GL_RGB;
	switch(lst->tType){
		case LA_SHADER_TEXTURE_RGB:		gMode = GL_RGB;
										break;
		case LA_SHADER_TEXTURE_RGBA:	gMode = GL_RGBA;
										break;
		case LA_SHADER_TEXTURE_R8:		gMode = GL_RED;
										break;
	}
	
	switch(lst->tMode){
		case LA_SHADER_TEXTURE_1D		    : glTexSubImage1D(GL_TEXTURE_1D, 0, 0, lst->width, gMode, gType, lst->data);
									  	  							break;
		case LA_SHADER_TEXTURE_2D		    : glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, lst->width, lst->height, gMode, gType, lst->data);
									  	 								break;
		case LA_SHADER_TEXTURE_CUBEMAP	: 
																			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 0, 0, lst->width, lst->height, gMode, gType, lst->data2);
																			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, lst->width, lst->height, gMode, gType, lst->data2);
																			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 0, 0, lst->width, lst->height, gMode, gType, lst->data2);
																			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0, 0, lst->width, lst->height, gMode, gType, lst->data2);
																			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 0, 0, lst->width, lst->height, gMode, gType, lst->data3);
																			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0, 0, lst->width, lst->height, gMode, gType, lst->data);
									  	  							break;
	}
	lst->dirty = false;
	
	(void) n;
	return LA_NO_ERROR;
}

