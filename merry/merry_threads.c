#include <merry_threads.h>

mresult_t merry_thread_create(mthread_t *th, mthexec_t func, mptr_t arg) {
  merry_check_ptr(th);
  merry_check_ptr(func);
#if defined(_USE_LINUX_)
  if (pthread_create(th, NULL, func, arg) != 0) {
    return MRES_SYS_FAILURE;
  }
#elif defined(_USE_WIN_)
  th = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, NULL);
  if (th == NULL) {
    return MRES_SYS_FAILURE;
  }
#endif
  return MRES_SUCCESS;
}

mresult_t merry_create_detached_thread(mthread_t *th, mthexec_t func,
                                       mptr_t arg) {
  merry_check_ptr(th);
  merry_check_ptr(func);
#if defined(_USE_LINUX_)
  pthread_attr_t attr;
  if (pthread_attr_init(&attr) != 0) {
    return MRES_SYS_FAILURE; // we failed
  }
  if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
    pthread_attr_destroy(&attr);
    return MRES_SYS_FAILURE;
  }

  if (pthread_create(th, &attr, func, arg) != 0) {
    pthread_attr_destroy(&attr);
    return MRES_SYS_FAILURE;
  }
  pthread_attr_destroy(&attr);
#elif defined(_USE_WIN_)
  th = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, NULL);
  if (th == NULL)
    return MRES_SYS_FAILURE;
#endif
  return MRES_SUCCESS;
}

mresult_t merry_thread_join(mthread_t thread, mptr_t return_val) {
#if defined(_USE_LINUX_)
  pthread_join(thread, return_val);
#elif defined(_USE_WIN_)
  WaitForSingleObject(thread, INFINITE);
#endif
  return MRES_SUCCESS;
}
