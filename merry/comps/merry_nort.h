#ifndef _MERRY_NORT_
#define _MERRY_NORT_

#include <merry_config.h>
#include <merry_error_stack.h>
#include <merry_platform.h>
#include <merry_protectors.h>
#include <merry_types.h>
#include <stdlib.h>

typedef struct MerryNort MerryNort;

struct MerryNort {
  msize_t maxm_requests;
  msize_t req_count;
  MerryErrorStack st;
#ifdef _USE_LINUX_
  struct io_uring ring;
#else
  // not yet
#endif
};

MerryNort *merry_initialize_nort(msize_t maxm_requests, MerryErrorStack *st);

void merry_destroy_nort(MerryNort *nort);

#endif
