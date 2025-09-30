#ifndef _MERRY_QUEUE_SIMPLE_ATM_
#define _MERRY_QUEUE_SIMPLE_ATM_

#include "merry_error_stack.h"
#include "merry_types.h"
#include "merry_utils.h"
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

// Simple queue is going to be Circular queue
typedef struct MerryQueueSimpleAtm MerryQueueSimpleAtm;

struct MerryQueueSimpleAtm {
  _Atomic msize_t head, rear;
  mptr_t *buf;
  msize_t buf_cap;
  msize_t elem_len;
};

#define merry_simple_queue_atm_empty(queue, idx) ((queue)->head == (idx))
#define merry_simple_queue_atm_full(queue, idx)                                \
  (((idx + 1) % (queue)->buf_cap) == (queue)->head)
#define merry_simple_queue_atm_clear(queue)                                    \
  ((queue)->head = (queue)->rear = (mqword_t)(-1))

MerryQueueSimpleAtm *merry_create_simple_queue(msize_t cap, msize_t elen,
                                               MerryErrorStack *st);

mptr_t merry_simple_queue_top(MerryQueueSimpleAtm *queue);

mret_t merry_simple_queue_enqueue(MerryQueueSimpleAtm *queue, mptr_t elem);

mret_t merry_simple_queue_dequeue(MerryQueueSimpleAtm *queue, mptr_t elem);

void merry_destroy_simple_queue(MerryQueueSimpleAtm *queue);

#endif
