#include <merry_nort.h>

_MERRY_ALWAYS_INLINE_ MerryNort *merry_get_nort() { return &nort; }

_MERRY_ALWAYS_INLINE_ void merry_nort_shutdown() {
  atomic_store_explicit((_Atomic mbool_t *)&nort.shut_down, mtrue,
                        memory_order_release);
}

_MERRY_ALWAYS_INLINE_ MerryErrorStack *merry_get_nort_error_stack() {
  return &nort.st;
}

mret_t merry_initialize_nort(mcond_t *graves_cond, mbool_t *notify_graves,
                             MerryErrorStack *st) {
  if (merry_mutex_init(nort.nort_lock) != RET_SUCCESS) {
    PUSH(st, "Lock Init Failed", "Failed to obtain LOCK", "Initializing Nort");
    merry_error_stack_errno(st);
    return RET_FAILURE;
  }
#ifdef _USE_LINUX_
  if (io_uring_queue_init(10 /*customizable*/, &nort.ring, 0) < 1) {
    PUSH(st, "IO_URING Initialization", "Failed to initialize the io_uring",
         "Initializing Nort");
    merry_error_stack_fatality(st);
    merry_mutex_destroy(nort.nort_lock);
    return RET_FAILURE;
  }
  nort.timeout.tv_sec = 0;
  nort.timeout.tv_nsec = 5000000; // 5ms
#else
  // not yet
#endif
  merry_error_stack_init(&nort.st, -1, -1, -1);
  nort.shut_down = mfalse;
  nort.maxm_requests = 10;
  nort.req_count = 0;
  nort.graves_cond = graves_cond;
  nort.notify_graves = notify_graves;
  return RET_SUCCESS;
}

void merry_nort_handle_issue() {
  atomic_store_explicit((_Atomic mbool_t *)nort.notify_graves, mtrue,
                        memory_order_release);
  while (atomic_load_explicit((_Atomic mbool_t *)nort.notify_graves,
                              memory_order_relaxed) == mtrue) {
    merry_cond_signal(nort.graves_cond);
    usleep(5);
  }
}

_THRET_T_ merry_nort_run(mptr_t arg) {
  while (1) {
    if (atomic_load_explicit((_Atomic mbool_t *)&nort.shut_down,
                             memory_order_relaxed) == mtrue)
      break;
    while (atomic_load_explicit((_Atomic msize_t *)&nort.req_count,
                                memory_order_relaxed) == 0)
      usleep(5);
#ifdef _USE_LINUX_
    merry_mutex_lock(nort.nort_lock);
    if (io_uring_submit(&nort.ring) < 0) {
      PUSH(&nort.st, "IO_URING", "System error- couldn't submit",
           "Nort Running");
      merry_nort_handle_issue();
      break;
    }
    merry_mutex_unlock(nort.nort_lock);
    struct io_uring_cqe *_areq;
    if (io_uring_wait_cqe_timeout(&nort.ring, &_areq, &nort.timeout) < 0) {
      PUSH(&nort.st, "IO_URING", "System error- couldn't wait", "Nort Running");
      merry_nort_handle_issue();
      break;
    }

    MerryNortRequest *req = io_uring_cqe_get_data(_areq);
    req->result = _areq->res;
    atomic_store_explicit((_Atomic mnreqstate_t *)&req->state, REQ_COMPLETED,
                          memory_order_release);
    merry_mutex_lock(req->owner_lock);
    merry_cond_signal(req->owner_cond); // if the owner is waiting
    merry_mutex_unlock(req->owner_lock);
    io_uring_cqe_seen(&nort.ring, _areq);
    atomic_fetch_sub_explicit((_Atomic msize_t *)&nort.req_count, 1,
                              memory_order_relaxed);
#endif
  }
  return (_THRET_T_)0;
}

void merry_destroy_nort() {
  merry_mutex_destroy(nort.nort_lock);
#ifdef _USE_LINUX_
  io_uring_queue_exit(&nort.ring);
#endif
}
