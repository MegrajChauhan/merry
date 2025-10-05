#include <merry_hord.h>

_MERRY_INTERNAL_ void merry_hord_task_resubmit(MerryHordTask *task) {
  // It won't be NULL
  if (atomic_load_explicit((_Atomic mbool_t *)&failed, memory_order_relaxed))
    return;
  if (merry_hord_enqueue_task(task) == RET_FAILURE) {
    // This is a serious error right here.
    // Hords cannot crowd Graves by sending a request about the error
    // So what can hord do? For the time being, we register the error and submit
    atomic_store_explicit((_Atomic mbool_t *)&failed, mtrue,
                          memory_order_release);
  }
}

_MERRY_INTERNAL_ mret_t merry_hord_steal_task(msize_t theif_id) {
  if (atomic_load_explicit((_Atomic mbool_t *)&failed, memory_order_relaxed))
    return RET_FAILURE;
  MerryHordTask task;
  for (msize_t i = 0; i < 5; i++) {
    if (i == theif_id)
      continue;
    if (atomic_load_explicit((_Atomic msize_t *)&hords[i].total_work_queued,
                             memory_order_relaxed) > 4) {
      if (merry_simple_queue_atm_dequeue(hords[i].task_queue, (mptr_t)&task) ==
          RET_SUCCESS) {
        atomic_fetch_sub((_Atomic msize_t *)&hords[i].total_work_queued, 1);
        merry_simple_queue_atm_enqueue(hords[theif_id].task_queue,
                                       (mptr_t)&task);
        hords[theif_id].total_work_queued++;
      }
    }
  }
  return RET_SUCCESS;
}

_MERRY_INTERNAL_ void merry_hord_flush_overflow() {
  merry_mutex_lock(&shared_lock);
  if (atomic_load_explicit((_Atomic mbool_t *)&failed, memory_order_relaxed))
    return;
  for (msize_t i = 0; i < overflow->data_count; i++) {
    msize_t j = i % 5;
    msize_t k = j;
    MerryHordTask *task;
    merry_dynamic_queue_pop(overflow, (mptr_t *)&task);
    do {
      if (merry_simple_queue_atm_enqueue(hords[j].task_queue, (mptr_t)task) ==
          RET_SUCCESS)
        break;
      j = (j + 1) % 5;
    } while (j != k);
    if (j == k) {
      // All hords have a full queue so we can't flush yet
      if (merry_dynamic_queue_push(overflow, task, &hords_st) == RET_FAILURE) {
        atomic_store_explicit((_Atomic mbool_t *)&failed, mtrue,
                              memory_order_release);
        PUSH(&hords_st, "Overflow buffer overflowed",
             "Hords couldn't flush the stocked-up tasks", "Hord Flush");
        return; // Failure
      }

      break;
    }
  }
  merry_mutex_unlock(&shared_lock);
}

mret_t merry_hord_init(MerryErrorStack *st) {
  // Initialize each worker and then start them
  __internal_bookkeep = 0;
  if (merry_mutex_init(&shared_lock) == RET_FAILURE) {
    PUSH(st, "Failed to Obtain Lock", "HORD couldn't initialize",
         "HORD Initialization");
    return RET_FAILURE;
  }
  if ((overflow = merry_dynamic_queue_init(st)) == RET_NULL) {
    PUSH(st, NULL, "HORD couldn't initialize", "HORD Initialization");
    return RET_FAILURE;
  }
  for (msize_t i = 0; i < 5; i++) {
    MerryHord *h = &hords[i];
    if (merry_cond_init(&h->hord_cond) == RET_FAILURE) {
      PUSH(st, "Failed to Obtain Condition Variable",
           "HORDs couldn't initialize", "HORDs Initialization");
      return RET_FAILURE;
    }
    if ((h->task_queue = merry_create_simple_queue_atm(
             128, sizeof(MerryHordTask), st)) == RET_NULL) {
      PUSH(st, NULL, "HORDs couldn't initialize", "HORDs Initialization");
      return RET_FAILURE;
    }
    h->total_work_queued = 0;
    h->hord_suspended = mtrue;
    h->hord_terminate = mfalse;
    mthread_t th;
    if (merry_create_detached_thread(&th, merry_hord_run, h, st) ==
        RET_FAILURE) {
      PUSH(st, "Failed to Start Hord", "HORD couldn't initialize",
           "HORD Initialization");
      return RET_FAILURE;
    }
    __internal_bookkeep++;
  }
  __internal_bookkeep++;
  merry_error_stack_init(&hords_st, -1, -1, -1);
  return RET_SUCCESS;
}

