#include <merry_nort.h>

MerryNort *merry_initialize_nort(msize_t maxm_requests) {
  merry_check_condition_and_exit_on_failure(maxm_requests > 0);
  MerryNort *nort = (MerryNort *)malloc(sizeof(MerryNort));
  if (!nort) {
    return RET_NULL;
  }
#ifdef _USE_LINUX_
  if (io_uring_queue_init(maxm_requests, &nort->ring, 0) < 1) {
    free(nort);
    return RET_FAILURE;
  }
#else
  // not yet
#endif
  nort->maxm_requests = maxm_requests;
  nort->req_count = 0;
  return nort;
}

void merry_destroy_nort(MerryNort *nort) {
#ifdef _USE_LINUX_
  io_uring_queue_exit(&nort->ring);
#endif
  free(nort);
}
