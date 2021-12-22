#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>

extern FILE *logFile;

#ifndef NDEBUG
#define LOG_PRINT(...) fprintf(logFile, __VA_ARGS__); fflush(logFile);
#else
#define LOG_PRINT(...)
#endif

#endif
