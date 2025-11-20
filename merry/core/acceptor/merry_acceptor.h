#ifndef _MERRY_ACCEPTOR_
#define _MERRY_ACCEPTOR_

#include <merry_types.h>
#include <merry_utils.h>
#include <merry_platform.h>
#include <merry_file.h>
#include <merry_operations.h>
#include <stdlib.h>

typedef struct MerryAcceptor MerryAcceptor;

struct MerryAcceptor {
	MerryFile file;
	msocket_t listening_socket;
	msize_t accepted_connections; // the number of accepted connections
	msize_t connection_limit;
	msize_t interval; // listen for a connection after every 'interval' microsecond
	msize_t STOP; // to close the acceptor
};

mresult_t merry_acceptor_init(MerryAcceptor **acceptor, );

#endif
