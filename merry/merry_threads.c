#include <merry_threads.h>

mret_t merry_thread_create(mthread_t *th, mthexec_t func, mptr_t arg,
                           MerryErrorStack *st) {
  merry_check_ptr(th);
  merry_check_ptr(func);
#if defined(_USE_LINUX_)
  if (pthread_create(th, NULL, func, arg) != 0) {
    PUSH(st, "System Failure", "Couldn't spawn a new thread",
         "Spawing a new thread");
    merry_error_stack_errno(st);
    return RET_FAILURE;
  }
#elif defined(_USE_WIN_)
  th = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, NULL);
  if (th == NULL) {
    PUSH(st, "System Failure", "Couldn't spawn a new thread",
         "Spawing a new thread");
    merry_error_stack_errno(st);
    return RET_FAILURE;
  }
#endif
  return RET_SUCCESS;
}

mret_t merry_create_detached_thread(mthread_t *th, mthexec_t func, mptr_t arg,
                                    MerryErrorStack *st) {
  merry_check_ptr(th);
  merry_check_ptr(func);
#if defined(_USE_LINUX_)
  pthread_attr_t attr;
  if (pthread_attr_init(&attr) != 0) {
    PUSH(st, "System Failure", "Couldn't spawn a detached thread",
         "Initializing thread attribute");
    merry_error_stack_errno(st);
    return RET_FAILURE; // we failed
  }
  if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
    PUSH(st, "System Failure", "Couldn't spawn a detached thread",
         "Setting thread state");
    merry_error_stack_errno(st);
    pthread_attr_destroy(&attr);
    return RET_FAILURE;
  }

  if (pthread_create(th, &attr, func, arg) != 0) {
    PUSH(st, "System Failure", "Couldn't spawn a detached thread",
         "Spawing a detached thread");
    merry_error_stack_errno(st);
    pthread_attr_destroy(&attr);
    return RET_FAILURE;
  }
  pthread_attr_destroy(&attr);
#elif defined(_USE_WIN_)
  th = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, NULL);
  if (th == NULL) {
    PUSH(st, "System Failure", "Couldn't spawn a detached thread",
         "Spawing a detached thread");
    merry_error_stack_errno(st);
    return RET_FAILURE;
  }
#endif
  return RET_SUCCESS;
}

mret_t merry_thread_join(mthread_t thread, mptr_t return_val) {
#if defined(_USE_LINUX_)
  pthread_join(thread, return_val);
#elif defined(_USE_WIN_)
  WaitForSingleObject(thread, INFINITE);
#endif
  return RET_SUCCESS;
}
