#ifndef _USE_PROTECTORS_
#define _USE_PROTECTORS_

#include <merry_protectors.h>
#include <use_defs.h>
#include <use_utils.h>

extern result_t mutex_init(mutex_t *lock) _ALIAS_(merry_mutex_init);

extern result_t cond_init(cond_t *cond) _ALIAS_(merry_cond_init);

extern void mutex_destroy(mutex_t *mutex) _ALIAS_(merry_mutex_destroy);

extern void cond_destroy(cond_t *cond) _ALIAS_(merry_cond_destroy);

extern void mutex_lock(mutex_t *mutex) _ALIAS_(merry_mutex_lock);

extern void mutex_unlock(mutex_t *mutex) _ALIAS_(merry_mutex_unlock);

extern void cond_wait(cond_t *cond, mutex_t *lock) _ALIAS_(merry_cond_wait);

extern void cond_signal(cond_t *cond) _ALIAS_(merry_cond_signal);

extern void cond_broadcast(cond_t *cond) _ALIAS_(merry_cond_broadcast);

#endif
