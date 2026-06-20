#include "../liblogicanalyzer.h"
#include "matrix.h"
#include <stdlib.h>

LAErrorCode LAMatInit(LAMat_t *mat){
	LA_HANDLE_NULLPTR(mat, LA_PROPAGATE_ERROR);

	mat->flags = 0x0;
	mat->col   = 0;
	mat->row   = 0;
	mat->data  = NULL;

	return LA_NO_ERROR;
}
