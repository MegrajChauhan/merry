#include <merry_backend.h>

MerryCore *merry_find_unused_core(MerryCoreList *lst) {
	for (msize_t i = 0; i < merry_Core_list_size(lst); i++) {
		if (!lst->buf[i].connection_active)
			return &buf->buf[i];
	}
	return NULL;
}

mresult_t merry_create_core(MerryContext *ctx, mstr_t core_name, mid_t *core_id, mbool_t reuse_id) {
	if (!ctx || !core_id || !core_name) 
		return MRES_INVALID_ARGS;

	if (!ctx->ctx_initialized || !ctx->ctx_not_configurable)
 		return MRES_NOT_CONFIGURED;
 	   	
    if (ctx->owner_tid != pthread_self())
 		return MRES_OPER_NOT_PERM;

 	mstr_t path;
	MerryCore *core;
	mresult_t res;

	if ((res = merry_conf_load_find(ctx->configs, core_name, &path)) != MRES_SUCCESS)
		return res;
	
	if (reuse_id) {
		if (*core_id < ctx->core_count) {
			core = &ctx->core_list->buf[*core_id];
			if (core->connection_active)
				return MRES_CONN_ACTIVE;				
		}else{
			return MRES_INVALID_ARGS;
		}
	}else{
		if (ctx->config.set_limit_to_core_count && ctx->active_core_count > ctx->config.core_count_limit) 
			return MRES_RESOURCE_LIM_REACHED;
		else {
	        if ((core = merry_find_unused_core(ctx->core_list) == NULL){
		    MerryCore tmp = {0};
			if ((res = merry_Core_list_push(ctx->core_list, &tmp)) != MRES_SUCCESS) {
				// try resizing
				if ((res = merry_Core_list_resize(ctx->core_list, 2)) != MRES_SUCCESS)
					return res;
				// try pushing again
				merry_Core_list_push(ctx->core_list, &tmp);
			}
			merry_Core_list_ref_of(ctx->core_list, &core, merry_Core_list_size(ctx->core_list) - 1);
		}
		}
	}
	merry_get_random_number(&core->verification);
    MerryQwordToPtr ptr;
    ptr.qword = core->verification;
	mstr_t env[] = {
		(mstr_t)ptr.ptr,
		NULL
	};
	
	res = merry_spawn_process_env(&core->process_id, path, env);

	if (res != MRES_SUCCESS) 
		return res;

    // The core should be running but prepare to accept its connection
    core->verified = mfalse;
    core->merry_spawned = mtrue;

	ctx->ctx_expecting_connection = mtrue;
    // Now try to connect to the core
	res = merry_connect_a_core(ctx, core, )
	return MRES_SUCCESS;
}

mresult_t merry_destroy_core(MerryContext *ctx, mid_t core_id);
