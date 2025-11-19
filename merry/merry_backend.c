#include <merry_backend.h>

_MERRY_DECLARE_STATIC_LIST_(Core, MerryCore);

mresult_t merry_ctx_init(MerryContext *ctx) {
	if (!ctx)
		return MRES_INVALID_ARGS;

	if (ctx->ctx_initialized)
		return MRES_NOT_ALLOWED;

	ctx->core_count = 0;
	ctx->active_core_count = 0;
	ctx->owner_tid = pthread_self(); // for Linux
	ctx->config.no_waiting_for_client_conn = mfalse;
	ctx->config.non_standard_port = mfalse;
	ctx->config.port_num = 0; 
	ctx->config.custom_config_file = mfalse;
	ctx->config.path_to_custom_config_file = NULL;
	ctx->config.set_limit_to_core_count = mfalse;
	ctx->config.core_count_limit = 0;
	ctx->ctx_initialized = mtrue;
	return MRES_SUCCESS;
}

mresult_t merry_ctx_config(MerryContext *ctx, mctxopcode_t OP, ...) {
	if (!ctx)
		return MRES_INVALID_ARGS;

	if (!ctx->ctx_initialized)
		return MRES_NOT_CONFIGURED;

	if (ctx->owner_tid != pthread_self())
		return MRES_OPER_NOT_PERM;

	if (ctx->ctx_not_configurable)
		return MRES_NOT_ALLOWED;

	va_list args;
	va_start(args, OP);

	switch (OP) {
		case CTX_CONFIG_NO_WAITING_FOR_CLIENT_CONN:
			ctx->config.no_waiting_for_client_conn = mtrue;
			break;
		case CTX_CONFIG_NON_STANDARD_PORT:
			ctx->config.non_standard_port = mtrue;
			ctx->config.port_num = va_arg(args, msize_t);
			break;
		case CTX_CONFIG_CUSTOM_ENTRIES:
			ctx->config.custom_config_file = mtrue;
			ctx->config.path_to_custom_config_file = va_arg(args, mstr_t);
			break;
		case CTX_CONFIG_SET_CORE_COUNT_LIMIT:
			ctx->config.set_limit_to_core_count = mtrue;
			ctx->config.core_count_limit = va_arg(args, msize_t);
			break;
		default:
			return MRES_INVALID_ARGS;
	}
	
	va_end(args);
	return MRES_SUCCESS;
}

mresult_t merry_ctx_ready(MerryContext *ctx) {
	if (!ctx)
		return MRES_INVALID_ARGS;
 
 	if (!ctx->ctx_initialized)
 		return MRES_NOT_CONFIGURED;
 	   	
    if (ctx->owner_tid != pthread_self())
 		return MRES_OPER_NOT_PERM;

    msize_t core_count = 10; // default and this is dynamic
    mstr_t config_path = "merry/conf/merry_conf.conf"; // default
    msize_t port = 8080; // haven't decided yet

    if (ctx->config.non_standard_port)
    	port = ctx->config.port_num;
    if (ctx->config.custom_config_file)
    	config_path = ctx->config.path_to_custom_config_file;
    if (ctx->config.set_limit_to_core_count)
    	core_count = ctx->config.core_count_limit;

    // Prepare everything
    mresult_t res = merry_Core_list_create(core_count, &ctx->core_list);
    if (res != MRES_SUCCESS)
    	return res;

    res = merry_conf_load_init(&ctx->configs);
    if (res != MRES_SUCCESS)
    	return res;

    res = merry_conf_load_load(ctx->configs, config_path);
    if (res != MRES_SUCCESS)
    	return res;

    ctx->core_count = core_count;

    res = merry_open_socket_conn(&ctx->listening_port, port);

    if (res != MRES_SUCCESS)
    	return res;

    if (ctx->config.no_waiting_for_client_conn) {
		merry_non_blocking_socket(&ctx->listening_port);
    } 
    	
	ctx->ctx_not_configurable = mtrue;
 	return MRES_SUCCESS;        	   		   	
}

mresult_t merry_ctx_destroy(MerryContext *ctx) {
	if (!ctx)
		return MRES_INVALID_ARGS;
	if (ctx->owner_tid != pthread_self() || ctx->active_core_count != 0)
		return MRES_NOT_ALLOWED;
	if (ctx->core_list)
		merry_Core_list_destroy(ctx->core_list);
	if (ctx->configs) 
	    merry_conf_load_destroy(ctx->configs);
	if (ctx->ctx_not_configurable)
		CLOSESOCKET(ctx->listening_port);
	ctx->ctx_initialized = mfalse;
	ctx->ctx_not_configurable = mfalse;
	return MRES_SUCCESS;	
}
