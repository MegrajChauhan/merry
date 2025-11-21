#include <merry_process.h>

mresult_t merry_spawn_process(mpid_t *res, mstr_t prog) {
  if (!res)
    return MRES_INVALID_ARGS;
#ifdef _USE_LINUX_
  *res = fork();
  if (*res == -1)
    return MRES_SYS_FAILURE;
  if (*res == 0) {
    // in the child
    execv(prog, &prog);
    return MRES_SYS_FAILURE; // we have failed
  }
#else
  // not yet defined
#endif
  return MRES_SUCCESS;
}

mresult_t merry_spawn_process_env(mpid_t *res, mstr_t prog, mstr_t* env) {
  if (!res)
    return MRES_INVALID_ARGS;
#ifdef _USE_LINUX_
  *res = fork();
  if (*res == -1)
    return MRES_SYS_FAILURE;
  if (*res == 0) {
    // in the child
    execvpe(prog, &prog, env);
    return MRES_SYS_FAILURE; // we have failed
  }
#else
  // not yet defined
#endif
  return MRES_SUCCESS;
}
