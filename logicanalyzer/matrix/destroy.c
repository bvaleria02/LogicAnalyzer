#include "../liblogicanalyzer.h"
#include "matrix.h"
#include <stdlib.h>

LAErrorCode LAMatDestroy(LAMat_t *mat){
	LA_HANDLE_NULLPTR(mat, LA_PROPAGATE_ERROR);

	if(LAMatIsAllocated(mat)){
		// Matrix is in heap
		free(mat->data);
		mat->flags = mat->flags & ~(LA_MAT_IS_ALLOCATED);
	}
	// else, matrix is in stack

	mat->flags = mat->flags & ~(LA_MAT_IS_SET);
	mat->col = 0;
	mat->row = 0;
	mat->data = NULL;
	
	return LA_NO_ERROR;
}
