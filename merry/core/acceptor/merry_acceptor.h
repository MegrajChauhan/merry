#ifndef _MERRY_ACCEPTOR_
#define _MERRY_ACCEPTOR_

#include <merry_types.h>
#include <merry_utils.h>
#include <merry_platform.h>
#include <merry_operations.h>
#include <merry_list.h>
#include <merry_protector.h>
#include <merry_client_list.h>
#include <stdlib.h>

typedef struct MerryAcceptor MerryAcceptor;

struct MerryAcceptor {
	mmutex_t *shared_lock;
 MerryClientList **list;
	msocket_t listening_socket;
	msize_t accepted_connections; // the number of accepted connections
	msize_t connection_limit;
	msize_t interval; // listen for a connection after every 'interval' microsecond
	msize_t STOP; // to close the acceptor
};

mresult_t merry_acceptor_init(MerryAcceptor **acceptor, mmutex_t *shared_lock, MerryClientList **list, msocket_t sock);

void merry_acceptor_destroy(MerryAcceptor *acceptor);

_THRET_T_ merry_acceptor_run(mptr_t a);

#endif
