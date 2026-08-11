#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../liblogicanalyzer.h"
#include "../error.h"
#include "../utils.h"
#include "matrix.h"

LAErrorCode LAMatTranspose(LAMat_t *m){
	LA_HANDLE_NULLPTR(m, LA_PROPAGATE_ERROR);

	if(!(LAMatIsValid(m))) 					return LA_ERROR_MATRIX;

	m->flags = m->flags ^ LA_MAT_TRANSPOSED;

	return LA_NO_ERROR;
}
