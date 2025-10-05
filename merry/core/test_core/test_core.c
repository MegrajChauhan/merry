#include <test_core/include/test_core.h>

mptr_t merry_test_core_create(MerryCoreBase *base, maddress_t st_addr,
                              MerryErrorStack *st) {
  merry_check_ptr(base);
  MerryTestCore *core = (MerryTestCore *)malloc(sizeof(MerryTestCore));
  if (!core) {
    PUSH(st, "Memory Allocation Failed",
         "Failed to allocate memory for test core", "Initializing Test Core");
    merry_error_stack_fatality(st);
    return RET_NULL;
  }
  core->base = base;
  core->PC = st_addr;
  return (mptr_t)core;
}

void merry_test_core_delete(mptr_t core) {
  // Delete the core being confident that the core has stopped
  MerryTestCore *c = (MerryTestCore *)core;
  free(c);
}

_THRET_T_ merry_test_core_run(mptr_t c) {
  MerryTestCore *core = (MerryTestCore *)c;
  MerryCoreBase *base = core->base;
  mbool_t *intr = &base->interrupt;
  base->running = mtrue;
  merry_error_stack_init(&base->estack, base->id, base->uid, base->guid);
  base->_greq->guid = base->guid;
  base->_greq->id = base->id;
  base->_greq->uid = base->uid;
  base->_greq->sys_request = mfalse;
  while (1) {
    if (atomic_load_explicit((_Atomic mbool_t *)intr, memory_order_relaxed) ==
        mtrue) {
      // We have been interrupted
      if (base->terminate == mtrue) {
        base->running = mfalse;
        break;
      }
      *intr = mfalse;
    }
  }
  return (_THRET_T_)0;
}

MerryCoreBase *merry_test_core_base_create(MerryErrorStack *st) {
  MerryCoreBase *base = (MerryCoreBase *)malloc(sizeof(MerryCoreBase));
  if (!base) {
    PUSH(st, "Memory Allocation Failed",
         "Failed to allocate memory for CORE BASE",
         "Test Core Base Allocation");
    return RET_NULL;
  }
  base->createc = merry_test_core_create;
  base->deletec = merry_test_core_delete;
  base->execc = merry_test_core_run;
  base->getargs = merry_test_core_get_args;
  base->predel = merry_test_core_prepare_for_deletion;

  base->req_res = mtrue;
  base->running = mfalse;
  base->interrupt = mfalse;
  base->terminate = mfalse;

  // id, uid, guid is assigned by Graves
  base->type = __TEST_CORE;
  // ram and iram are assigned by Graves

  if ((base->nort = merry_initialize_nort(10, st)) == RET_NULL) {
    PUSH(st, NULL, "Failed to create CORE BASE",
         "Test Core Base Initialization");
    free(base);
    return RET_NULL;
  }

  if (!(base->_greq =
            (MerryGravesRequest *)malloc(sizeof(MerryGravesRequest)))) {
    PUSH(st, "Memory Allocation Failure",
         "Failed to allocate for graves request",
         "Test Core Base Initialization");
    merry_destroy_nort(base->nort);
    free(base);
    return RET_NULL;
  }

  if (merry_cond_init(&base->cond) == RET_FAILURE) {
    PUSH(st, "Failed to Obtain Condition Variable",
         "Couldn't obtain a condition variable",
         "Test Core Base Initialization");
    free(base->_greq);
    merry_destroy_nort(base->nort);
    free(base);
    return RET_NULL;
  }

  if ((base->task_list = merry_create_dynamic_list(10, sizeof(MerryHordTask),
                                                   st)) == RET_NULL) {
    PUSH(st, NULL, "Couldn't initialize task list for Test Core",
         "Test Core Base Initialization");
    merry_cond_destroy(&base->cond);
    free(base->_greq);
    merry_destroy_nort(base->nort);
    free(base);
    return RET_NULL;
  }

  return base;
}

void merry_test_core_base_delete(MerryCoreBase *base) {
  merry_check_ptr(base);
  // We delete RAM ourselves
  if (base->ram)
    merry_destroy_RAM(base->ram, &base->estack);
  if (base->nort)
    merry_destroy_nort(base->nort);
  if (base->_greq)
    free(base->_greq);
  merry_cond_destroy(&base->cond);
  if (base->task_list)
    merry_destroy_dynamic_list(base->task_list);

  free(base);
}

_MERRY_ALWAYS_INLINE_ MerryRequestArgs *merry_test_core_get_args(mptr_t c) {
  return &(((MerryTestCore *)c)->base->args);
}

void merry_test_core_prepare_for_deletion(mptr_t c) {
  MerryTestCore *core = (MerryTestCore *)c;
  core->base->terminate = mtrue;
  atomic_store((_Atomic mbool_t *)&core->base->interrupt, mtrue);
  while (atomic_load((_Atomic mbool_t *)&core->base->running) == mtrue)
    usleep(5);
}
