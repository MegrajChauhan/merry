#ifndef _MERRY_LIST_
#define _MERRY_LIST_

/*
 * We are gonna have:
 * 1. Static list
 * 2. lock free static list here
 * Dynamic lists are not implemented for more control. The ones using these
 * lists will have more control over their operations
 * */

#include <merry_logger.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h> // memcpy

/*------------------STATIC LIST---------------------*/
#define _MERRY_LIST_METADATA_LEN_ 32
#define _MERRY_LIST_METADATA_LEN_EXCLUDE_BUF_ 24
#define _MERRY_LIST_GET_CAP_ 8
#define _MERRY_LIST_GET_IND_ 16
#define _MERRY_LIST_GET_ELEN_ 24

/*
 * elem_len -> 8-byte size_t
 * ind -> 8-bytes, current index
 * cap -> 8-byte size_t
 * buf -> 8-byte pointer
 * */
/*
#define merry_list_create(type, cap) (type *)_list_create(sizeof(type), cap)
#define merry_list_destroy(lst) _list_destroy(lst)
#define merry_list_push(lst, elem) _list_push(lst, elem)
#define merry_list_pop(lst) _list_pop(lst)
#define merry_list_at(lst, ind) _list_at(lst, ind)
#define merry_list_resize(lst, resize_factor) _list_resize(lst, resize_factor)
#define merry_list_size(lst) _list_size(lst)
#define merry_list_index_of(lst, elem) _list_index_of(lst, elem)

mptr_t _list_create(msize_t elen, msize_t cap);
void _list_destroy(mptr_t lst);
mret_t _list_push(mptr_t lst, mptr_t elem);
mptr_t _list_pop(mptr_t lst);
mptr_t _list_at(mptr_t lst, msize_t ind);
mptr_t _list_resize(mptr_t lst, msize_t resize_factor);
msize_t _list_size(mptr_t lst);
msize_t _list_index_of(mptr_t lst, mptr_t elem);
*/

#define _MERRY_DECLARE_STATIC_LIST_(name, type)                                \
  typedef struct Merry##name##List Merry##name##List;                          \
  struct Merry##name##List {                                                   \
    msize_t cap;                                                               \
    msize_t curr_ind;                                                          \
    type *buf;                                                                 \
  };                                                                           \
  Merry##name##List *merry_##name##_list_create(msize_t cap);                  \
  void merry_##name##_list_destroy(Merry##name##List *lst);                    \
  mret_t merry_##name##_list_push(Merry##name##List *lst, type *elem);         \
  type *merry_##name##_list_pop(Merry##name##List *lst);                       \
  type *merry_##name##_list_at(Merry##name##List *lst, msize_t ind);           \
  Merry##name##List *merry_##name##_list_resize(Merry##name##List *lst,        \
                                                msize_t resize_factor);        \
  msize_t merry_##name##_list_size(Merry##name##List *lst);                    \
  msize_t merry_##name##_list_index_of(Merry##name##List *lst, type *elem);

#define _MERRY_DEFINE_STATIC_LIST_(name, type)                                 \
  Merry##name##List *merry_##name##_list_create(msize_t cap) {                 \
    if (cap == 0)                                                              \
      return RET_NULL;                                                         \
    Merry##name##List *list =                                                  \
        (Merry##name##List *)malloc(sizeof(Merry##name##List));                \
    if (!list) {                                                               \
      MFATAL(NULL, "Failed to allocate memory for a static list", NULL);       \
      return RET_NULL;                                                         \
    }                                                                          \
    list->buf = (type *)malloc(sizeof(type) * cap);                            \
                                                                               \
    if (!list->buf) {                                                          \
      MFATAL(NULL, "Failed to allocate memory for static list buffer", NULL);  \
      free(list);                                                              \
      return RET_NULL;                                                         \
    }                                                                          \
    list->cap = cap;                                                           \
    list->curr_ind = 0;                                                        \
    return (list);                                                             \
  }                                                                            \
  void merry_##name##_list_destroy(Merry##name##List *lst) {                   \
    merry_check_ptr(lst);                                                      \
    free(lst->buf);                                                            \
    free(lst);                                                                 \
  }                                                                            \
  mret_t merry_##name##_list_push(Merry##name##List *lst, type *elem) {        \
    merry_check_ptr(lst);                                                      \
    merry_check_ptr(elem);                                                     \
    if (lst->curr_ind >= lst->cap)                                             \
      return RET_FAILURE;                                                      \
    lst->buf[lst->curr_ind] = *elem;                                           \
    lst->curr_ind++;                                                           \
    return RET_SUCCESS;                                                        \
  }                                                                            \
  type *merry_##name##_list_pop(Merry##name##List *lst) {                      \
    merry_check_ptr(lst);                                                      \
    if (lst->curr_ind == 0)                                                    \
      return RET_NULL;                                                         \
    lst->curr_ind--;                                                           \
    return &lst->buf[lst->curr_ind];                                           \
  }                                                                            \
  type *merry_##name##_list_at(Merry##name##List *lst, msize_t ind) {          \
    merry_check_ptr(lst);                                                      \
    if (ind >= lst->cap)                                                       \
      return RET_NULL;                                                         \
    return &lst->buf[ind];                                                     \
  }                                                                            \
  Merry##name##List *merry_##name##_list_resize(Merry##name##List *lst,        \
                                                msize_t resize_factor) {       \
    merry_check_ptr(lst);                                                      \
    Merry##name##List *new_lst =                                               \
        merry_##name##_list_create(lst->cap * resize_factor);                  \
    if (!new_lst)                                                              \
      return RET_NULL;                                                         \
    mempcpy(new_lst->buf, lst->buf, lst->curr_ind * sizeof(type));             \
    new_lst->curr_ind = lst->curr_ind;                                         \
    merry_##name##_list_destroy(lst);                                          \
    return new_lst;                                                            \
  }                                                                            \
  _MERRY_ALWAYS_INLINE_ msize_t merry_##name##_list_size(                      \
      Merry##name##List *lst) {                                                \
    merry_check_ptr(lst);                                                      \
    return lst->curr_ind;                                                      \
  }                                                                            \
  _MERRY_ALWAYS_INLINE_ msize_t merry_##name##_list_index_of(                  \
      Merry##name##List *lst, type *elem) {                                    \
    merry_check_ptr(lst);                                                      \
    merry_check_ptr(elem);                                                     \
    return (msize_t)(((mbptr_t)elem - (mbptr_t)lst->buf) / sizeof(type));      \
  }

