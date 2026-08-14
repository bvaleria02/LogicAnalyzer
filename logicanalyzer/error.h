#ifndef LA_ERROR
#define LA_ERROR

#include <stddef.h>

#ifndef LAErrorCode
	typedef enum _la_error_code LAErrorCode;
#endif

enum _la_error_code {
	LA_NO_ERROR						          = 0,
	LA_ERROR_NULLPTR				        = 1,
	LA_ERROR_MALLOC					        = 2,
	LA_ERROR_NOBUCKET				        = 3,
	LA_ERROR_FILE					          = 4,
	LA_ERROR_VALUEREAD				      = 5,
	LA_ERROR_OUTOFRANGE				      = 6,
	LA_ERROR_FILEREAD				        = 7,
	LA_ERROR_INCORRECTVALUE			    = 8,
	LA_ERROR_FILENOTFOUND			      = 9,
	LA_ERROR_MMAP					          = 10,
	LA_ERROR_MUNMAP					        = 11,
	LA_ERROR_INVALIDSYNTAX			    = 12,
	LA_ERROR_INVALIDVALUE			      = 13,
	LA_ERROR_ZEROLENGTH				      = 14,
	LA_ERROR_MATRIX					        = 15,
	LA_ERROR_NONMATCHING_DIMENSION 	= 16,
	LA_ERROR_PERMISSIONS 			      = 17,
	LA_ERROR_NONINVERTIBLE_MATRIX   = 18,
	LA_ERROR_BIG_INT				        = 19,
	LA_ERROR_ZERODIV				        = 20,
	LA_ERROR_OPENGL_SHADERS			    = 21,
	LA_ERROR_OPENGL_COMPILE			    = 22,
	LA_ERROR_OUTOFBOUND				      = 23,
	LA_ERROR_VTABLE_BASE				    = 24,
	LA_ERROR_MAX_LENGTH				      = 25,
	LA_ERROR_INT_OVERFLOW				    = 26,
	LA_ERROR_INT_UNDERFLOW			    = 27,
	LA_ERROR_PARSER_LENGTH          = 28,
	LA_ERROR_MISMATCH_FCS           = 29
};

typedef const char *LAFunctionName;
typedef const char *LAFileName;
typedef       int   LALineNumber;

extern _Thread_local LAErrorCode	  la_errno;
extern _Thread_local LAFunctionName la_funcname;
extern _Thread_local LAFileName 	  la_filename;
extern _Thread_local LALineNumber 	la_linenumber;

#define LA_PROPAGATE_ERROR 	-1
#define LA_NO_RETURN 		    -2

#define LA_RAISE_ERROR(__errorCode) do{		\
	la_errno		   =__errorCode;			\
	la_funcname 	 = __func__;				\
	la_filename 	 = __FILE__;				\
	la_linenumber  = __LINE__;				\
} while(0)

// Deprecated, use LA_CHECK_NULLPTR
#define LA_HANDLE_NULLPTR(__ptr, __returnValue) do{		\
	if(__ptr == NULL){									\
		LA_RAISE_ERROR(LA_ERROR_NULLPTR);				\
														\
		if(__returnValue == LA_NO_RETURN){				\
			return 0;									\
		} else if(__returnValue == LA_PROPAGATE_ERROR){	\
			return LA_ERROR_NULLPTR;					\
		}												\
		return __returnValue;							\
	}													\
} while(0)

#define LA_CHECK_NULLPTR(__ptr) do{   \
  if((__ptr) == NULL){                \
    LA_RAISE_ERROR(LA_ERROR_NULLPTR); \
    return LA_ERROR_NULLPTR;          \
  }                                   \
} while(0)

#endif //LA_ERROR
