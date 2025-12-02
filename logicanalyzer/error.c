#include "liblogicanalyzer.h"
#include <stdlib.h>

_Thread_local LAErrorCode	la_errno 		= LA_NO_ERROR;
_Thread_local LAFunctionName la_funcname	= NULL;
_Thread_local LAFileName 	la_filename		= NULL;
_Thread_local LALineNumber 	la_linenumber	= -1;
