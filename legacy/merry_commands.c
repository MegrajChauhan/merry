#include <merry_backend.h>
#include <merry_core_connection.h>

MerryCore *merry_find_unused_core(MerryCoreList *lst) {
	for (msize_t i = 0; i < merry_Core_list_size(lst); i++) {
		if (!lst->buf[i].connection_active)
			return &buf->buf[i];
	}
	return NULL;
}

mresult_t merry_load_core(MerryContext *ctx, mstr_t core_name, mid_t *core_id, mbool_t reuse_id) {
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
	      if ((core = merry_find_unused_core(ctx->core_list) == NULL){
				if (ctx->config.set_limit_to_core_count && ctx->active_core_count > ctx->config.core_count_limit) 
					return MRES_RESOURCE_LIM_REACHED;
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
	merry_get_random_number(&core->verification);
    MerryQwordToPtr ptr;
    ptr.qword = core->verification;
    byte_t var[65] = {0};
#ifdef _USE_LINUX_
    snprintf(var,sizeof(var), "CT=%lu",core->verification);
	mstr_t env[] = {
		var,
		NULL
	};
#else

#endif
	
	res = merry_spawn_process_env(&core->process_id, path, env);

	if (res != MRES_SUCCESS) 
		return res;

    // The core should be running but prepare to accept its connection
    core->verified = mfalse;
    core->merry_spawned = mtrue;

	ctx->ctx_expecting_connection = mtrue;
    // Now try to connect to the core
    
	res = merry_connect_a_core(ctx, core, core_name, mtrue);
	if (res == MRES_SUCCESS) 
    	*core_id = merry_Core_list_ref_of(ctx->core_list)-1;
	ctx->ctx_expecting_connection = mfalse;
	return res;
}

mresult_t merry_destroy_core(MerryContext *ctx, mid_t core_id) {
	if (!ctx)
		return MRES_INVALID_ARGS;
	if (!ctx->ctx_initialized || !ctx->ctx_not_configurable)
 		return MRES_NOT_CONFIGURED;
    if (ctx->owner_tid != pthread_self())
 		return MRES_OPER_NOT_PERM;

	MerryCore *core;
	if (core_id < ctx->core_count) {
		core = &ctx->core_list->buf[core_id];
		if (core->connection_active)
			return MRES_CONN_ACTIVE;
	} else
	  return MRES_NOT_EXISTS;
	CLOSESOCKET(core->comms);
	return MRES_SUCCESS;
}

mresult_t merry_unload_core(MerryContext *ctx, mid_t core_id) {
	if (!ctx)
		return MRES_INVALID_ARGS;
	if (!ctx->ctx_initialized || !ctx->ctx_not_configurable)
 		return MRES_NOT_CONFIGURED;
    if (ctx->owner_tid != pthread_self())
 		return MRES_OPER_NOT_PERM;

	MerryCore *core;
	if (core_id < ctx->core_count) {
		core = &ctx->core_list->buf[core_id];
		if (!core->connection_active)
			return MRES_SUCCESS;
	}
	core->connection_active = mfalse;

	// Tell the core that it can terminate
	// but there will be a simple protocol for that
	// If the core requires the user to finish something such as freeing stuff, then
	// it can make such requests and only after they are fulfilled will the core unload
	// The initial handshake for verification is simple
	// The rest of the comms are slightly different
	// The way that communication works is like such:
	// - The user wants to make a request. The request is forwarded but the result isn't
	//   required right away.
	// - The user may request for a result at a later time which will also be forwarded.
	// The user may batch the requests and request for a specific result at a given time.
	// Once the request has been submitted for something, the core should return an ID 
	// specifically for that request.
	// The id is transferred to the user which they may later use to see progress on the
	// request. The core should never respond unless a request is made.    
}
