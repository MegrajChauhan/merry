#ifndef _MERRY_STACK_
#define _MERRY_STACK_

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
  Merry##name##Stack *merry_##name##_stack_init(msize_t cap);                  \
  Merry##name##Stack *merry_##name##_stack_init_ext_buf(type *buf,             \
                                                        msize_t len);          \
  mret_t merry_##name##_stack_push(Merry##name##Stack *stack, type *value);    \
  type *merry_##name##_stack_pop(Merry##name##Stack *stack);                   \
  void merry_##name##_stack_clear(Merry##name##Stack *stack);                  \
  void merry_##name##_stack_destroy(Merry##name##Stack *stack);

#define merry_is_stack_full(stack) (((stack)->sp + 1) >= (stack)->sp_max)
#define merry_is_stack_empty(stack) ((stack)->sp == (msize_t)(-1))
#define merry_stack_has_this_many_items(stack, n) (((stack)->sp + 1) >= (n))
#define merry_stack_has_space_for(stack, n)                                    \
  (((stack)->sp - (stack)->sp_max) >= (n))

#define _MERRY_DEFINE_STACK_(name, type)                                       \
  Merry##name##Stack *merry_##name##_stack_init(msize_t cap) {                 \
    Merry##name##Stack *stk =                                                  \
        (Merry##name##Stack *)malloc(sizeof(Merry##name##Stack));              \
    if (!stk) {                                                                \
      MFATAL(NULL, "Failed to allocate memory for stack", NULL);               \
      return RET_NULL;                                                         \
    }                                                                          \
    stk->buf = (type *)malloc(sizeof(type) * cap);                             \
    if (!stk->buf) {                                                           \
      MFATAL(NULL, "Failed to allocate memory for stack buffer", NULL);        \
      free(stk);                                                               \
      return RET_NULL;                                                         \
    }                                                                          \
    stk->sp = (msize_t)(-1);                                                   \
    stk->cap = cap;                                                            \
    stk->sp_max = cap - 1;                                                     \
    stk->external_buffer = mfalse;                                             \
    return stk;                                                                \
  }                                                                            \
  Merry##name##Stack *merry_##name##_stack_init_ext_buf(type *buf,             \
                                                        msize_t len) {         \
    merry_check_ptr(buf);                                                      \
    if (len == 0)                                                              \
      merry_unreachable();                                                     \
    Merry##name##Stack *stk =                                                  \
        (Merry##name##Stack *)malloc(sizeof(Merry##name##Stack));              \
    if (!stk) {                                                                \
      MFATAL(NULL, "Failed to allocate memory for stack", NULL);               \
      return RET_NULL;                                                         \
    }                                                                          \
    stk->buf = buf;                                                            \
    stk->sp = (msize_t)(-1);                                                   \
    stk->cap = len;                                                            \
    stk->sp_max = len - 1;                                                     \
    stk->external_buffer = mtrue;                                              \
    return stk;                                                                \
  }                                                                            \
  mret_t merry_##name##_stack_push(Merry##name##Stack *stack, type *value) {   \
    merry_check_ptr(stack);                                                    \
    merry_check_ptr(stack->buf);                                               \
    merry_check_ptr(value);                                                    \
    if (merry_is_stack_full(stack))                                            \
      return RET_FAILURE;                                                      \
    stack->sp++;                                                               \
    stack->buf[stack->sp] = *value;                                            \
    return RET_SUCCESS;                                                        \
  }                                                                            \
  type *merry_##name##_stack_pop(Merry##name##Stack *stack) {                  \
    merry_check_ptr(stack);                                                    \
    merry_check_ptr(stack->buf);                                               \
    if (merry_is_stack_empty(stack))                                           \
      return RET_NULL;                                                         \
    register type *v = &stack->buf[stack->sp];                                 \
    stack->sp--;                                                               \
    return v;                                                                  \
  }                                                                            \
  void merry_##name##_stack_clear(Merry##name##Stack *stack) {                 \
    merry_check_ptr(stack);                                                    \
    merry_check_ptr(stack->buf);                                               \
    stack->sp = (msize_t) - 1;                                                 \
  }                                                                            \
  void merry_##name##_stack_destroy(Merry##name##Stack *stack) {               \
    merry_check_ptr(stack);                                                    \
    merry_check_ptr(stack->buf);                                               \
    if (!stack->external_buffer)                                               \
      free(stack->buf);                                                        \
    free(stack);                                                               \
  }

#endif