_THRET_T_ merry_hord_run(mptr_t hord) {
  MerryHord *h = (MerryHord *)hord;
  MerryQueueSimpleAtm *my_tasks = h->task_queue;
  while (mtrue) {
    if (atomic_load_explicit((_Atomic mbool_t *)&h->hord_terminate,
                             memory_order_relaxed))
      break;
    if (atomic_load_explicit((_Atomic mbool_t *)&failed,
                             memory_order_relaxed)) {
      h->hord_suspended = mtrue;
      merry_cond_wait(&h->hord_cond, &shared_lock);
    }
    if (h->total_work_queued == 0) {
      merry_hord_flush_overflow(); // try to flush it
      if (merry_simple_queue_atm_empty(my_tasks, my_tasks->rear)) {
        // Try to steal other hord's job
        if (merry_hord_steal_task(h->st_ind) == RET_FAILURE)
          continue; // failed set so sleep
      }
    }
    MerryHordTask task;
    merry_simple_queue_atm_dequeue(my_tasks, (mptr_t)&task);
    atomic_store_explicit((_Atomic mhtaskstate_t *)&task.state, TASK_WORKING,
                          memory_order_release);
    msqword_t res = task.task_func(task.task_param, &task.task_res);
    switch (res) {
    case -1: // The task has suspended itself
    {
      atomic_store_explicit((_Atomic mhtaskstate_t *)&task.state,
                            TASK_SUSPENDED, memory_order_release);
      merry_hord_task_resubmit((mptr_t)&task); // Save for later
      break;
    }
    case 0:
      atomic_store_explicit((_Atomic mhtaskstate_t *)&task.state, TASK_FINISHED,
                            memory_order_release);
      atomic_fetch_sub_explicit((_Atomic msize_t *)&h->total_work_queued, 1,
                                memory_order_relaxed);
    }
  }
  atomic_fetch_sub((_Atomic msize_t *)&__internal_bookkeep, 1);
  return (_THRET_T_)0;
}

mret_t merry_hord_enqueue_task(MerryHordTask *task) {
  merry_check_ptr(task);

  if (atomic_load_explicit((_Atomic mbool_t *)&failed, memory_order_relaxed))
    return RET_FAILURE;
  msize_t tries = 0;
  while (1) {
    if (atomic_load_explicit((_Atomic mbool_t *)&failed, memory_order_relaxed))
      break;
    msize_t q = atomic_fetch_add((_Atomic msize_t *)&curr_queue, 1);
    if (tries == 5) {
      // Every hord is full
      merry_mutex_lock(&shared_lock);
      mret_t r = merry_dynamic_queue_push(overflow, (mptr_t)task, &hords_st);
      merry_mutex_unlock(&shared_lock);
      return r;
    }
    msize_t idx = q % 5;
    if (merry_simple_queue_atm_enqueue(hords[idx].task_queue, (mptr_t)task) ==
        RET_FAILURE) {
      tries++;
    } else {
      atomic_fetch_add_explicit(
          (_Atomic msize_t *)&hords[idx].total_work_queued, 1,
          memory_order_relaxed);
      merry_cond_signal(&hords[idx].hord_cond);
    }
  }
  return RET_SUCCESS;
}

_MERRY_ALWAYS_INLINE_ mbool_t merry_hords_status() {
  return atomic_load_explicit((_Atomic mbool_t *)&failed, memory_order_relaxed);
}

_MERRY_ALWAYS_INLINE_ MerryErrorStack *merry_hord_error_status() {
  return &hords_st;
}

void merry_hord_destroy() {
  for (msize_t i = 0; i < __internal_bookkeep; i++) {
    MerryHord *h = &hords[i];
    atomic_store_explicit((_Atomic mbool_t *)&h->hord_terminate, mtrue,
                          memory_order_release);
    h->total_work_queued = 0;
    if (h->hord_suspended)
      merry_cond_signal(&h->hord_cond);
  }
  while (atomic_load((_Atomic msize_t *)&__internal_bookkeep) != 0)
    usleep(5);
  for (msize_t i = 0; i < __internal_bookkeep; i++) {
    MerryHord *h = &hords[i];
    merry_cond_destroy(&h->hord_cond);
    merry_destroy_simple_queue_atm(h->task_queue);
  }
  merry_mutex_destroy(&shared_lock);
  merry_dynamic_queue_destroy(overflow);
}
