#ifndef DEBUG_H
#define DEBUG_H

#include <cstdio>

using namespace std;

#define DEBUG 0

#if DEBUG
  #define LOG_PRINT(fmt) printf(fmt)
  #define LOG_PRINTF(fmt, ...) printf(fmt, __VA_ARGS__)
#else
  #define LOG_PRINT(fmt) ;
  #define LOG_PRINTF(fmt, ...) ;
#endif

#endif