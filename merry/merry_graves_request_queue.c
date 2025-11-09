#include "merry_graves_request_queue.h"

_MERRY_DEFINE_QUEUE_(GravesRequest, MerryGravesRequest *);

mret_t merry_graves_req_queue_init() {
  mresult_t res = merry_GravesRequest_llqueue_init(&g_queue.req_queue);
  if (res != MRES_SUCCESS)
    return RET_FAILURE;

  g_queue.accept_requests = mtrue;
  return RET_SUCCESS;
}

mresult_t merry_SEND_REQUEST(MerryGravesRequest *creq) {
  merry_mutex_lock(g_queue.owner_lock);
  if (g_queue.accept_requests == mfalse) {
    merry_mutex_unlock(g_queue.owner_lock);
    return MRES_OPER_NOT_AVAI;
  }

  if (merry_GravesRequest_llqueue_push(g_queue.req_queue, &creq) !=
      MRES_SUCCESS) {
    merry_cond_signal(g_queue.owner_cond);
    merry_mutex_unlock(g_queue.owner_lock);
    return MRES_COMP_FAILURE;
  }
  creq->fufilled = mfalse;
  merry_cond_signal(g_queue.owner_cond);
  merry_cond_wait(creq->used_cond, g_queue.owner_lock);
  if (creq->fufilled == mfalse) {
    // The request wasn't fulfilled and yet the core was awaken implying that
    // something interferred which is not allowed and hence we will sleep until
    // it is fulfilled
    merry_mutex_unlock(g_queue.owner_lock);
    while (creq->fufilled == mfalse) {
      usleep(10); // not the best choice
    }
    return MRES_SUCCESS; // done
  }
  merry_mutex_unlock(g_queue.owner_lock);
  return MRES_SUCCESS;
}

mresult_t merry_SEND_REQUEST_async(MerryGravesRequest *creq) {
  merry_mutex_lock(g_queue.owner_lock);
  if (g_queue.accept_requests == mfalse) {
    merry_mutex_unlock(g_queue.owner_lock);
    return MRES_OPER_NOT_AVAI;
  }
  if (merry_GravesRequest_llqueue_push(g_queue.req_queue, &creq) !=
      MRES_SUCCESS) {
    merry_cond_signal(g_queue.owner_cond);
    merry_mutex_unlock(g_queue.owner_lock);
    return MRES_COMP_FAILURE;
  }
  creq->fufilled = mfalse;
  merry_cond_signal(g_queue.owner_cond);
  merry_mutex_unlock(g_queue.owner_lock);
  return MRES_SUCCESS;
}

mresult_t merry_graves_wants_work(MerryGravesRequest **req) {
  mresult_t res = merry_GravesRequest_llqueue_pop(g_queue.req_queue, req);
  return (res == MRES_SUCCESS) ? MRES_SUCCESS : MRES_FAILURE;
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
