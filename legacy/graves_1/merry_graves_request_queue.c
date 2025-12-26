#include "merry_graves_request_queue.h"
#include "merry_types.h"
#include "merry_utils.h"

_MERRY_DEFINE_QUEUE_(GravesRequest, MerryGravesRequest *);

mret_t merry_graves_req_queue_init() {
  mresult_t res = merry_GravesRequest_llqueue_init(&g_queue.req_queue);
  if (res != MRES_SUCCESS)
    return RET_FAILURE;

  g_queue.accept_requests = mtrue;
  return RET_SUCCESS;
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

mret_t merry_request_set_CREATE_CORE(MerryRequestArgs *args, mcore_t new_core_type, maddress_t st_address, mbool_t same_group, mbool_t new_group, mguid_t gid) {
   if (!args) return RET_FAILURE;
   args->create_core.new_core_type = new_core_type;
   args->create_core.st_addr = st_address;
   args->create_core.gid = gid;
   args->create_core.new_group = new_group;
   args->create_core.same_group = same_group;
   return RET_SUCCESS;
}

_MERRY_ALWAYS_INLINE_ mret_t merry_request_get_CREATE_CORE(MerryRequestArgs *args, mguid_t *gid, mid_t *id, muid_t *uid) {
  if (!args || !gid || !id || !uid) return RET_FAILURE;
  *gid = args->create_core.gid;
  *id = args->create_core.new_id;
  *uid = args->create_core.new_uid;
  return RET_SUCCESS;
}

_MERRY_ALWAYS_INLINE_ mret_t merry_request_get_CREATE_GROUP(MerryRequestArgs *args, mguid_t *gid) {
  if (!args || !gid) return RET_FAILURE;
  *gid = args->create_group.new_guid;
  return RET_SUCCESS;
} 

_MERRY_ALWAYS_INLINE_ mret_t merry_request_set_GET_GROUP_DETAILS(MerryRequestArgs *args, mguid_t gid) {
  if (!args) return RET_FAILURE;
  args->get_group_details.guid = gid;
  return RET_SUCCESS;
}

_MERRY_ALWAYS_INLINE_ mret_t merry_request_get_GET_GROUP_DETAILS(MerryRequestArgs *args, msize_t *core_count, msize_t *active_core_count) {
  if (!args || !core_count || !active_core_count) return RET_FAILURE;
  *active_core_count =  args->get_group_details.active_core_count;
  *core_count = args->get_group_details.core_count;
  return RET_SUCCESS;
}

_MERRY_ALWAYS_INLINE_ mret_t merry_request_get_GET_SYSTEM_DETAILS(MerryRequestArgs *args, msize_t *grp_count, msize_t *core_count, msize_t *active_core_count) {
  if (!args || !grp_count || !core_count || !active_core_count) return RET_FAILURE;
  *grp_count = args->get_system_details.grp_count;
  *core_count = args->get_system_details.core_count;
  *active_core_count = args->get_system_details.active_core_count;
  return RET_SUCCESS;
}
