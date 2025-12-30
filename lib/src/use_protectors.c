#include <abs/os/merry_protectors.h>
#include <use_protectors.h>

_ALWAYS_INLINE_ result_t mutex_init(mutex_t *lock) {
  return merry_mutex_init(lock);
}

_ALWAYS_INLINE_ result_t cond_init(cond_t *cond) {
  return merry_cond_init(cond);
}

_ALWAYS_INLINE_ void mutex_destroy(mutex_t *mutex) {
  return merry_mutex_destroy(mutex);
}

_ALWAYS_INLINE_ void cond_destroy(cond_t *cond) {
  return merry_cond_destroy(cond);
}

_ALWAYS_INLINE_ void mutex_lock(mutex_t *mutex) {
  return merry_mutex_lock(mutex);
}

_ALWAYS_INLINE_ void mutex_unlock(mutex_t *mutex) {
  return merry_mutex_unlock(mutex);
}

_ALWAYS_INLINE_ void cond_wait(cond_t *cond, mutex_t *lock) {
  return merry_cond_wait(cond, lock);
}

_ALWAYS_INLINE_ void cond_signal(cond_t *cond) {
  return merry_cond_signal(cond);
}

_ALWAYS_INLINE_ void cond_broadcast(cond_t *cond) {
  return merry_cond_broadcast(cond);
}
