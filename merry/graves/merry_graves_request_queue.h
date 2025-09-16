#ifndef _MERRY_GRAVES_REQUEST_QUEUE_
#define _MERRY_GRAVES_REQUEST_QUEUE_

#include <merry_core_types.h>
#include <merry_error_stack.h>
#include <merry_graves_defs.h>
#include <merry_protectors.h>
#include <merry_queue.h>
#include <merry_requests.h>

typedef struct MerryGravesReqQueue MerryGravesReqQueue;
typedef struct MerryGravesRequest MerryGravesRequest;
typedef union MerryRequestArgs MerryRequestArgs;

struct MerryGravesRequest {
  mgreq_t type;
  mid_t id;
  muid_t uid;
  mguid_t guid;
};

struct MerryGravesReqQueue {
  MerryDynamicQueue *req_queue;
  mcond_t queue_cond;
  mmutex_t queue_lock;
  mcond_t *graves_cond;
  MerryGravesRequest req;
  mbool_t accept_requests;
  MerryQueueNode *last_temp_req;
};

/*
 * Each core has it's own convention for argument passing
 * but will use this to convey the arguments to Graves
 * */
union MerryRequestArgs {
  struct {
    mcore_t new_core_type;
    maddress_t st_addr;
    mbool_t same_group; // If this is set, gid and new_group are ignored else
    mbool_t new_group;  // If set and !same_group then a new group is created by
                        // ignoring gid
    mguid_t gid; // if !same_group and !new_group use this to find a group
  } create_core;
};

_MERRY_LOCAL_ MerryGravesReqQueue g_queue;
_MERRY_LOCAL_ MerryErrorStack g_queue_stack; // The queue's personal error stack

mret_t merry_graves_req_queue_init(mcond_t *graves_cond, MerryErrorStack *st);

mret_t merry_SEND_REQUEST(MerryGravesRequest *creq, mcond_t *cond);

mret_t merry_graves_wants_work(MerryGravesRequest **req);

mret_t merry_graves_temporary_get_work(MerryGravesRequest **req);

MerryErrorStack *merry_graves_get_req_queue_error_stack();

void merry_graves_req_no_more_requests();

void merry_graves_req_queue_free();

#endif
