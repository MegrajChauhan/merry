#ifndef _MERRY_OPERATIONS_
#define _MERRY_OPERATIONS_

typedef enum mresult_t mresult_t;

enum mresult_t {
  MRES_FAILURE,
  MRES_NOT_MERRY_FAILURE,
  MRES_SUCCESS,
  MRES_SYS_FAILURE, // glibc or syscall failed
  MRES_NOT_EXISTS,  // if something is accessed but it doesn't exist
  MRES_CONT_FULL,
  MRES_CONT_EMPTY,
  MRES_INVALID_ARGS,
  MRES_OPER_NOT_AVAI,
  MRES_OPER_NOT_PERM,
  MRES_COMP_FAILURE, // sometimes we don't want to propagate other messages
  MRES_RESOURCE_LIM_REACHED,
  MRES_INTERRUPTED,
};

#endif
