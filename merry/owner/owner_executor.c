#include <owner_executor.h>

static bool owner_init_port(size_t port_num, socket_t *res) {
	socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
	  FATAL("Failed to initialize port at %zu", port_num);
	  return false;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (bind(sock, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
	  FATAL("Failed to initialize port at %zu(Binding failed: %s)", port_num, strerr(errno));
	  CLOSESOCKET(sock);
	  return false;
	}

	*res = sock;
	return true;
}

static bool owner_list_for_polling(OwnerExecutor *e, OwnerClient *cl) {
#ifdef _USE_LINUX_
	struct epoll_event _ev = {0};
	_ev.event = EPOLLIN;
	_ev.data.ptr = cl;

	if (epoll_ctl(e->polling, EPOLL_CTL_ADD, cl->sock, &_ev) < 0) {
		FATAL("Failed to poll a socket: %s", strerr(errno));
		return false;
	}
#endif
	return true;
}

static bool owner_accept_socket(OwnerExecutor *e, socket_t *res) {
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	socket_t c = accept(e->ctx_socket, (struct sockaddr *)&addr, &len);
	if (c < 0)
	  return false;

	// for verification
	// We will send our secret passcode first
	// and then they will verify it and then they send their secret passcode
	// if it matches what we have then the connection is a go
	send(c, e->owner_pass, e->op_len, 0);
	*res = c;
	return true;
}

static bool owner_verify_socket(OwnerExecutor *e, socket_t sock) {
	// The pass has to be 32 bytes long.
	char pass[33] = {0};
	int len;
	if ((len = recv(sock, pass, 32, 0)) == 0) {
		LOG("CTX %zu closed connection...", sock);
		return false;
	}
	if (len < 32)
		return false;
	if (strlen(pass, e->client_pass) == 0)
		return true;
	return false;
}

OwnerExecutor *owner_executor_create(size_t ctx_port, size_t core_port) {
	if (ctx_port == core_port) {
		FATAL("Cannot have the save port for ctx and cores: %zu == %zu", ctx_port, core_port);
		return NULL;
	}
	OwnerExecutor *e = (OwnerExecutor*)malloc(sizeof(OwnerExecutor));

	if (!e) {
		FATAL("Failed to initialize executor", NULL);
		return NULL;
	}

	if (!owner_init_port(ctx_port, &e->ctx_socket)) {
		LOG("Opening ctx port...", NULL);
		free(e);
		return NULL;
	}	

	if (!owner_init_port(core_port, &e->core_socket)) {
		LOG("Opening core port...", NULL);
		CLOSESOCKET(e->ctx_socket);
		free(e);
		return NULL;
	}	

	if ((e->map = owner_ts_map_create(100)) == NULL) {
		CLOSESOCKET(e->ctx_socket);
		CLOSESOCKET(e->core_socket);
		free(e);
		return NULL;
	}

	if ((e->lst = owner_client_list_create(500)) == NULL) {
		CLOSESOCKET(e->ctx_socket);
		CLOSESOCKET(e->core_socket);
		owner_ts_map_destroy(e->map);
		free(e);
		return NULL;
	}

#ifdef _USE_LINUX_
	e->polling = epoll_create1(0);
#endif
	
	return e;
}

void owner_executor_add_pass(OwnerExecutor* e, const char *op, const char *cp) {
	e->owner_pass = op;
	e->client_pass = cp;
	e->op_len = strlen(op);
	e->cp_len = strlen(cp);	
}

void owner_executor_destroy(OwnerExecutor *executor) {
	CLOSESOCKET(e->ctx_socket);
	CLOSESOCKET(e->core_socket);
	owner_ts_map_destroy(e->map);
	owner_client_list_destroy(e->lst);
#ifdef _USE_LINUX_
	close(e->polling);
#endif
	free(executor);
}

void owner_run(OwnerExecutor *executor) {
	OwnerClientList *lst = executor->lst;
	OwnerTSCoreMap *map = executor->map;

	executor->active_ctx_conns = 0;

	OwnerClient cl;
	cl.socket = executor->ctx_socket;
	cl.verified = true;

	owner_client_list_push(lst, &cl);
	cl.verified = false;
	
	owner_list_for_polling(executor, owner_client_list_last(lst));	

	size_t count = 1;
	bool_t conn_made = false;
	
	while (true) {
#ifdef _USE_LINUX_
		struct epoll_event all_events[100];
		int n = epoll_wait(executor->polling, all_events, 100, 100); 
		if (n < 0) {
			if ((count % 100) == 0 && conn_made == false) {
				// We timedout on a connection
				FATAL("Owner didn't receive any connections: Terminating...", NULL);
				break;
			}
			count++;
			continue;
		}

		// handle the events
		for (size_t i = 0; i < (size_t)n; i++) {
			OwnerClient* sock = (OwnerClient*)all_events[i].data.ptr;
			if (sock->sock == executor->ctx_socket) {
				// We might have a connection
				if (!owner_accept_socket(executor, &cl.sock)) {
					LOG("Failed to accept socket connection...", NULL);
				}else {
					if (!owner_client_list_push(lst, &cl)) {
						LOG("Failed to add new ctx client... %zu", cl.sock);
					}else {
						executor->active_ctx_conns++;
						conn_made = true;
						owner_list_for_polling(executor, owner_client_list_last(lst));	
					}
				}
			} else {
				// Read the command and respond as needed.... 
				if (!sock->verified) {
					// try verification now
					if (!owner_verify_socket(executor, sock->sock)) {
					    CLOSESOCKET(sock->sock);
					    continue;
					}
					 else
					 	sock->verified = true;
				} else {
					// command execution....
				}
			}
			owner_list_for_polling(executor, sock);
		}
#endif
	}	
}
