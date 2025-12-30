#include <abs/os/merry_threads.h>
#include <use_threads.h>

_ALWAYS_INLINE_ result_t thread_create(thread_t *th, thexec_t func, ptr_t arg) {
  return merry_thread_create(th, func, arg);
}

_ALWAYS_INLINE_ result_t create_detached_thread(thread_t *th, thexec_t func,
                                                ptr_t arg) {
  return merry_create_detached_thread(th, func, arg);
}

_ALWAYS_INLINE_ result_t thread_join(thread_t thread, ptr_t return_val) {
  return merry_thread_join(thread, return_val);
}
