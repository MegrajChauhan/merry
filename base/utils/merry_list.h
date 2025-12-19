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
#include <merry_results.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h> // memcpy

/*------------------STATIC LIST---------------------*/
#define _MERRY_DECLARE_STATIC_LIST_(name, type)                                \
  typedef struct Merry##name##List Merry##name##List;                          \
  struct Merry##name##List {                                                   \
    msize_t cap;                                                               \
    msize_t curr_ind;                                                          \
    type *buf;                                                                 \
  };                                                                           \
  mresult_t merry_##name##_list_create(msize_t cap, Merry##name##List **lst);  \
  void merry_##name##_list_destroy(Merry##name##List *lst);                    \
  mresult_t merry_##name##_list_push(Merry##name##List *lst, type *elem);      \
  mresult_t merry_##name##_list_pop(Merry##name##List *lst, type *elem);       \
  mresult_t merry_##name##_list_at(Merry##name##List *lst, type *elem,         \
                                   msize_t ind);                               \
  mresult_t merry_##name##_list_resize(Merry##name##List *lst,                 \
                                       msize_t resize_factor);                 \
  msize_t merry_##name##_list_size(Merry##name##List *lst);                    \
  mresult_t merry_##name##_list_ref_of(Merry##name##List *lst, type **elem,    \
                                       msize_t ind);                           \
  msize_t merry_##name##_list_index_of(Merry##name##List *lst, type *elem);

