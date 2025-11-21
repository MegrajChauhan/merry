#ifndef _MERRY_ACCEPTOR_
#define _MERRY_ACCEPTOR_

#include <merry_types.h>
#include <merry_utils.h>
#include <merry_platform.h>
#include <merry_operations.h>
#include <merry_protector.h>
#include <merry_acceptor_commands.h>
#include <merry_queue.h>
#include <stdlib.h>

typedef struct MerryAcceptorCommand MerryAcceptorCommand;
typedef struct MerryAcceptor MerryAcceptor;

struct MerryAcceptorCommand {
	maccop_t opcode;
	msize_t custom_break;
};

struct MerryAcceptor {
	mmutex_t *shared_lock;
	MerrySQueue *queue;
	mbool_t started;
	msocket_t listening_socket;
	mbool_t CONTINUE;
	msize_t accepted_connections; // the number of accepted connections
	msize_t total_accepted_connections;
	msize_t connection_limit;
	msize_t interval; // listen for a connection after every 'interval' microsecond
	mbool_t STOP;
	mbool_t READY;
	mresult_t CRASHED;
	msize_t queue_size;
	MerryAcceptorCommand command;
};

mresult_t merry_acceptor_init(MerryAcceptor **acceptor, mmutex_t *shared_lock, msocket_t sock, msize_t queue_size);

mresult_t merry_accceptor_command(MerryAcceptor *acceptor, MerryAcceptorCommand *command);

void merry_acceptor_destroy(MerryAcceptor *acceptor);

_THRET_T_ merry_acceptor_run(mptr_t a);

#endif
