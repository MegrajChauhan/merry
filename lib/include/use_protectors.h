#ifndef _USE_PROTECTORS_
#define _USE_PROTECTORS_

#include <use_defs.h>
#include <use_utils.h>

result_t mutex_init(mutex_t *lock);

result_t cond_init(cond_t *cond);

void mutex_destroy(mutex_t *mutex);

void cond_destroy(cond_t *cond);

void mutex_lock(mutex_t *mutex);

void mutex_unlock(mutex_t *mutex);

void cond_wait(cond_t *cond, mutex_t *lock);

void cond_signal(cond_t *cond);

void cond_broadcast(cond_t *cond);

#endif
