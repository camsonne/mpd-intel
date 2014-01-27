/*
 * Macros and routines of general use
 *
 */

#ifndef __GI_COMMON__
#define __GI_COMMON__

#include <stdio.h>

// --- verbosity macro

#define GI_VERBOSE 3 // 0 = info, 1 = errror, 2 = +warning, 3 = +message, 4 = +debug message, 5= +dump

#define GI_STR "%s[%d] "
#define GI_ARG __FUNCTION__, __LINE__
#define GI_PRINT(...) fprintf(stderr,__VA_ARGS__)

#if (GI_VERBOSE>4)
#define GI_DUM(_fmt, ...) GI_PRINT("DUM:" GI_STR _fmt, GI_ARG, ##__VA_ARGS__)
#else
#define GI_DUM(...)
#endif
#if (GI_VERBOSE>3)
#define GI_DBG(_fmt, ...) GI_PRINT("DBG:" GI_STR _fmt, GI_ARG, ##__VA_ARGS__)
#else
#define GI_DBG(...)
#endif
#if (GI_VERBOSE>2)
#define GI_MSG(_fmt, ...) GI_PRINT("MSG:" GI_STR _fmt, GI_ARG, ##__VA_ARGS__)
#else
#define GI_MSG(...)
#endif
#if (GI_VERBOSE>1)
#define GI_WRN(_fmt, ...) GI_PRINT("WRN:" GI_STR _fmt, GI_ARG, ##__VA_ARGS__)
#else
#define GI_WRN(...)
#endif
#if (GI_VERBOSE>0)
#define GI_ERR(_fmt, ...) GI_PRINT("ERR:" GI_STR _fmt, GI_ARG, ##__VA_ARGS__)
#else
#define GI_ERR(...)
#endif
#define GI_INF(_fmt, ...) GI_PRINT("INF:" GI_STR _fmt, GI_ARG, ##__VA_ARGS__)
#endif
