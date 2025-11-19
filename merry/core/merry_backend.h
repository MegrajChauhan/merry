#ifndef _MERRY_BACKEND_
#define _MERRY_BACKEND_

#include <merry_types.h>
#include <merry_utils.h>
#include <merry_socket.h>
#include <merry_process.h>
#include <merry_core.h>
#include <merry_conf_load.h>
#include <merry_logger.h>
#include <merry_list.h>
#include <merry_operations.h>
#include <merry_platform.h>
#include <merry_ctx_change_opcodes.h>
#include <merry_communication_protocol.h>
#include <stdlib.h>
#include <stdarg.h>

_MERRY_DECLARE_STATIC_LIST_(Core,MerryCore);

typedef struct MerryContext MerryContext;
typedef struct MerryContextConfig MerryContextConfig;

struct MerryContextConfig {
	mbool_t no_waiting_for_client_conn; // don't wait for a client to appear

	mbool_t non_standard_port;
	msize_t port_num;

	mbool_t custom_config_file;
	mstr_t path_to_custom_config_file;

	mbool_t set_limit_to_core_count;
	msize_t core_count_limit;
};

struct MerryContext {
	MerryCoreList *core_list;
    MerryConfLoad *configs;
	msize_t core_count;
	msize_t active_core_count;
	mthread_t owner_tid;
	MerryContextConfig config;
	msocket_t listening_port;

	// flags
	mbool_t ctx_expecting_connection;
	mbool_t ctx_initialized;
	mbool_t ctx_not_configurable;
};

// They can create a context and then configure it for themselves
mresult_t merry_ctx_init(MerryContext *ctx);

mresult_t merry_ctx_config(MerryContext *ctx, mctxopcode_t OP, ...);

mresult_t merry_ctx_ready(MerryContext *ctx);

mresult_t merry_ctx_destroy(MerryContext *ctx);

// The following are just commands now
mresult_t merry_create_core(MerryContext *ctx, mstr_t core_name, mid_t *core_id, mbool_t reuse_id);

mresult_t merry_destroy_core(MerryContext *ctx, mid_t core_id);

#endif
