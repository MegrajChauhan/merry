#include "merry_memory.h"

mret_t merry_initialize_memory_interface(MerryErrorStack *st) {
  if ((pg_list = merry_create_dynamic_list(10, sizeof(mbptr_t), st)) ==
      RET_NULL) {
    PUSH(st, NULL, "Dead Page List failed to initialize",
         "Initializing memory interface");
    return RET_FAILURE;
  }
  return RET_SUCCESS;
}

_MERRY_INTERNAL_ mbptr_t merry_find_a_free_page() {
  if (merry_is_dyn_list_empty(pg_list))
    return RET_NULL;
  return *(mbptr_t *)merry_dynamic_list_pop(pg_list);
}

MerryNormalMemoryPage *merry_create_normal_memory_page(MerryErrorStack *st) {
  MerryNormalMemoryPage *pg =
      (MerryNormalMemoryPage *)malloc(sizeof(MerryNormalMemoryPage));
  if (!pg) {
    PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
         "Allocating Normal Memory Page");
    merry_error_stack_fatality(st);
    return RET_NULL;
  }

  pg->buf = NULL;
  pg->init = mfalse;

  return pg;
}

mret_t merry_initialize_normal_memory_page(MerryNormalMemoryPage *pg,
                                           MerryErrorStack *st) {
  merry_check_ptr(pg);

  // reinitialization won't work
  merry_assert(pg->buf == NULL);

  if ((pg->buf = merry_find_a_free_page()) == RET_NULL) {
    if ((pg->buf = merry_get_memory_page(_MERRY_PAGE_LEN_, _MERRY_PROT_DEFAULT_,
                                         _MERRY_FLAG_DEFAULT_)) ==
        _MERRY_FAILED_TO_GET_PAGE_) {
      PUSH(st, "Page Allocation Failure", "Failed to allocate memory page",
           "Allocating a new memory page");
      // This is not fatal here because it depends on the context of who the
      // callee is
      merry_error_stack_errno(st);
      return RET_FAILURE;
    }
  }

  pg->init = mtrue;
  return RET_SUCCESS;
}

MerryNormalMemoryPage *
merry_obtain_initialized_normal_memory_page(MerryErrorStack *st) {
  MerryNormalMemoryPage *pg = merry_create_normal_memory_page(st);
  if (!pg)
    return RET_NULL;
  if (merry_initialize_normal_memory_page(pg, st) == RET_FAILURE) {
    free(pg);
    return RET_NULL;
  }

  return pg;
}

void merry_return_normal_memory_page(MerryNormalMemoryPage *pg,
                                     MerryErrorStack *st) {
  merry_check_ptr(pg);
  if (surelyT(pg->buf)) {
    if (merry_dynamic_list_push(pg_list, pg->buf, st) == RET_FAILURE)
      merry_return_memory_page(
          pg->buf,
          _MERRY_PAGE_LEN_); // if we fail to save it then we fail to save it
  }
  free(pg);
}

void merry_destroy_memory_interface() {
  merry_check_ptr(pg_list);
  while (!merry_is_dyn_list_empty(pg_list)) {
    mbptr_t pg = *(mbptr_t *)merry_dynamic_list_pop(pg_list);
    if (surelyT(pg)) {
      merry_return_memory_page(pg, _MERRY_PAGE_LEN_);
    }
  }
}
