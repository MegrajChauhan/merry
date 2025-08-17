#include "merry_graves_request_queue.h"

mret_t merry_graves_req_queue_init(mcond_t *graves_cond, MerryErrorStack *st) {
  if (merry_cond_init(&g_queue.queue_cond) == RET_FAILURE) {
    PUSH(st, "System Error", "Failed to obtain condition variable",
         "Initializing Request Queue");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }
  if (merry_mutex_init(&g_queue.queue_lock) == RET_FAILURE) {
    PUSH(st, "System Error", "Failed to obtain mutex lock",
         "Initializing Request Queue");
    merry_error_stack_fatality(st);
    merry_cond_destroy(&g_queue.queue_cond);
    return RET_FAILURE;
  }

  if ((g_queue.req_queue = merry_dynamic_queue_init(st)) == RET_NULL) {
    merry_cond_destroy(&g_queue.queue_cond);
    merry_mutex_destroy(&g_queue.queue_lock);
    return RET_FAILURE;
  }

  g_queue.graves_cond = graves_cond;
  g_queue.accept_requests = mtrue;
  return RET_SUCCESS;
}

mret_t merry_SEND_REQUEST(MerryGravesRequest *creq, mcond_t *cond) {
  merry_mutex_lock(&g_queue.queue_lock);
  if (g_queue.accept_requests == mfalse) {
    merry_mutex_unlock(&g_queue.queue_lock);
    return RET_FAILURE;
  }

  /*
   * We have decided to be strict
   * */
  if (merry_dynamic_queue_push(g_queue.req_queue, creq, &g_queue_stack) ==
      RET_FAILURE) {
    PUSH(&g_queue_stack, NULL, "Cannot PUSH to the request queue",
         "Pushing a request to request queue");
    merry_error_stack_fatality(&g_queue_stack);
    merry_dynamic_queue_clear(g_queue.req_queue);
    g_queue.accept_requests = mfalse;
    merry_cond_signal(g_queue.graves_cond);
    return RET_FAILURE;
  }

  merry_cond_signal(g_queue.graves_cond);
  merry_cond_wait(cond, &g_queue.queue_lock);
  merry_mutex_unlock(&g_queue.queue_lock);
  return RET_SUCCESS;
}

mret_t merry_graves_wants_work(MerryGravesRequest **req) {
  merry_mutex_lock(&g_queue.queue_lock);
  mret_t res = merry_dynamic_queue_pop(g_queue.req_queue, (mptr_t *)req);
  merry_mutex_unlock(&g_queue.queue_lock);
  return res;
}

MerryErrorStack *merry_graves_get_req_queue_error_stack() {
  return &g_queue_stack;
}

void merry_graves_req_queue_free() {
  merry_dynamic_queue_destroy(g_queue.req_queue);
  merry_cond_destroy(&g_queue.queue_cond);
  merry_mutex_destroy(&g_queue.queue_lock);
}
