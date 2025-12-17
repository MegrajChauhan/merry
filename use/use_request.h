#ifndef _USE_REQUEST_
#define _USE_REQUEST_

#include <use_defs.h>
#include <use_utils.h>
#include <use_types.h>

extern result_t SEND_REQUEST(GravesRequest *creq) _ALIAS_(merry_SEND_REQUEST);

extern result_t SEND_REQUEST_async(GravesRequest *creq)
    _ALIAS_(merry_SEND_REQUEST_async);

// extern ret_t request_set_CREATE_CORE(RequestArgs *args, core_t new_core_type, address_t st_address, bool_t same_group, bool_t new_group, mguid_t gid) _ALIAS_(merry_request_set_CREATE_CORE) ;
// extern ret_t request_get_CREATE_CORE(RequestArgs *args, mguid_t *gid, id_t *id, uid_t *uid) _ALIAS_(merry_request_get_CREATE_CORE) ;
// extern ret_t request_get_CREATE_GROUP(RequestArgs *args, mguid_t *gid) _ALIAS_(merry_request_get_CREATE_GROUP) ;
// extern ret_t request_set_GET_GROUP_DETAILS(RequestArgs *args, mguid_t gid) _ALIAS_(merry_request_set_GET_GROUP_DETAILS) ;
// extern ret_t request_get_GET_GROUP_DETAILS(RequestArgs *args, size_t *core_count, size_t *active_core_count) _ALIAS_(merry_request_get_GET_GROUP_DETAILS) ;
// extern ret_t request_get_GET_SYSTEM_DETAILS(RequestArgs *args, size_t *grp_count, size_t *core_count, size_t *active_core_count) _ALIAS_(merry_request_get_GET_SYSTEM_DETAILS) ;
#endif
