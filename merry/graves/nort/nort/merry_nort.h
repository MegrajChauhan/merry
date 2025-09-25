#ifndef _MERRY_NORT_
#define _MERRY_NORT_

#include <merry_config.h>
#include <merry_error_stack.h>
#include <merry_nort_request.h>
#include <merry_platform.h>
#include <merry_protectors.h>
#include <merry_types.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <time.h>

typedef struct MerryNort MerryNort;

struct MerryNort {
  mmutex_t *nort_lock;
  mbool_t shut_down; // Flag for Graves
  msize_t maxm_requests;
  msize_t req_count;
  mbool_t *notify_graves;
  mcond_t *graves_cond;
  MerryErrorStack st;
#ifdef _USE_LINUX_
  struct __kernel_timespec timeout;
  struct io_uring ring;
#else
  // not yet
#endif
};

_MERRY_INTERNAL_ MerryNort nort;

MerryNort *merry_get_nort();
void merry_nort_shutdown();
MerryErrorStack *merry_get_nort_error_stack();

void merry_nort_handle_issue();

mret_t merry_initialize_nort(mcond_t *graves_cond, mbool_t *gflag,
                             MerryErrorStack *st);

_THRET_T_ merry_nort_run(mptr_t arg);

void merry_destroy_nort();

#endif
