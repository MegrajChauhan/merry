#include "merry_graves_request_queue.h"

_MERRY_DEFINE_QUEUE_(GravesRequest, MerryGravesRequest);

mret_t merry_graves_req_queue_init() {

  if ((g_queue.req_queue = merry_GravesRequest_llqueue_init()) == RET_NULL) {
    return RET_FAILURE;
  }

  g_queue.accept_requests = mtrue;
  return RET_SUCCESS;
}

mret_t merry_SEND_REQUEST(MerryGravesRequest *creq, mcond_t *cond) {
  merry_mutex_lock(g_queue.owner_lock);
  if (g_queue.accept_requests == mfalse) {
    merry_mutex_unlock(g_queue.owner_lock);
    return RET_FAILURE;
  }

  /*
   * We have decided to be strict
   * */
  if (merry_GravesRequest_llqueue_push(g_queue.req_queue, creq) ==
      RET_FAILURE) {
    merry_cond_signal(g_queue.owner_cond);
    merry_mutex_unlock(g_queue.owner_lock);
    return RET_FAILURE;
  }
  merry_cond_signal(g_queue.owner_cond);
  merry_cond_wait(cond, g_queue.owner_lock);
  merry_mutex_unlock(g_queue.owner_lock);
  return RET_SUCCESS;
}

mret_t merry_graves_wants_work(MerryGravesRequest *req) {
  mret_t res = merry_GravesRequest_llqueue_pop(g_queue.req_queue, req);
  return res;
}

void merry_graves_req_no_more_requests() {
  merry_mutex_lock(g_queue.owner_lock);
  g_queue.accept_requests = mfalse;
  merry_mutex_unlock(g_queue.owner_lock);
}

void merry_graves_req_queue_free() {
  merry_GravesRequest_llqueue_destroy(g_queue.req_queue);
}

void merry_graves_req_register_wakeup(mcond_t *owner_cond,
                                      mmutex_t *owner_lock) {
  g_queue.owner_cond = owner_cond;
  g_queue.owner_lock = owner_lock;
}
