#ifndef _MERRY_GRAVES_REQUEST_QUEUE_
#define _MERRY_GRAVES_REQUEST_QUEUE_

#include <merry_core_interface.h>
#include <merry_core_types.h>
#include <merry_protectors.h>
#include <merry_queue.h>
#include <merry_requests.h>
#include <merry_results.h>

typedef struct MerryGravesRequestQueueHandler MerryGravesRequestQueueHandler;
_MERRY_DECLARE_QUEUE_(GravesRequest, MerryGravesRequest *);

struct MerryGravesRequestQueueHandler {
  MerryLLGravesRequestQueue *req_queue;
  mmutex_t *owner_lock;
  mcond_t *owner_cond;
  mbool_t accept_requests;
};

_MERRY_INTERNAL_ MerryGravesRequestQueueHandler g_queue;

mresult_t merry_graves_req_queue_init();

void merry_graves_req_register_wakeup(mcond_t *owner_cond,
                                      mmutex_t *owner_lock);

mresult_t merry_SEND_REQUEST(MerryGravesRequest *creq);

mresult_t merry_SEND_REQUEST_async(MerryGravesRequest *creq);

mresult_t merry_graves_wants_work(MerryGravesRequest **req);

void merry_graves_req_no_more_requests();

void merry_graves_req_queue_free();

#endif
