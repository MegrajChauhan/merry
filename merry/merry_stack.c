#include <merry_stack.h>

MerryStack *merry_stack_init(msize_t cap, MerryErrorStack *st) {
  merry_check_ptr(st);
  MerryStack *stk = (MerryStack *)malloc(sizeof(MerryStack));
  if (!stk) {
    PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
         "Allocating a new stack");
    merry_error_stack_fatality(st);
    return RET_NULL;
  }
  stk->buf = (mptr_t *)malloc(sizeof(mptr_t) * cap);
  if (!stk->buf) {
    PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
         "Allocating stack buffer");
    merry_error_stack_fatality(st);
    free(stk);
    return RET_NULL;
  }

  stk->sp = (msize_t)(-1);
  stk->cap = cap;
  stk->sp_max = cap - 1;
  return stk;
}

mret_t merry_stack_push(MerryStack *stack, mptr_t value) {
  merry_check_ptr(stack);
  merry_check_ptr(stack->buf);
  merry_check_ptr(value);

  if (merry_is_stack_full(stack))
    return RET_FAILURE;

  stack->sp++;
  stack->buf[stack->sp] = value;
  return RET_SUCCESS;
}

mptr_t merry_stack_pop(MerryStack *stack) {
  merry_check_ptr(stack);
  merry_check_ptr(stack->buf);

  if (merry_is_stack_empty(stack))
    return RET_NULL;

  register mptr_t v = stack->buf[stack->sp];
  stack->sp--;
  return v;
}

void merry_stack_clear(MerryStack *stack) {
  merry_check_ptr(stack);
  merry_check_ptr(stack->buf);

  stack->sp = (msize_t)-1;
}

void merry_stack_destroy(MerryStack *stack) {
  merry_check_ptr(stack);
  merry_check_ptr(stack->buf);
  free(stack->buf);
  free(stack);
}
