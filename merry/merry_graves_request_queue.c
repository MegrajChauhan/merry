#include "merry_graves_request_queue.h"
#include "merry_types.h"
#include "merry_utils.h"

_MERRY_DEFINE_QUEUE_(GravesRequest, MerryGravesRequest *);

mresult_t merry_graves_req_queue_init() {
  mresult_t res = merry_GravesRequest_llqueue_init(&g_queue.req_queue);
  if (res != MRES_SUCCESS)
    return MRES_INVALID_ARGS;

  g_queue.accept_requests = mtrue;
  return MRES_SUCCESS;
}

mresult_t merry_SEND_REQUEST(MerryGravesRequest *creq) {
  merry_mutex_lock(g_queue.owner_lock);
  if (g_queue.accept_requests == mfalse) {
    merry_mutex_unlock(g_queue.owner_lock);
    return MRES_OPER_NOT_AVAI;
  }

  if (merry_GravesRequest_llqueue_push(g_queue.req_queue, &creq) !=
      MRES_SUCCESS) {
    merry_cond_signal(g_queue.owner_cond);
    merry_mutex_unlock(g_queue.owner_lock);
    return MRES_COMP_FAILURE;
  }
  creq->fufilled = mfalse;
  merry_cond_signal(g_queue.owner_cond);
  merry_cond_wait(creq->used_cond, g_queue.owner_lock);
  if (creq->fufilled == mfalse) {
  	// Interrupted...?
    merry_mutex_unlock(g_queue.owner_lock);
    return MRES_INTERRUPTED; // done
  }
  merry_mutex_unlock(g_queue.owner_lock);
  return MRES_SUCCESS;
}

mresult_t merry_SEND_REQUEST_async(MerryGravesRequest *creq) {
  merry_mutex_lock(g_queue.owner_lock);
  if (g_queue.accept_requests == mfalse) {
    merry_mutex_unlock(g_queue.owner_lock);
    return MRES_OPER_NOT_AVAI;
  }
  if (merry_GravesRequest_llqueue_push(g_queue.req_queue, &creq) !=
      MRES_SUCCESS) {
    merry_cond_signal(g_queue.owner_cond);
    merry_mutex_unlock(g_queue.owner_lock);
    return MRES_COMP_FAILURE;
  }
  creq->fufilled = mfalse;
  merry_cond_signal(g_queue.owner_cond);
  merry_mutex_unlock(g_queue.owner_lock);
  return MRES_SUCCESS;
}

mresult_t merry_graves_wants_work(MerryGravesRequest **req) {
  mresult_t res = merry_GravesRequest_llqueue_pop(g_queue.req_queue, req);
  return (res == MRES_SUCCESS) ? MRES_SUCCESS : MRES_FAILURE;
}

void merry_graves_req_no_more_requests() {
  merry_mutex_lock(g_queue.owner_lock);
  g_queue.accept_requests = mfalse;
  merry_mutex_unlock(g_queue.owner_lock);
}

void merry_graves_req_queue_free() {
  merry_GravesRequest_llqueue_destroy(g_queue.req_queue);
}

void merry_graves_req_register_wakeup(mcond_t *owner_cond,
                                      mmutex_t *owner_lock) {
  g_queue.owner_cond = owner_cond;
  g_queue.owner_lock = owner_lock;
}

mresult_t merry_init_request(MerryGravesRequest **req, mcond_t *cond, mbool_t async) {
	if (!req)
		return MRES_INVALID_ARGS;
	if (!async && !cond)
		return MRES_INVALID_ARGS;
	MerryGravesRequest *r = (MerryGravesRequest*)malloc(sizeof(MerryGravesRequest));
	if (!r)
		return MRES_SYS_FAILURE;
	r->used_cond = cond;
	*req = r;
	return MRES_SUCCESS;
}

_MERRY_ALWAYS_INLINE_ mresult_t merry_set_request_CREATE_CORE(MerryGravesRequest *req, mcore_t new_core_type, maddress_t st_address, mguid_t gid) {
   if (!req) return MRES_INVALID_ARGS;
   req->type = CREATE_CORE;
   req->args.create_core.new_core_type = new_core_type;
   req->args.create_core.st_addr = st_address;
   req->args.create_core.gid = gid;
   return MRES_SUCCESS;
}

_MERRY_ALWAYS_INLINE_ mresult_t merry_set_request_CREATE_GROUP(MerryGravesRequest *req) {
  if (!req) return MRES_INVALID_ARGS;
  req->type = CREATE_GROUP;
  return MRES_SUCCESS;
} 

_MERRY_ALWAYS_INLINE_ mresult_t merry_get_request_result_CREATE_CORE(MerryGravesRequest *req, msize_t *id, msize_t *uid) {
	if (!req || !id || !uid || req->type != CREATE_CORE)
		return MRES_INVALID_ARGS;
	*id = req->args.create_core.new_id;
	*uid = req->args.create_core.new_uid;
	return MRES_SUCCESS;
}

_MERRY_ALWAYS_INLINE_ mresult_t merry_get_request_result_CREATE_GROUP(MerryGravesRequest *req, msize_t *gid) {
	if (!req || !gid || req->type != CREATE_GROUP)
		return MRES_INVALID_ARGS;
	*gid = req->args.create_group.new_guid;
	return MRES_SUCCESS;
}