#define _MERRY_DEFINE_STATIC_LIST_(name, type)                                 \
  mresult_t merry_##name##_list_create(msize_t cap, Merry##name##List **lst) { \
    if (cap == 0)                                                              \
      return MRES_INVALID_ARGS;                                                \
    *lst = (Merry##name##List *)malloc(sizeof(Merry##name##List));             \
    if (!(*lst)) {                                                             \
      return MRES_SYS_FAILURE;                                                 \
    }                                                                          \
    (*lst)->buf = (type *)calloc(cap, sizeof(type));                           \
    if (!(*lst)->buf) {                                                        \
      free(*lst);                                                              \
      return MRES_SYS_FAILURE;                                                 \
    }                                                                          \
    (*lst)->cap = cap;                                                         \
    (*lst)->curr_ind = 0;                                                      \
    return MRES_SUCCESS;                                                       \
  }                                                                            \
  void merry_##name##_list_destroy(Merry##name##List *lst) {                   \
    merry_check_ptr(lst);                                                      \
    free(lst->buf);                                                            \
    free(lst);                                                                 \
  }                                                                            \
  mresult_t merry_##name##_list_push(Merry##name##List *lst, type *elem) {     \
    merry_check_ptr(lst);                                                      \
    merry_check_ptr(elem);                                                     \
    if (lst->curr_ind >= lst->cap)                                             \
      return MRES_CONT_FULL;                                                   \
    lst->buf[lst->curr_ind] = *elem;                                           \
    lst->curr_ind++;                                                           \
    return MRES_SUCCESS;                                                       \
  }                                                                            \
  mresult_t merry_##name##_list_pop(Merry##name##List *lst, type *elem) {      \
    merry_check_ptr(lst);                                                      \
    if (!elem)                                                                 \
      return MRES_INVALID_ARGS;                                                \
    if (lst->curr_ind == 0)                                                    \
      return MRES_CONT_EMPTY;                                                  \
    lst->curr_ind--;                                                           \
    *elem = lst->buf[lst->curr_ind];                                           \
    return MRES_SUCCESS;                                                       \
  }                                                                            \
  mresult_t merry_##name##_list_at(Merry##name##List *lst, type *elem,         \
                                   msize_t ind) {                              \
    merry_check_ptr(lst);                                                      \
    if (!elem)                                                                 \
      return MRES_INVALID_ARGS;                                                \
    if (ind >= lst->cap)                                                       \
      return MRES_NOT_EXISTS;                                                  \
    *elem = lst->buf[ind];                                                     \
    return MRES_SUCCESS;                                                       \
  }                                                                            \
  mresult_t merry_##name##_list_ref_of(Merry##name##List *lst, type **elem,    \
                                       msize_t ind) {                          \
    merry_check_ptr(lst);                                                      \
    if (!elem)                                                                 \
      return MRES_INVALID_ARGS;                                                \
    if (ind >= lst->cap)                                                       \
      return MRES_NOT_EXISTS;                                                  \
    *elem = &lst->buf[ind];                                                    \
    return MRES_SUCCESS;                                                       \
  }                                                                            \
  mresult_t merry_##name##_list_resize(Merry##name##List *lst,                 \
                                       msize_t resize_factor) {                \
    merry_check_ptr(lst);                                                      \
    type *new_buf = (type *)malloc(sizeof(type) * lst->cap * resize_factor);   \
    if (!new_buf)                                                              \
      return MRES_SYS_FAILURE;                                                 \
    mempcpy(new_buf, lst->buf, lst->curr_ind * sizeof(type));                  \
    type *tmp = lst->buf;                                                      \
    lst->buf = new_buf;                                                        \
    lst->cap *= resize_factor;                                                 \
    free(tmp);                                                                 \
    return MRES_SUCCESS;                                                       \
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
  mresult_t merry_lf_##name##_list_create(msize_t cap,                         \
                                          MerryLF##name##List **lst);          \
  void merry_lf_##name##_list_destroy(MerryLF##name##List *lst);               \
  mresult_t merry_lf_##name##_list_push(MerryLF##name##List *lst, type *elem); \
  mresult_t merry_lf_##name##_list_pop(MerryLF##name##List *lst, type *elem);  \
  msize_t merry_lf_##name##_list_size(MerryLF##name##List *lst);

#define _MERRY_DEFINE_LF_STATIC_LIST_(name, type)                              \
  mresult_t merry_lf_##name##_list_create(msize_t cap,                         \
                                          MerryLF##name##List **lst) {         \
    if (cap == 0)                                                              \
      return MRES_INVALID_ARGS;                                                \
    *lst = (MerryLF##name##List *)malloc(sizeof(MerryLF##name##List));         \
    if (!(*lst)) {                                                             \
      return MRES_SYS_FAILURE;                                                 \
    }                                                                          \
    (*list)->buf = (type *)calloc(cap, sizeof(type));                          \
                                                                               \
    if (!(*list)->buf) {                                                       \
      free(list);                                                              \
      return MRES_SYS_FAILURE;                                                 \
    }                                                                          \
    list->cap = cap;                                                           \
    list->curr_ind = 0;                                                        \
    return MRES_SUCCESS;                                                       \
  }                                                                            \
  void merry_lf_##name##_list_destroy(MerryLF##name##List *lst) {              \
    merry_check_ptr(lst);                                                      \
    free(lst->buf);                                                            \
    free(lst);                                                                 \
  }                                                                            \
  mresult_t merry_lf_##name##_list_push(MerryLF##name##List *lst,              \
                                        type *elem) {                          \
    merry_check_ptr(lst);                                                      \
    merry_check_ptr(elem);                                                     \
    msize_t ind = atomic_fetch_add_explicit((_Atomic msize_t *)&lst->curr_ind, \
                                            1, memory_order_relaxed);          \
    if (ind >= lst->cap) {                                                     \
      atomic_fetch_sub_explicit((_Atomic msize_t *)&lst->curr_ind, 1,          \
                                memory_order_relaxed);                         \
      return MRES_CONT_FULL;                                                   \
    }                                                                          \
    lst->buf[ind] = *elem;                                                     \
    return MRES_SUCCESS;                                                       \
  }                                                                            \
  mresult_t merry_lf_##name##_list_pop(MerryLF##name##List *lst, type *elem) { \
    merry_check_ptr(lst);                                                      \
    if (lst->curr_ind == 0)                                                    \
      return MRES_CONT_EMPTY;                                                  \
    msize_t ind = atomic_fetch_sub_explicit((_Atomic msize_t *)curr_ind, 1,    \
                                            memory_order_relaxed);             \
    *elem = lst->buf[ind];                                                     \
    return MRES_SUCCESS;                                                       \
  }                                                                            \
  _MERRY_ALWAYS_INLINE_ msize_t merry_lf_##name##_list_size(                   \
      MerryLF##name##List *lst) {                                              \
    merry_check_ptr(lst);                                                      \
    return lst->curr_ind;                                                      \
  }                                                                            \
  /*-----------------END STATIC LIST--------------------*/

#endif
