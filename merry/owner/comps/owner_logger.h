#ifndef _OWNER_LOGGER_
#define _OWNER_LOGGER_

#include <stdio.h>

#define FATAL(msg, ...)                                                  \
  fprintf(stderr, "FATAL: " msg "\n",           \
          __VA_ARGS__)
#define ERROR(msg, ...)                                                  \
  fprintf(stderr, "ERROR: " msg "\n",           \
          __VA_ARGS__)
#define NOTE(msg, ...)                                                   \
  fprintf(stdout, "NOTE: " msg "\n", __VA_ARGS__)
  
#define LOG(msg, ...)                                                    \
  fprintf(stdout, "LOG: " msg "\n", __VA_ARGS__)

#endif
