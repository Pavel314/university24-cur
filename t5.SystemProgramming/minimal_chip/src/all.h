#ifndef ALL_H
#define ALL_H
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdint.h>
#include "nk_sdl3_defs.h"
#include "res/chip_resources.h"
#include "romdb/romdb.h"

typedef void (*free_fn)(void*);

#define COUNTOF_192F(arr) (sizeof(arr) / sizeof((arr)[0]))
#define UNUSED_192F(x) ((void)(x))

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
#define THREAD_LOCAL_192F thread_local
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define THREAD_LOCAL_192F _Thread_local
#elif defined(_MSC_VER)
#define THREAD_LOCAL_192F __declspec(thread)
#elif defined(__GNUC__)
#define THREAD_LOCAL_192F __thread
#else
#define THREAD_LOCAL_192F
#endif


/*END OF ALL_H*/
#endif