#ifndef _MERRY_NORT_
#define _MERRY_NORT_

#include <merry_config.h>
#include <merry_error_stack.h>
#include <merry_nort_request.h>
#include <merry_platform.h>
#include <merry_protectors.h>
#include <merry_types.h>
#include <stdlib.h>
#include <time.h>

typedef struct MerryNort MerryNort;

struct MerryNort {
  mmutex_t *nort_lock;
  mbool_t shut_down; // Flag for Graves
#ifdef _USE_LINUX_
  struct __kernel_timespec timeout;
  struct io_uring ring;
#else
  // not yet
#endif
};

_MERRY_INTERNAL_ MerryNort nort;

_MERRY_ALWAYS_INLINE_ MerryNort *merry_get_nort();
_MERRY_ALWAYS_INLINE_ void merry_nort_shutdown();

mret_t merry_initialize_nort(MerryErrorStack *st);

_THRET_T_ merry_nort_run(mptr_t arg);

void merry_destroy_nort();

#endif
