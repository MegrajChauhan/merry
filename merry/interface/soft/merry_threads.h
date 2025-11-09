#ifndef _MERRY_THREADS_
#define _MERRY_THREADS_

#include <merry_config.h>
#include <merry_operations.h>
#include <merry_platform.h>
#include <merry_types.h>
#include <merry_utils.h>

mresult_t merry_thread_create(mthread_t *th, mthexec_t func, mptr_t arg);

mresult_t merry_create_detached_thread(mthread_t *th, mthexec_t func,
                                       mptr_t arg);

mresult_t merry_thread_join(mthread_t thread, mptr_t return_val);

#endif
