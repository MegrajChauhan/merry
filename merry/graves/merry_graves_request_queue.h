#ifndef _MERRY_GRAVES_REQUEST_QUEUE_
#define _MERRY_GRAVES_REQUEST_QUEUE_

#include <merry_core_types.h>
#include <merry_graves_defs.h>
#include <merry_operations.h>
#include <merry_protectors.h>
#include <merry_queue.h>
#include <merry_requests.h>

typedef struct MerryGravesRequestQueueHandler MerryGravesRequestQueueHandler;
_MERRY_DECLARE_QUEUE_(GravesRequest, MerryGravesRequest *);

struct MerryGravesRequestQueueHandler {
  MerryLLGravesRequestQueue *req_queue;
  mmutex_t *owner_lock;
  mcond_t *owner_cond;
  MerryGravesRequest req;
  mbool_t accept_requests;
};

_MERRY_INTERNAL_ MerryGravesRequestQueueHandler g_queue;

mret_t merry_graves_req_queue_init();

void merry_graves_req_register_wakeup(mcond_t *owner_cond,
                                      mmutex_t *owner_lock);

mresult_t merry_SEND_REQUEST(MerryGravesRequest *creq);

mresult_t merry_SEND_REQUEST_async(MerryGravesRequest *creq);

mresult_t merry_graves_wants_work(MerryGravesRequest **req);

void merry_graves_req_no_more_requests();

void merry_graves_req_queue_free();
mret_t merry_request_set_CREATE_CORE(MerryRequestArgs *args, mcore_t new_core_type, maddress_t st_address, mbool_t same_group, mbool_t new_group, mguid_t gid);
mret_t merry_request_get_CREATE_CORE(MerryRequestArgs *args, mguid_t *gid, mid_t *id, muid_t *uid);
mret_t merry_request_get_CREATE_GROUP(MerryRequestArgs *args, mguid_t *gid);
mret_t merry_request_set_GET_GROUP_DETAILS(MerryRequestArgs *args, mguid_t gid);
mret_t merry_request_get_GET_GROUP_DETAILS(MerryRequestArgs *args, msize_t *core_count, msize_t *active_core_count);
mret_t merry_request_get_GET_SYSTEM_DETAILS(MerryRequestArgs *args, msize_t *grp_count, msize_t *core_count, msize_t *active_core_count);

#endif
