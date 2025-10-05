#ifndef _MERRY_LOGGER_
#define _MERRY_LOGGER_

#include <merry_config.h>
#include <stdio.h>

#define MFATAL(src, msg, ...)                                                  \
  fprintf(stderr, "FATAL[<%s>]: " msg "\n", (src) ? src : "UNKNOWN",           \
          __VA_ARGS__)
#define MERROR(src, msg, ...)                                                  \
  fprintf(stderr, "ERROR[<%s>]: " msg "\n", (src) ? src : "UNKNOWN",           \
          __VA_ARGS__)
#define MNOTE(src, msg, ...)                                                   \
  fprintf(stdout, "NOTE[<%s>]: " msg "\n", (src) ? src : "UNKNOWN", __VA_ARGS__)
#ifndef _MERRY_RELEASE_
#define MLOG(src, msg, ...)                                                    \
  fprintf(stdout, "LOG[<%s>]: " msg "\n", (src) ? src : "UNKNOWN", __VA_ARGS__)
#else
#define MLOG(src, msg, ...)
#endif

#endif
