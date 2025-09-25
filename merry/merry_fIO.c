#include <merry_fIO.h>

_MERRY_ALWAYS_INLINE_ void merry_fio_prepare() {
  nort_instance_fIO = merry_get_nort();
}

msqword_t merry_fio_read(MerryFile *file, mbptr_t buf, msize_t N,
                         MerryNortRequest *req, MerryErrorStack *st) {
  merry_check_ptr(file);
  merry_check_ptr(buf);
  merry_check_ptr(req);

  msqword_t ret = 0;
  if (surelyF(N == 0))
    return 0;

  if (surelyF(file->interface_t != INTERFACE_FILE))
    return _EBADINTERFACE;

  if (file->file.blocking == mfalse) {
#ifdef _USE_LINUX_
    ret = read(file->file.fd, (mptr_t)buf, N);
    merry_error_stack_errno(st);
    ret = (ret < 0) ? -1 : ret;
#endif
  } else {
    merry_mutex_lock(nort_instance_fIO->nort_lock);
#ifdef _USE_LINUX_
    while (atomic_load_explicit(
               (_Atomic msize_t *)&nort_instance_fIO->req_count,
               memory_order_relaxed) == nort_instance_fIO->maxm_requests) {
      // The request queue is full really
      // We can only wait here
      usleep(5); // The best we can do right now
    }
    req->state = REQ_PENDING;
    struct io_uring_sqe *sqe = io_uring_get_sqe(&nort_instance_fIO->ring);
    io_uring_prep_read(sqe, file->file.fd, (mptr_t)buf, N, 0);
    io_uring_sqe_set_data(sqe, (mptr_t)req);
#endif
    atomic_fetch_add_explicit((_Atomic msize_t *)&nort_instance_fIO->req_count,
                              1, memory_order_release);
    merry_mutex_unlock(nort_instance_fIO->nort_lock);
  }
  return ret;
}

msqword_t merry_fio_write(MerryFile *file, mbptr_t buf, msize_t N,
                          MerryNortRequest *req, MerryErrorStack *st) {
  merry_check_ptr(file);
  merry_check_ptr(buf);
  merry_check_ptr(req);

  msqword_t ret = 0;
  if (surelyF(N == 0))
    return 0;

  if (surelyF(file->interface_t != INTERFACE_FILE))
    return _EBADINTERFACE;

  if (file->file.blocking == mfalse) {
#ifdef _USE_LINUX_
    ret = write(file->file.fd, (mptr_t)buf, N);
    merry_error_stack_errno(st);
    ret = (ret < 0) ? -1 : ret;
#endif
  } else {
    merry_mutex_lock(nort_instance_fIO->nort_lock);
#ifdef _USE_LINUX_
    while (atomic_load_explicit(
               (_Atomic msize_t *)&nort_instance_fIO->req_count,
               memory_order_relaxed) == nort_instance_fIO->maxm_requests) {
      // The request queue is full really
      // We can only wait here
      usleep(5); // The best we can do right now
    }
    req->state = REQ_PENDING;
    struct io_uring_sqe *sqe = io_uring_get_sqe(&nort_instance_fIO->ring);
    io_uring_prep_write(sqe, file->file.fd, (mptr_t)buf, N, 0);
    io_uring_sqe_set_data(sqe, (mptr_t)req);
#endif
    atomic_fetch_add_explicit((_Atomic msize_t *)&nort_instance_fIO->req_count,
                              1, memory_order_release);
    merry_mutex_unlock(nort_instance_fIO->nort_lock);
  }
  return ret;
}
