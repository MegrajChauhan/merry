#include <merry_hord.h>

mret_t merry_hord_init(MerryErrorStack *st) {
  // Initialize each worker and then start them
  if (merry_mutex_init(&shared_lock) == RET_FAILURE) {
    PUSH(st, "Failed to Obtain Lock", "HORD couldn't initialize",
         "HORD Initialization");
    return RET_FAILURE;
  }
  for (msize_t i = 0; i < 5; i++) {
    MerryHord *h = &hords[i];
    if (merry_cond_init(&h->hord_cond) == RET_FAILURE) {
      PUSH(st, "Failed to Obtain Condition Variable",
           "HORDs couldn't initialize", "HORDs Initialization");
      return RET_FAILURE;
    }
    if ((task_pool[i] = merry_dynamic_queue_init(st)) == RET_NULL) {
      PUSH(st, NULL, "HORDs couldn't initialize", "HORDs Initialization");
      return RET_FAILURE;
    }
    h->total_work_queued = 0;
    h->hord_suspended = mtrue;
    h->hord_terminate = mfalse;
    h->st_ind = i;
    mthread_t th;
    if (merry_create_detached_thread(&th, merry_hord_run, h, st) ==
        RET_FAILURE) {
      PUSH(st, "Failed to Start Hord", "HORD couldn't initialize",
           "HORD Initialization");
      return RET_FAILURE;
    }
  }
  return RET_SUCCESS;
}

_THRET_T_ merry_hord_run(mptr_t hord) {
  MerryHord *h = (MerryHord *)hord;
  MerryDynamicQueue *my_tasks = task_pool[h->st_ind];
  msize_t curr_queue = 0;
  while (mtrue) {
    h->total_work_queued = my_tasks->data_count;
    if (atomic_load_explicit((_Atomic mbool_t *)&h->hord_terminate,
                             memory_order_relaxed))
      break;
    if (h->total_work_queued == 0) {
      h->hord_suspended = mtrue;
      merry_cond_wait(&h->hord_cond, &shared_lock);
    }
    MerryHordTask task;
    merry_dynamic_queue_pop(my_tasks, (mptr_t)&task);
    atomic_store_explicit((_Atomic mhtaskstate_t *)&task->state, TASK_WORKING,
                          memory_order_release);
    msqword_t res = task->task_func(task->task_param, &task->task_res);
    switch (res) {
    case -1: // The task has suspended itself
    {
      atomic_store_explicit((_Atomic mhtaskstate_t *)&task->state,
                            TASK_SUSPENDED, memory_order_release);
      break;
    }
    case 0:
      atomic_store_explicit((_Atomic mhtaskstate_t *)&task->state,
                            TASK_FINISHED, memory_order_release);
    }
  }
  return (_THRET_T_)0;
}

void merry_hord_destroy() {
  for (msize_t i = 0; i < 5; i++) {
    MerryHord *h = &hords[i];
    atomic_store_explicit((_Atomic mbool_t *)&h->hord_terminate, mtrue,
                          memory_order_release);
    h->total_work_queued = 0;
    if (h->hord_suspended)
      merry_cond_signal(&h->hord_cond);
    merry_cond_destroy(&h->hord_cond);
    merry_destroy_dynamic_list(h->all_queues);
  }
  merry_mutex_destroy(&shared_lock);
}
