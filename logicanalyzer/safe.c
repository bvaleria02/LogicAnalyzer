#include "stddef.h"
#include "error.h"
#include "liblogicanalyzer.h"
#include "utils.h"

LAErrorCode LACheckedSizeAdd(size_t a, size_t b, size_t *c){
	LA_CHECK_NULLPTR(c);

	if(a > (SIZE_MAX - b)){
		return LA_ERROR_INT_OVERFLOW;
	}

	(*c) = a + b;

	return LA_NO_ERROR;
}
