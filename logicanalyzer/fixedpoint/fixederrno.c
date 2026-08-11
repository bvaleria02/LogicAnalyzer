#include "fixedpoint.h"
#include <stdio.h>
#include <stdlib.h>

_Thread_local FPErrno fp_errno = FP_NO_ERROR;

FPErrorCode fixedGetErrno(){
	return fp_errno;
}

void fixedClearErrno(){
	fp_errno = FP_NO_ERROR;
}

void fixedSetErrno(FPErrorCode code){
	fp_errno = code;
}
