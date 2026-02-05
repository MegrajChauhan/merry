#ifndef _MERRY_UTILS_
#define _MERRY_UTILS_

#include <merry_logger.h>
#include <stdio.h>
#include <stdlib.h>

#define _stringify(x) #x
#define _glued(x, y) x##y
#define _toggle(x) (~x) >> 63
#define _MERRY_STRINGIFY_(to_str) _stringify(to_str)
#define _MERRY_CONCAT_(x, y) _glued(x, y)
#define _MERRY_TOGGLE_(x) _toggle(x) // toggle a boolean value
#define _MERRY_TO_BOOL_(x) !!(x)     // convert x to bool

// tools
// CHECK CONDITION AND IF IT FAILS, THROW ERROR AND EXIT
#define merry_check_condition(cond)                                            \
  do {                                                                         \
    if (surelyF(!(cond))) {                                                    \
      MDBG("Condition: "_MERRY_STRINGIFY_(                                     \
               (cond)) ": Failed. Line %d[FILE: %s]\n",                        \
           __LINE__, __FILE__);                                                \
      exit(-1);                                                                \
    }                                                                          \
  } while (0)

#ifdef _MERRY_RELEASE_
#define merry_assert(cond)
#else
#define _MERRY_TEST_
#define merry_assert(cond) merry_check_condition(cond)

#endif

#define merry_unreachable()                                                    \
  do {                                                                         \
    MERR("Unreachable code execution: %s %d", __FILE__, __LINE__);             \
    exit(-1);                                                                  \
  } while (0)

#endif
