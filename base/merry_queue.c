#include "merry_queue.h"

mresult_t merry_create_squeue(MerrySQueue **queue, msize_t cap, msize_t elen) {
  if (surelyF(!queue || cap == 0 || elen == 0))
    return MRES_INVALID_ARGS;
  *queue = (MerrySQueue *)malloc(sizeof(MerrySQueue));
  if (!(*queue)) {
    return MRES_SYS_FAILURE;
  }
  (*queue)->buf = malloc(sizeof(mptr_t) * cap * elen);
  if (!(*queue)->buf) {
    free(*queue);
    return MRES_SYS_FAILURE;
  }
  (*queue)->buf_cap = cap;
  (*queue)->elem_len = elen;
  (*queue)->head = (*queue)->rear = (mqword_t)(-1);
  return MRES_SUCCESS;
}

mptr_t merry_squeue_top(MerrySQueue *queue) {
  if (!queue) return RET_NULL;
  if (merry_squeue_empty(queue))
    return RET_NULL;
  return (mptr_t)(((char *)queue->buf + (queue->head * queue->elem_len)));
}

mresult_t merry_squeue_enqueue(MerrySQueue *queue, mptr_t elem) {
  if (!queue || !elem) return MRES_INVALID_ARGS;

  if (merry_squeue_full(queue))
    return MRES_CONT_FULL;

  if (merry_squeue_empty(queue))
    queue->head = 0;
  queue->rear = (queue->rear + 1) % queue->buf_cap;
  memcpy((char *)queue->buf + queue->rear * queue->elem_len, elem,
         queue->elem_len);
  return MRES_SUCCESS;
}

mresult_t merry_squeue_dequeue(MerrySQueue *queue, mptr_t elem) {
  if (!queue || !elem) return MRES_INVALID_ARGS;

  if (merry_squeue_empty(queue))
    return MRES_CONT_EMPTY;

  memcpy(elem, (mptr_t)((char *)queue->buf + queue->head * queue->elem_len),
         queue->elem_len);

  if (queue->head == queue->rear)
    queue->head = queue->rear = (mqword_t)(-1);
  else
    queue->head = (queue->head + 1) % queue->buf_cap;

  return MRES_SUCCESS;
}

void merry_destroy_squeue(MerrySQueue *queue) {
  if (!queue) return;
  free(queue->buf);
  free(queue);
}

mresult_t merry_create_squeue_atm(MerrySQueueAtm **queue, msize_t cap,
                                  msize_t elen) {
  if (surelyF(!queue || cap == 0 || elen == 0))
    return MRES_INVALID_ARGS;
  *queue = (MerrySQueueAtm *)malloc(sizeof(MerrySQueueAtm));
  if (!(*queue)) {
    return MRES_SYS_FAILURE;
  }
  (*queue)->buf = malloc(sizeof(mptr_t) * cap * elen);
  if (!(*queue)->buf) {
    free(*queue);
    return MRES_SYS_FAILURE;
  }
  (*queue)->buf_cap = cap;
  (*queue)->elem_len = elen;
  (*queue)->head = (*queue)->rear = (mqword_t)(0);
  return MRES_SUCCESS;
}

mptr_t merry_squeue_atm_top(MerrySQueueAtm *queue) {
  if (!queue) return RET_NULL;
  if (merry_squeue_atm_empty(queue, queue->rear))
    return RET_NULL;
  return (mptr_t)(((char *)queue->buf + (queue->head * queue->elem_len)));
}

mresult_t merry_squeue_atm_enqueue(MerrySQueueAtm *queue, mptr_t elem) {
  if (!queue || !elem) return MRES_INVALID_ARGS;

  msize_t id = atomic_fetch_add(&queue->rear, 1);
  msize_t inx = (id) % queue->buf_cap;

  if (merry_squeue_atm_full(queue, inx))
    return MRES_CONT_FULL;

  memcpy((char *)queue->buf + inx * queue->elem_len, elem, queue->elem_len);
  return MRES_SUCCESS;
}

mresult_t merry_squeue_atm_dequeue(MerrySQueueAtm *queue, mptr_t elem) {
  if (!queue || !elem) return MRES_INVALID_ARGS;

  if (merry_squeue_atm_empty(queue, queue->rear))
    return MRES_CONT_EMPTY;

  msize_t head = atomic_load_explicit(&queue->head, memory_order_relaxed);

  memcpy(elem, (mptr_t)((char *)queue->buf + head * queue->elem_len),
         queue->elem_len);

  head = (head + 1) % queue->buf_cap;
  atomic_store_explicit(&queue->head, head, memory_order_release);

  return MRES_SUCCESS;
}

void merry_destroy_squeue_atm(MerrySQueueAtm *queue) {
  if (!queue) return;
  free(queue->buf);
  free(queue);
}
