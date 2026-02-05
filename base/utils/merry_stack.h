#ifndef _MERRY_STACK_
#define _MERRY_STACK_

#include <merry_results.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdlib.h>

// The stacks are going to be static only
// The stack also stores only pointers(or type-casted integers)
#define _MERRY_DECLARE_STACK_(name, type)                                      \
  typedef struct Merry##name##Stack Merry##name##Stack;                        \
  struct Merry##name##Stack {                                                  \
    type *buf;                                                                 \
    msize_t sp;                                                                \
    msize_t cap;                                                               \
    msize_t sp_max;                                                            \
    mbool_t external_buffer;                                                   \
  };                                                                           \
  mresult_t merry_##name##_stack_init(Merry##name##Stack **stack,              \
                                      msize_t cap);                            \
  mresult_t merry_##name##_stack_init_ext_buf(Merry##name##Stack **stack,      \
                                              type *buf, msize_t len);         \
  mresult_t merry_##name##_stack_push(Merry##name##Stack *stack, type *value); \
  mresult_t merry_##name##_stack_pop(Merry##name##Stack *stack, type *elem);   \
  mresult_t merry_##name##_stack_top(Merry##name##Stack *stack, type *elem);   \
  void merry_##name##_stack_clear(Merry##name##Stack *stack);                  \
  void merry_##name##_stack_destroy(Merry##name##Stack *stack);

#define merry_is_stack_full(stack) (((stack)->sp + 1) >= (stack)->sp_max)
#define merry_is_stack_empty(stack) ((stack)->sp == (msize_t)(-1))
#define merry_stack_has_this_many_items(stack, n) (((stack)->sp + 1) >= (n))
#define merry_stack_has_space_for(stack, n)                                    \
  (((stack)->sp - (stack)->sp_max) >= (n))

#define _MERRY_DEFINE_STACK_(name, type)                                       \
  mresult_t merry_##name##_stack_init(Merry##name##Stack **stack,              \
                                      msize_t cap) {                           \
    if (!stack)                                                                \
      return MRES_INVALID_ARGS;                                                \
    *stack = (Merry##name##Stack *)malloc(sizeof(Merry##name##Stack));         \
    if (!(*stack)) {                                                           \
      return MRES_SYS_FAILURE;                                                 \
    }                                                                          \
    (*stack)->buf = (type *)malloc(sizeof(type) * cap);                        \
    if (!(*stack)->buf) {                                                      \
      free(*stack);                                                            \
      return MRES_SYS_FAILURE;                                                 \
    }                                                                          \
    (*stack)->sp = (msize_t)(-1);                                              \
    (*stack)->cap = cap;                                                       \
    (*stack)->sp_max = cap - 1;                                                \
    (*stack)->external_buffer = mfalse;                                        \
    return MRES_SUCCESS;                                                       \
  }                                                                            \
  mresult_t merry_##name##_stack_init_ext_buf(Merry##name##Stack **stack,      \
                                              type *buf, msize_t len) {        \
    if (!stack || len == 0)                                                    \
      return MRES_INVALID_ARGS;                                                \
    *stack = (Merry##name##Stack *)malloc(sizeof(Merry##name##Stack));         \
    if (!(*stack)) {                                                           \
      return MRES_SYS_FAILURE;                                                 \
    }                                                                          \
    (*stack)->buf = buf;                                                       \
    (*stack)->sp = (msize_t)(-1);                                              \
    (*stack)->cap = len;                                                       \
    (*stack)->sp_max = len - 1;                                                \
    (*stack)->external_buffer = mtrue;                                         \
    return MRES_SUCCESS;                                                       \
  }                                                                            \
  mresult_t merry_##name##_stack_push(Merry##name##Stack *stack,               \
                                      type *value) {                           \
    if (!stack || !value)                                                      \
      return MRES_INVALID_ARGS;                                                \
    if (merry_is_stack_full(stack))                                            \
      return MRES_CONT_FULL;                                                   \
    stack->sp++;                                                               \
    stack->buf[stack->sp] = *value;                                            \
    return MRES_SUCCESS;                                                       \
  }                                                                            \
  mresult_t merry_##name##_stack_pop(Merry##name##Stack *stack, type *elem) {  \
    if (!stack || !elem)                                                       \
      return MRES_INVALID_ARGS;                                                \
    if (merry_is_stack_empty(stack))                                           \
      return MRES_CONT_EMPTY;                                                  \
    *elem = stack->buf[stack->sp];                                             \
    stack->sp--;                                                               \
    return MRES_SUCCESS;                                                       \
  }                                                                            \
  mresult_t merry_##name##_stack_top(Merry##name##Stack *stack, type *elem) {  \
    if (!stack || !elem)                                                       \
      return MRES_INVALID_ARGS;                                                \
    if (merry_is_stack_empty(stack))                                           \
      return MRES_CONT_EMPTY;                                                  \
    *elem = stack->buf[stack->sp];                                             \
    return MRES_SUCCESS;                                                       \
  }                                                                            \
  void merry_##name##_stack_clear(Merry##name##Stack *stack) {                 \
    if (!stack)                                                                \
      return;                                                                  \
    stack->sp = (msize_t) - 1;                                                 \
  }                                                                            \
  void merry_##name##_stack_destroy(Merry##name##Stack *stack) {               \
    if (!stack)                                                                \
      return;                                                                  \
    if (stack->external_buffer)                                                \
      free(stack->buf);                                                        \
    free(stack);                                                               \
  }

#endif
