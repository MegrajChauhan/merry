#include <merry_nort_request.h>

MerryNortRequest *merry_new_nort_request(mbool_t blocking, mcond_t *ocond,
                                         mmutex_t *owner_lock,
                                         MerryErrorStack *st) {
  merry_check_ptr(ocond);
  merry_check_ptr(owner_lock);
  MerryNortRequest *req = (MerryNortRequest *)malloc(sizeof(MerryNortRequest));
  if (!req) {
    PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
         "ALlocating Nort Request");
    merry_error_stack_fatality(st);
    return RET_NULL;
  }
  req->blocking = blocking;
  req->owner_cond = ocond;
  req->result = 0;
  req->state = REQ_NOT_SUBMITTED;
  req->owner_lock = owner_lock;
  return req;
}

void merry_destory_nort_request(MerryNortRequest *req) {
  merry_check_ptr(req);
  free(req);
}
