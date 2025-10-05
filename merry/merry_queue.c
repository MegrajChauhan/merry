#include "merry_queue.h"

MerrySQueue *merry_create_squeue(msize_t cap, msize_t elen) {
  MerrySQueue *queue = (MerrySQueue *)malloc(sizeof(MerrySQueue));
  if (!queue) {
    MFATAL(NULL, "Failed to allocate memory for SQUEUE", NULL);
    return RET_NULL;
  }
  queue->buf = malloc(sizeof(mptr_t) * cap * elen);
  if (!queue->buf) {
    MFATAL(NULL, "Failed to allocate memory for SQUEUE BUFFER", NULL);
    free(queue);
    return RET_NULL;
  }
  queue->buf_cap = cap;
  queue->elem_len = elen;
  queue->head = queue->rear = (mqword_t)(-1);
  return queue;
}

mptr_t merry_squeue_top(MerrySQueue *queue) {
  merry_check_ptr(queue);
  if (merry_squeue_empty(queue))
    return RET_NULL;
  return (mptr_t)(((char *)queue->buf + (queue->head * queue->elem_len)));
}

mret_t merry_squeue_enqueue(MerrySQueue *queue, mptr_t elem) {
  merry_check_ptr(queue);
  merry_check_ptr(elem);

  if (merry_squeue_full(queue))
    return RET_FAILURE;

  if (merry_squeue_empty(queue))
    queue->head = 0;
  queue->rear = (queue->rear + 1) % queue->buf_cap;
  memcpy((char *)queue->buf + queue->rear * queue->elem_len, elem,
         queue->elem_len);
  return RET_SUCCESS;
}

mret_t merry_squeue_dequeue(MerrySQueue *queue, mptr_t elem) {
  merry_check_ptr(queue);
  merry_check_ptr(elem);

  if (merry_squeue_empty(queue))
    return RET_FAILURE;

  memcpy(elem, (mptr_t)((char *)queue->buf + queue->head * queue->elem_len),
         queue->elem_len);

  if (queue->head == queue->rear)
    queue->head = queue->rear = (mqword_t)(-1);
  else
    queue->head = (queue->head + 1) % queue->buf_cap;

  return RET_SUCCESS;
}

void merry_destroy_squeue(MerrySQueue *queue) {
  merry_check_ptr(queue);
  merry_check_ptr(queue->buf);
  free(queue->buf);
  free(queue);
}

MerrySQueueAtm *merry_create_squeue_atm(msize_t cap, msize_t elen) {
  if (surelyF(cap == 0 || elen == 0))
    return RET_NULL;
  MerrySQueueAtm *queue = (MerrySQueueAtm *)malloc(sizeof(MerrySQueueAtm));
  if (!queue) {
    MFATAL(NULL, "Failed to allocate memory for SQUEUEAtm", NULL);
    return RET_NULL;
  }
  queue->buf = malloc(sizeof(mptr_t) * cap * elen);
  if (!queue->buf) {
    MFATAL(NULL, "Failed to allocate memory for SQUEUEAtm BUFFER", NULL);
    return RET_NULL;
  }
  queue->buf_cap = cap;
  queue->elem_len = elen;
  queue->head = queue->rear = (mqword_t)(0);
  return queue;
}

mptr_t merry_squeue_atm_top(MerrySQueueAtm *queue) {
  merry_check_ptr(queue);
  if (merry_squeue_atm_empty(queue, queue->rear))
    return RET_NULL;
  return (mptr_t)(((char *)queue->buf + (queue->head * queue->elem_len)));
}

mret_t merry_squeue_atm_enqueue(MerrySQueueAtm *queue, mptr_t elem) {
  merry_check_ptr(queue);
  merry_check_ptr(elem);

  msize_t id = atomic_fetch_add(&queue->rear, 1);
  msize_t inx = (id) % queue->buf_cap;

  if (merry_squeue_atm_full(queue, inx))
    return RET_FAILURE;

  memcpy((char *)queue->buf + inx * queue->elem_len, elem, queue->elem_len);
  return RET_SUCCESS;
}

mret_t merry_squeue_atm_dequeue(MerrySQueueAtm *queue, mptr_t elem) {
  merry_check_ptr(queue);
  merry_check_ptr(elem);

  if (merry_squeue_atm_empty(queue, queue->rear))
    return RET_FAILURE;

  msize_t head = atomic_load_explicit(&queue->head, memory_order_relaxed);

  memcpy(elem, (mptr_t)((char *)queue->buf + head * queue->elem_len),
         queue->elem_len);

  head = (head + 1) % queue->buf_cap;
  atomic_store_explicit(&queue->head, head, memory_order_release);

  return RET_SUCCESS;
}

void merry_destroy_squeue_atm(MerrySQueueAtm *queue) {
  merry_check_ptr(queue);
  merry_check_ptr(queue->buf);
  free(queue->buf);
  free(queue);
}
