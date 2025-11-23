#ifndef _OWNER_CORE_LIST_
#define _OWNER_CORE_LIST_

#include <owner_platform.h>
#include <owner_logger.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct OwnerCoreList OwnerCoreList;
typedef struct OwnerCore OwnerCore;

struct OwnerCore {
	socket_t sock;
	bool identified;
	size_t _at;
};

struct OwnerCoreList {
	OwnerCore *buf;
	size_t cap;
	size_t curr_ind;
};

#define owner_core_list_last(lst) ((&lst->buf[lst->curr_ind - 1]))

OwnerCoreList* owner_core_list_create(size_t cap);
void owner_core_list_destroy(OwnerCoreList *lst);
bool owner_core_list_push(OwnerCoreList *lst, OwnerCore *elem);
bool owner_core_list_pop(OwnerCoreList *lst, OwnerCore *elem);
bool owner_core_list_at(OwnerCoreList *lst, OwnerCore *elem, size_t ind);
bool owner_core_list_resize(OwnerCoreList *lst, size_t resize_factor);
size_t owner_core_list_size(OwnerCoreList *lst);
size_t owner_core_list_index_of(OwnerCoreList *lst, OwnerCore *elem);

#endif
