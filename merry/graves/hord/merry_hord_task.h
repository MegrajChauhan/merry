#ifndef _MERRY_HORD_TASK_
#define _MERRY_HORD_TASK_

#include <merry_types.h>
#include <merry_utils.h>

typedef struct MerryHordTask MerryHordTask;
typedef enum mhtaskstate_t mhtaskstate_t;

_MERRY_DEFINE_FUNC_PTR_(msqword_t, mtaskfunc_t, mptr_t, mret_t *);

enum mhtaskstate_t {
  TASK_PENDING,
  TASK_SUSPENDED,
  TASK_WORKING,
  TASK_FINISHED,
};

struct MerryHordTask {
  mhtaskstate_t state;
  mtaskfunc_t task_func;
  mptr_t task_param;
  mret_t task_res;
};

#endif
