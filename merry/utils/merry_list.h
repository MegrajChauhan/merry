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
#define merry_list_create(type, cap) (type *)_list_create(sizeof(type), cap)
#define merry_list_destroy(lst) _list_destroy(lst)
#define merry_list_push(lst, elem) _list_push(lst, elem)
#define merry_list_pop(lst) _list_pop(lst)
#define merry_list_at(lst, ind) _list_at(lst, ind)
#define merry_list_resize(lst, resize_factor) _list_resize(lst, resize_factor)

mptr_t _list_create(msize_t elen, msize_t cap);
void _list_destroy(mptr_t lst);
mret_t _list_push(mptr_t lst, mptr_t elem);
mptr_t _list_pop(mptr_t lst);
mptr_t _list_at(mptr_t lst, msize_t ind);
mptr_t _list_resize(mptr_t lst, msize_t resize_factor);

/*-----------------END STATIC LIST--------------------*/

/*------------------LF STATIC LIST---------------------*/
#define _MERRY_LF_LIST_METADATA_LEN_ 32
#define _MERRY_LF_LIST_METADATA_LEN_EXCLUDE_BUF_ 24
#define _MERRY_LF_LIST_GET_CAP_ 8
#define _MERRY_LF_LIST_GET_IND_ 16
#define _MERRY_LF_LIST_GET_ELEN_ 24

/*
 * elem_len -> 8-byte size_t
 * ind -> 8-bytes, current index
 * cap -> 8-byte size_t
 * buf -> 8-byte pointer
 * */
#define merry_lf_list_create(type, cap) (type *)_list_create(sizeof(type), cap)
#define merry_lf_list_destroy(lst) _list_destroy(lst)
#define merry_lf_list_push(lst, elem) _lf_list_push(lst, elem)
#define merry_lf_list_pop(lst) _lf_list_pop(lst)

mret_t _lf_list_push(mptr_t lst, mptr_t elem);
mptr_t _lf_list_pop(mptr_t lst);

/*-----------------END STATIC LIST--------------------*/

#endif
