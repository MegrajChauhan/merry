#ifndef _MERRY_GRAVES_REQUEST_QUEUE_
#define _MERRY_GRAVES_REQUEST_QUEUE_

#include <merry_core_types.h>
#include <merry_graves_defs.h>
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

_MERRY_LOCAL_ MerryGravesRequestQueueHandler g_queue;

mret_t merry_graves_req_queue_init();

void merry_graves_req_register_wakeup(mcond_t *owner_cond,
                                      mmutex_t *owner_lock);

void merry_SEND_REQUEST(MerryGravesRequest *creq);

mret_t merry_graves_wants_work(MerryGravesRequest **req);

void merry_graves_req_no_more_requests();

void merry_graves_req_queue_free();

#endif
