#ifndef _MERRY_QUEUE_
#define _MERRY_QUEUE_

#include <merry_logger.h>
#include <merry_operations.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

/*
 * We will need:
 * 1. Dynamic Queue using Linked Lists
 * 2. Static Queue
 * 3. Lock free static queue
 * Lock free Dynamic queue will be extremely hard
 * */

/*----------DYNAMIC QUEUE------------*/

#define _MERRY_DECLARE_QUEUE_(name, type)                                      \
  typedef struct MerryLL##name##QueueNode MerryLL##name##QueueNode;            \
  typedef struct MerryLL##name##Queue MerryLL##name##Queue;                    \
  struct MerryLL##name##QueueNode {                                            \
    MerryLL##name##QueueNode *next_node, *prev_node;                           \
    type data;                                                                 \
  };                                                                           \
  struct MerryLL##name##Queue {                                                \
    MerryLL##name##QueueNode *head, *tail;                                     \
  };                                                                           \
  mresult_t merry_##name##_llqueue_init(MerryLL##name##Queue **queue);         \
  mresult_t merry_##name##_llqueue_push(MerryLL##name##Queue *queue,           \
                                        type *data);                           \
  mresult_t merry_##name##_llqueue_pop(MerryLL##name##Queue *queue,            \
                                       type *_store_in);                       \
  void merry_##name##_llqueue_clear(MerryLL##name##Queue *queue);              \
  void merry_##name##_llqueue_destroy(MerryLL##name##Queue *queue);

#define _MERRY_DEFINE_QUEUE_(name, type)                                       \
  mresult_t merry_##name##_llqueue_init(MerryLL##name##Queue **queue) {        \
    *queue = (MerryLL##name##Queue *)malloc(sizeof(MerryLL##name##Queue));     \
    if (!(*queue)) {                                                           \
      return MRES_SYS_FAILURE;                                                 \
    }                                                                          \
    (*queue)->head = (*queue)->tail = NULL;                                    \
    return MRES_SUCCESS;                                                       \
  }                                                                            \
  mresult_t merry_##name##_llqueue_push(MerryLL##name##Queue *queue,           \
                                        type *data) {                          \
    merry_check_ptr(queue);                                                    \
    merry_check_ptr(data);                                                     \
    MerryLL##name##QueueNode *node =                                           \
        (MerryLL##name##QueueNode *)malloc(sizeof(MerryLL##name##QueueNode));  \
    if (!node) {                                                               \
      return MRES_SYS_FAILURE;                                                 \
    }                                                                          \
    node->data = *data;                                                        \
    if (queue->head == NULL && queue->tail == NULL) {                          \
      node->next_node = NULL;                                                  \
      node->prev_node = NULL;                                                  \
      queue->head = queue->tail = node;                                        \
    } else {                                                                   \
      node->next_node = NULL;                                                  \
      node->prev_node = queue->tail;                                           \
      queue->tail->next_node = node;                                           \
      queue->tail = node;                                                      \
    }                                                                          \
    return MRES_SUCCESS;                                                       \
  }                                                                            \
  mresult_t merry_##name##_llqueue_pop(MerryLL##name##Queue *queue,            \
                                       type *_store_in) {                      \
    merry_check_ptr(queue);                                                    \
    merry_check_ptr(_store_in);                                                \
    if (queue->head == NULL && queue->tail == NULL) {                          \
      return MRES_CONT_EMPTY;                                                  \
    }                                                                          \
    MerryLL##name##QueueNode *head = queue->head;                              \
    *_store_in = head->data;                                                   \
    if (queue->head == queue->tail) {                                          \
      queue->tail = NULL;                                                      \
      queue->head = NULL;                                                      \
    } else {                                                                   \
      head->next_node->prev_node = NULL;                                       \
      queue->head = head->next_node;                                           \
    }                                                                          \
    free(head);                                                                \
    return RET_SUCCESS;                                                        \
  }                                                                            \
  void merry_##name##_llqueue_clear(MerryLL##name##Queue *queue) {             \
    merry_check_ptr(queue);                                                    \
    merry_assert(!(queue->head && !queue->tail));                              \
    merry_assert(!(!queue->head && queue->tail));                              \
    MerryLL##name##QueueNode *curr = queue->head;                              \
    while (curr != NULL) {                                                     \
      MerryLL##name##QueueNode *tmp = curr->next_node;                         \
      free(curr);                                                              \
      curr = tmp;                                                              \
    }                                                                          \
    queue->head = NULL;                                                        \
    queue->tail = NULL;                                                        \
  }                                                                            \
  void merry_##name##_llqueue_destroy(MerryLL##name##Queue *queue) {           \
    merry_##name##_llqueue_clear(queue);                                       \
    free(queue);                                                               \
  }

/*----------END DYNAMIC QUEUE------------*/

/*----------STATIC QUEUE------------*/
typedef struct MerrySQueue MerrySQueue;

struct MerrySQueue {
  msize_t head, rear;
  mptr_t *buf;
  msize_t buf_cap;
  msize_t elem_len;
};

#define merry_squeue_empty(queue) ((queue)->head == (mqword_t)(-1))
#define merry_squeue_full(queue)                                               \
  ((((queue)->rear + 1) % (queue)->buf_cap) == (queue)->head)
#define merry_squeue_clear(queue)                                              \
  ((queue)->head = (queue)->rear = (mqword_t)(-1))

mresult_t merry_create_squeue(MerrySQueue **queue, msize_t cap, msize_t elen);

mptr_t merry_squeue_top(MerrySQueue *queue);

mresult_t merry_squeue_enqueue(MerrySQueue *queue, mptr_t elem);

mresult_t merry_squeue_dequeue(MerrySQueue *queue, mptr_t elem);

void merry_destroy_squeue(MerrySQueue *queue);

/*----------END STATIC QUEUE------------*/

/*----------LF STATIC QUEUE------------*/

typedef struct MerrySQueueAtm MerrySQueueAtm;

struct MerrySQueueAtm {
  _Atomic msize_t head, rear;
  mptr_t *buf;
  msize_t buf_cap;
  msize_t elem_len;
};

#define merry_squeue_atm_empty(queue, idx) ((queue)->head == (idx))
#define merry_squeue_atm_full(queue, idx)                                      \
  (((idx + 1) % (queue)->buf_cap) == (queue)->head)
#define merry_squeue_atm_clear(queue)                                          \
  ((queue)->head = (queue)->rear = (mqword_t)(-1))

mresult_t merry_create_squeue_atm(MerrySQueueAtm **queue, msize_t cap,
                                  msize_t elen);

mptr_t merry_squeue_atm_top(MerrySQueueAtm *queue);

mresult_t merry_squeue_atm_enqueue(MerrySQueueAtm *queue, mptr_t elem);

mresult_t merry_squeue_atm_dequeue(MerrySQueueAtm *queue, mptr_t elem);

void merry_destroy_squeue_atm(MerrySQueueAtm *queue);

/*----------END LF STATIC QUEUE------------*/

#endif
