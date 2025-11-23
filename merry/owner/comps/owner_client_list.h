#ifndef _OWNER_CLIENT_LIST_
#define _OWNER_CLIENT_LIST_

#include <owner_platform.h>
#include <owner_logger.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct OwnerClientList OwnerClientList;
typedef struct OwnerClient OwnerClient;

struct OwnerClient {
	socket_t sock;
	bool verified;
};

struct OwnerClientList {
	OwnerClient *buf;
	size_t cap;
	size_t curr_ind;
};

#define owner_client_list_last(lst) ((&lst->buf[lst->curr_ind - 1]))

OwnerClientList* owner_client_list_create(size_t cap);
void owner_client_list_destroy(OwnerClientList *lst);
bool owner_client_list_push(OwnerClientList *lst, OwnerClient *elem);
bool owner_client_list_pop(OwnerClientList *lst, OwnerClient *elem);
bool owner_client_list_at(OwnerClientList *lst, OwnerClient *elem, size_t ind);
bool owner_client_list_resize(OwnerClientList *lst, size_t resize_factor);
size_t owner_client_list_size(OwnerClientList *lst);
size_t owner_client_list_index_of(OwnerClientList *lst, OwnerClient *elem);

#endif
