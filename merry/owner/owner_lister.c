#include <owner_lister.h>

static bool owner_lister_list_for_polling(OwnerLister *l, OwnerCore *c) {
#ifdef _USE_LINUX_
	struct epoll_event _ev = {0};
	_ev.event = EPOLLIN;
	_ev.data.ptr = c;

	if (epoll_ctl(l->polling, EPOLL_CTL_ADD, c->sock, &_ev) < 0) {
		FATAL("Failed to poll a core: %s", strerr(errno));
		return false;
	}
#endif
	return true;
}

static bool owner_lister_accept_socket(OwnerLister *l, socket_t *res) {
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	socket_t c = accept(e->sock, (struct sockaddr *)&addr, &len);
	if (c < 0)
	  return false;
	*res = c;
	return true;
}

OwnerLister *owner_lister_create(OwnerTSCoreMap *map, socket_t sock) {
	OwnerLister *l = (OwnerLister*)malloc(sizeof(OwnerLister));
	if (!l) {
		FATAL("Failed to initialize lister %s", strerr(errno));
		return NULL;
	}

	if ((l->list = owner_core_list_create(500)) == NULL) {
		FATAL("Failed to initialize lister %s", strerr(errno));
		free(l);
		return NULL;
	}

	l->map = map;
	l->sock = sock;
	l->loops = 0;
#ifdef _USE_LINUX_
	l->polling = epoll_create1(0);
#endif
	return l;
}

void owner_lister_destroy(OwnerLister *l) {
	owner_core_list_destroy(l->list);
#ifdef _USE_LINUX_
	close(l->polling);
#endif
	free(l);
}

void *owner_run_lister(void *arg) {
	OwnerLister *l = (OwnerLister*)arg;
	OwnerCore c;
	c.sock = l->sock;
	c.identified = false;
	c._at = 0;

	owner_core_list_push(l->list, &c);
	owner_lister_list_for_polling(l, owner_core_list_last(l->list));

	l->die = false;
	
	while (1) {
		if (l->die)
			break;
		if (l->loops % 500 == 0) {
			// Kill inactive connections
		}
#ifdef _USE_LINUX_
	struct epoll_event all_events[100];
	int n = epoll_wait(l->polling, all_events, 500, 100); 
	if (n > 0) {
		for (size_t i = 0; i < n; i++) {
			OwnerCore *core = (OwnerCore*)all_events[i].data.ptr;
			if (core->sock == l->sock) {
				if (!owner_lister_accept_socket(l, &c.sock)) {
							LOG("Failed to accept socket connection...", NULL);
				}else {
					if (!owner_client_list_push(l->list, &c)) {
							LOG("Failed to add new core client... %zu", c.sock);
					}else {
						owner_list_for_polling(l, owner_client_list_last(l->list));	
					}
				}
				owner_list_for_polling(l, core);
			} else {
				// We have a core that is probably trying to identify itself
				//....
			}
		}
	}
	l->loops++;

#endif
	}	
	return NULL;
}
