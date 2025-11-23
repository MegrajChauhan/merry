#ifndef _OWNER_EXECUTOR_
#define _OWNER_EXECUTOR_

#include <owner_platform.h>
#include <owner_ts_core_map.h>
#include <owner_client_list.h>
#include <owner_logger.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct OwnerExecutor OwnerExecutor;

struct OwnerExecutor {
	socket_t ctx_socket;
	socket_t core_socket;
	size_t active_ctx_conns;
	OwnerTSCoreMap *map;
	OwnerClientList *lst;
	const char *owner_pass;
	const char *client_pass;
	size_t op_len;
	size_t cp_len;
#ifdef _USE_LINUX_
	int polling;
#endif
};

OwnerExecutor *owner_executor_create(size_t ctx_port, size_t core_port);

void owner_executor_add_pass(OwnerExecutor* e, const char *op, const char *cp);

void owner_executor_destroy(OwnerExecutor *executor);

void owner_run(OwnerExecutor *executor);

#endif
