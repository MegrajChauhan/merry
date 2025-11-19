#ifndef _MERRY_PROCESS_
#define _MERRY_PROCESS_

#include <merry_config.h>
#include <merry_operations.h>
#include <merry_platform.h>
#include <merry_types.h>

mresult_t merry_spawn_process(mpid_t *res, mstr_t prog);

mresult_t merry_spawn_process_env(mpid_t *res, mstr_t prog, mstr_t* env);

#endif
