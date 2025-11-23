#ifndef _OWNER_LISTER_
#define _OWNER_LISTER_

#include <owner_platform.h>
#include <owner_core_list.h>
#include <owner_ts_core_map.h>
#include <owner_logger.h>
#include <stdlib.h>

typedef struct OwnerLister OwnerLister;

struct OwnerLister {
	socket_t sock;
	OwnerTSCoreMap *map;
	OwnerCoreList *list;
	size_t loops;
#ifdef _USE_LINUX_
	int polling;
#endif	
	_Atomic bool die;
};

OwnerLister *owner_lister_create(OwnerTSCoreMap *map, socket_t sock);

void owner_lister_destroy(OwnerLister *l);

void* owner_run_lister(void *arg);

#endif
