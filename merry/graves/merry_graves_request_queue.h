#ifndef _MERRY_GRAVES_REQUEST_QUEUE_
#define _MERRY_GRAVES_REQUEST_QUEUE_

#include <merry_error_stack.h>
#include <merry_protectors.h>
#include <merry_queue.h>
#include <merry_requests.h>

typedef struct MerryGravesReqQueue MerryGravesReqQueue;
typedef struct MerryGravesRequest MerryGravesRequest;

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
};

_MERRY_LOCAL_ MerryGravesReqQueue g_queue;
_MERRY_LOCAL_ MerryErrorStack g_queue_stack; // The queue's personal error stack

mret_t merry_graves_req_queue_init(mcond_t *graves_cond, MerryErrorStack *st);

mret_t merry_SEND_REQUEST(MerryGravesRequest *creq, mcond_t *cond);

mret_t merry_graves_wants_work(MerryGravesRequest **req);

MerryErrorStack *merry_graves_get_req_queue_error_stack();

void merry_graves_req_queue_free();

#endif
