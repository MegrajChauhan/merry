#ifndef _USE_THREADS_
#define _USE_THREADS_

#include <use_defs.h>
#include <use_utils.h>

extern result_t thread_create(thread_t *th, thexec_t func, ptr_t arg)
    _ALIAS_(merry_thread_create);

extern result_t create_detached_thread(thread_t *th, thexec_t func, ptr_t arg)
    _ALIAS_(merry_create_detached_thread);

extern result_t thread_join(thread_t thread, ptr_t return_val)
    _ALIAS_(merry_thread_join);

#endif
