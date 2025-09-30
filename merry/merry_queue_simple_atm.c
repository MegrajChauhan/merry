#include <merry_queue_simple_atm.h>

MerryQueueSimpleAtm *merry_create_simple_queue(msize_t cap, msize_t elen,
                                               MerryErrorStack *st) {
  if (surelyF(cap == 0 || elen == 0))
    return RET_NULL;
  MerryQueueSimpleAtm *queue =
      (MerryQueueSimpleAtm *)malloc(sizeof(MerryQueueSimpleAtm));
  if (!queue) {
    PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
         "Allocating a new simple queue");
    merry_error_stack_fatality(st);
    return RET_NULL;
  }
  queue->buf = malloc(sizeof(mptr_t) * cap * elen);
  if (!queue->buf) {
    PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
         "Allocating a simple queue buffer");
    merry_error_stack_fatality(st);
    return RET_NULL;
  }
  queue->buf_cap = cap;
  queue->elem_len = elen;
  queue->head = queue->rear = (mqword_t)(-1);
  return queue;
}

mptr_t merry_simple_queue_top(MerryQueueSimpleAtm *queue) {
  merry_check_ptr(queue);
  if (merry_simple_queue_atm_empty(queue, queue->rear))
    return RET_NULL;
  return (mptr_t)(((char *)queue->buf + (queue->head * queue->elem_len)));
}

mret_t merry_simple_queue_enqueue(MerryQueueSimpleAtm *queue, mptr_t elem) {
  merry_check_ptr(queue);
  merry_check_ptr(elem);

  msize_t id = atomic_fetch_add(&queue->rear, 1);
  msize_t inx = (id) % queue->buf_cap;

  if (merry_simple_queue_atm_full(queue, inx))
    return RET_FAILURE;

  memcpy((char *)queue->buf + inx * queue->elem_len, elem, queue->elem_len);
  return RET_SUCCESS;
}

mret_t merry_simple_queue_dequeue(MerryQueueSimpleAtm *queue, mptr_t elem) {
  merry_check_ptr(queue);
  merry_check_ptr(elem);

  if (merry_simple_queue_atm_empty(queue, queue->rear))
    return RET_FAILURE;

  msize_t head = atomic_load_explicit(&queue->head, memory_order_relaxed);

  memcpy(elem, (mptr_t)((char *)queue->buf + head * queue->elem_len),
         queue->elem_len);

  head = (head + 1) % queue->buf_cap;
  atomic_store_explicit(&queue->head, head, memory_order_release);

  return RET_SUCCESS;
}

void merry_destroy_simple_queue(MerryQueueSimpleAtm *queue) {
  merry_check_ptr(queue);
  merry_check_ptr(queue->buf);
  free(queue->buf);
  free(queue);
}
