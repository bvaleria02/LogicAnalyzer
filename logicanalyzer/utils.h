#ifndef LA_UTIS
#define LA_UTILS

#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define LA_VALUE_ERROR 		-3

#define LA_CLAMP_VALUE(__value, __min, __max) do{	\
	if((__value) < (__min)) (__value) = (__min);	\
	if((__value) > (__max)) (__value) = (__max);	\
} while(0)

#define LA_GET_PARAMETER(__index, __paramList, __paramCount) ((__index) < (__paramCount) && (__index) >= 0) ? ((__paramList)[__index]) : 0

#define LA_USE_VAR(__var) (void) (__var)

#define LA_PROFILER(__func, __name) do{							\
	{													\
		struct timespec __startTime;					\
		clock_gettime(CLOCK_MONOTONIC, &__startTime);	\
		struct timespec __endTime;						\
		{__func}										\
		clock_gettime(CLOCK_MONOTONIC, &__endTime);		\
		printf("Time spent (%s): %le seconds\n", __name, ((__endTime.tv_sec + (__endTime.tv_nsec / (double) 1000000000)) - (__startTime.tv_sec + (__startTime.tv_nsec / (double) 1000000000))));	\
	}													\
} while(0)

// Try - except - else-like block
// Auto handles mutex close
// closes mutex BEFORE else, except and finally
// Only intended for single use
// 
// - Always free/close in finally block
// - Except2 block is the intended safe way to return/break/continue
// - exc1 handles pre-finally exception (EG: print error)
// - exc2 handles post-finally exception, like a return/break/continue
// - Else runs if there is no exception
// - _excv should be a bool or int variable
//
// Please use LA_MUTEX if you want the simpler version
#define LA_WITH_MUTEX(__mut, _excv, __try, __exc1, __exc2,__els, __fin) do{  \
  /* General lock */                                                 \
  pthread_mutex_lock(&(__mut));                                      \
                                                                     \
  /* "Try" block                                                     \
      THREAD SAFE         */                                         \
  do {                                                               \
    __try                                                            \
  } while(0);                                                        \
                                                                     \
  /* General unlock */                                               \
  pthread_mutex_unlock(&(__mut));                                    \
                                                                     \
  if((_excv)){                                                       \
    /* Handle exception; "Except" block                              \
       NO THREAD SAFE  */                                            \
    do {                                                             \
      __exc1                                                         \
      __fin                                                          \
      __exc2                                                         \
    } while(0);                                                      \
  } else {                                                           \
    /* Executes if there is no exception. Else block                 \
       NO THREAD SAFE */                                             \
    do {                                                             \
      __els                                                          \
      __fin                                                          \
    } while(0);                                                      \
  }                                                                  \
} while(0)


// Handle mutex
// No error handling
#define LA_MUTEX(__mut, __code) do {                                 \
  /* General lock */                                                 \
  pthread_mutex_lock((__mut));                                      \
                                                                     \
  /* "Try" block                                                     \
     THREAD SAFE */                                                  \
  do {                                                               \
    __code                                                           \
  } while(0);                                                        \
                                                                     \
  /* General unlock */                                               \
  pthread_mutex_unlock((__mut));                                    \
} while(0)

#define LA_MATRIX_INDEX(c, r, h) ((r)*(h) + (c))

#endif //LA_UTILS
