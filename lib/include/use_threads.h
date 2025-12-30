#ifndef _USE_THREADS_
#define _USE_THREADS_

#include <use_defs.h>
#include <use_types.h>
#include <use_utils.h>

result_t thread_create(thread_t *th, thexec_t func, ptr_t arg);

result_t create_detached_thread(thread_t *th, thexec_t func, ptr_t arg);

result_t thread_join(thread_t thread, ptr_t return_val);

#endif
