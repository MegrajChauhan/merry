#include "merry_list.h"

/*--------------------------STATIC QUEUE---------------------------*/

mptr_t _list_create(msize_t elen, msize_t cap) {
  msize_t *list =
      (msize_t *)malloc(elen * cap + _MERRY_LIST_METADATA_LEN_EXCLUDE_BUF_);
  if (!list) {
    MFATAL(NULL, "Failed to allocate memory for a static list", NULL);
    return RET_NULL;
  }
  *(list) = elen;
  list++;
  *list = 0;
  list++;
  *list = cap;
  list++;
  memset(list, 0, elen * cap);
  return (mptr_t)(list);
}

void _list_destroy(mptr_t lst) {
  merry_check_ptr(lst);
  lst -= sizeof(msize_t) * 3;
  free(lst);
}

mret_t _list_push(mptr_t lst, mptr_t elem) {
  merry_check_ptr(lst);
  merry_check_ptr(elem);

  register msize_t *list = (msize_t *)lst;
  register msize_t *elen = (list - 3);
  register msize_t *curr_ind = (list - 2);
  register msize_t *cap = (list - 1);

  if ((*curr_ind + 1) >= *cap)
    return RET_FAILURE;

  register msize_t *c = (msize_t *)((mbptr_t)lst + *elen * *curr_ind);
  memcpy(c, elem, *elen);
  (*curr_ind)++;

  return RET_SUCCESS;
}

mptr_t _list_pop(mptr_t lst) {
  merry_check_ptr(lst);
  register msize_t *list = (msize_t *)lst;
  register msize_t *elen = (list - 3);
  register msize_t *curr_ind = (list - 2);

  if ((*curr_ind) == 0)
    return RET_NULL; // nothing to pop

  (*curr_ind)--;
  register msize_t *c = (msize_t *)((mbptr_t)lst + *elen * *curr_ind);

  return (mptr_t)c;
}

mptr_t _list_at(mptr_t lst, msize_t ind) {
  merry_check_ptr(lst);
  register msize_t *list = (msize_t *)lst;
  register msize_t *elen = (list - 3);
  register msize_t *cap = (list - 1);

  if ((ind) >= *cap)
    return RET_NULL; // incorrect index

  register msize_t *c = (msize_t *)((mbptr_t)lst + *elen * ind);

  return (mptr_t)c;
}

mptr_t _list_resize(mptr_t lst, msize_t resize_factor) {
  merry_check_ptr(lst);
  register msize_t *list = (msize_t *)lst;
  register msize_t *elen = (list - 3);
  register msize_t *cap = (list - 1);
  msize_t *new_lst = (msize_t *)malloc(*elen * *cap * resize_factor +
                                       _MERRY_LIST_METADATA_LEN_EXCLUDE_BUF_);

  if (!new_lst)
    return RET_NULL; // we fail here without errors because this may not be
                     // fatal

  // success
  msize_t old_cap = *cap;
  *cap *= resize_factor;

  mempcpy(new_lst, list - 3,
          *elen * old_cap * resize_factor +
              _MERRY_LIST_METADATA_LEN_EXCLUDE_BUF_);
  free(lst);
  return new_lst + 3;
}

msize_t _list_size(mptr_t lst) {
  merry_check_ptr(lst);
  return (msize_t)((msize_t *)lst - 2);
}

msize_t _list_index_of(mptr_t lst, mptr_t elem) {
  merry_check_ptr(lst);
  merry_check_ptr(elem);
  register msize_t *elen = ((msize_t *)lst - 3);

  return (msize_t)((elem - lst) / *elen);
}

/*--------------------------STATIC QUEUE END---------------------------*/

mret_t _lf_list_push(mptr_t lst, mptr_t elem) {
  merry_check_ptr(lst);
  merry_check_ptr(elem);

  register msize_t *list = (msize_t *)lst;
  register msize_t *elen = (list - 3);
  register msize_t *curr_ind = (list - 2);
  register msize_t *cap = (list - 1);

  register msize_t my_ind = atomic_fetch_add_explicit(
      (_Atomic msize_t *)curr_ind, 1, memory_order_relaxed);

  if (my_ind >= *cap) {
    atomic_fetch_sub_explicit((_Atomic msize_t *)curr_ind, 1,
                              memory_order_relaxed);
    return RET_FAILURE;
  }

  register msize_t *c = (msize_t *)((mbptr_t)lst + *elen * my_ind);
  memcpy(c, elem, *elen);

  return RET_SUCCESS;
}

mptr_t _lf_list_pop(mptr_t lst) {
  merry_check_ptr(lst);
  register msize_t *list = (msize_t *)lst;
  register msize_t *elen = (list - 3);
  register msize_t *curr_ind = (list - 2);
  register msize_t my_ind = atomic_fetch_sub_explicit(
      (_Atomic msize_t *)curr_ind, 1, memory_order_relaxed);
  if ((my_ind) == 0) {
    atomic_fetch_add_explicit((_Atomic msize_t *)curr_ind, 1,
                              memory_order_relaxed);
    return RET_NULL; // nothing to pop
  }
  register msize_t *c = (msize_t *)((mbptr_t)lst + *elen * my_ind);

  return (mptr_t)c;
}