/*-----------------END STATIC LIST--------------------*/

/*------------------LF STATIC LIST---------------------*/
#define _MERRY_DECLARE_LF_STATIC_LIST_(name, type)                             \
  typedef struct MerryLF##name##List MerryLF##name##List;                      \
  struct MerryLF##name##List {                                                 \
    msize_t cap;                                                               \
    msize_t curr_ind;                                                          \
    type *buf;                                                                 \
  };                                                                           \
  MerryLF##name##List *merry_lf_##name##_list_create(msize_t cap);             \
  void merry_lf_##name##_list_destroy(MerryLF##name##List *lst);               \
  mret_t merry_lf_##name##_list_push(MerryLF##name##List *lst, type *elem);    \
  type *merry_lf_##name##_list_pop(MerryLF##name##List *lst);                  \
  msize_t merry_lf_##name##_list_size(MerryLF##name##List *lst);

#define _MERRY_DEFINE_LF_STATIC_LIST_(name, type)                              \
  MerryLF##name##List *merry_lf_##name##_list_create(msize_t cap) {            \
    if (cap == 0)                                                              \
      return RET_NULL;                                                         \
    MerryLF##name##List *list =                                                \
        (MerryLF##name##List *)malloc(sizeof(MerryLF##name##List));            \
    if (!list) {                                                               \
      MFATAL(NULL, "Failed to allocate memory for a lf static list", NULL);    \
      return RET_NULL;                                                         \
    }                                                                          \
    list->buf = (type *)malloc(sizeof(type) * cap);                            \
                                                                               \
    if (!list->buf) {                                                          \
      MFATAL(NULL, "Failed to allocate memory for lf static list buffer",      \
             NULL);                                                            \
      free(list);                                                              \
      return RET_NULL;                                                         \
    }                                                                          \
    list->cap = cap;                                                           \
    list->curr_ind = 0;                                                        \
    return (list);                                                             \
  }                                                                            \
  void merry_lf_##name##_list_destroy(MerryLF##name##List *lst) {              \
    merry_check_ptr(lst);                                                      \
    free(lst->buf);                                                            \
    free(lst);                                                                 \
  }                                                                            \
  mret_t merry_lf_##name##_list_push(MerryLF##name##List *lst, type *elem) {   \
    merry_check_ptr(lst);                                                      \
    merry_check_ptr(elem);                                                     \
    msize_t ind = atomic_fetch_add_explicit((_Atomic msize_t *)&lst->curr_ind, \
                                            1, memory_order_relaxed);          \
    if (ind >= lst->cap) {                                                     \
      atomic_fetch_sub_explicit((_Atomic msize_t *)&lst->curr_ind, 1,          \
                                memory_order_relaxed);                         \
      return RET_FAILURE;                                                      \
    }                                                                          \
    lst->buf[ind] = *elem;                                                     \
    return RET_SUCCESS;                                                        \
  }                                                                            \
  type *merry_lf_##name##_list_pop(MerryLF##name##List *lst) {                 \
    merry_check_ptr(lst);                                                      \
    if (lst->curr_ind == 0)                                                    \
      return RET_NULL;                                                         \
    msize_t ind = atomic_fetch_sub_explicit((_Atomic msize_t *)curr_ind, 1,    \
                                            memory_order_relaxed);             \
    return &lst->buf[ind];                                                     \
  }                                                                            \
  _MERRY_ALWAYS_INLINE_ msize_t merry_lf_##name##_list_size(                   \
      MerryLF##name##List *lst) {                                              \
    merry_check_ptr(lst);                                                      \
    return lst->curr_ind;                                                      \
  }                                                                            \
  /*-----------------END STATIC LIST--------------------*/

#endif
