#ifndef _MERRY_HORD_
#define _MERRY_HORD_

#include <merry_config.h>
#include <merry_error_stack.h>
#include <merry_hord_task.h>
#include <merry_protectors.h>
#include <merry_queue.h>
#include <merry_queue_simple_atm.h>
#include <merry_threads.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdatomic.h>
#include <stdlib.h>

typedef struct MerryHord MerryHord;

// One Hord represents one worker
struct MerryHord {
  MerryQueueSimpleAtm *task_queue;
  msize_t total_work_queued;
  mbool_t hord_suspended;
  mbool_t hord_terminate;
  msize_t st_ind;
  mcond_t hord_cond;
};

/*
 * In order to bypass the slow Request handler and Graves request handling,
 * cores will have direct access to all Hords via the API provided here. Hords
 * will provide the needed asynchronous functionalities for the cores to
 * register certain tasks
 * */

_MERRY_INTERNAL_ MerryHord hords[5];   // 5 workers(Hords) for now
_MERRY_INTERNAL_ mmutex_t shared_lock; // all workers will share this
_MERRY_INTERNAL_ MerryDynamicQueue *overflow;
_MERRY_INTERNAL_ msize_t curr_queue = 0;
_MERRY_INTERNAL_ mbool_t failed = mfalse;
_MERRY_INTERNAL_ MerryErrorStack hords_st;
_MERRY_INTERNAL_ msize_t __internal_bookkeep = 5;

mret_t merry_hord_init(MerryErrorStack *st);

_THRET_T_ merry_hord_run(mptr_t hord);

// void merry_hord_get_status(msize_t res[5]);

mret_t merry_hord_enqueue_task(MerryHordTask *task);

mbool_t merry_hords_status();

MerryErrorStack *merry_hord_error_status();

void merry_hord_destroy();

#endif
