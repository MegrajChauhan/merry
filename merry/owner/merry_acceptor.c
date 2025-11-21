#include <merry_acceptor.h>

mresult_t merry_acceptor_init(MerryAcceptor **acceptor, mmutex_t *shared_lock, msocket_t sock, msize_t queue_size) {
	if (!acceptor || !shared_lock || !queue)
		return MRES_INVALID_ARGS;
    MerryAcceptor* a = (MerryAcceptor*)malloc(sizeof(MerryAcceptor));
 	if (!a)
    	return MRES_SYS_FAILURE;
 
 	a->shared_lock = shared_lock;
 	a->listening_socket = sock;
 	a->queue = NULL;
 	a->accepted_connections = 0;
 	a->total_accepted_connections = 0;
 	a->queue_size = queue_size;
	a->STOP = mfalse;
	a->interval = 100;
	a->connection_limit = merry_squeue_size(*queue);
 	*acceptor = a;
 	return MRES_SUCCESS;
}

void merry_acceptor_destroy(MerryAcceptor *acceptor) {
  free(acceptor);
}

mresult_t merry_accceptor_command(MerryAcceptor *acceptor, MerryAcceptorCommand *command) {
	if (!acceptor || !command)
		return MRES_INVALID_ARGS;

	if (acceptor->started)
		return MRES_NOT_ALLOWED;

	switch (command->opcode) {
		case ACC_RUN_TILL_FULL:
			break;
		case ACC_RUN_TILL_CUSTOM:
			if (command->custom_break == 0)
				return MRES_INVALID_ARGS;
		    acceptor->connection_limit = command->custom_break;
		    break;
		case ACC_RUN_INDEFINITELY:
			acceptor->connection_limit = (msize_t)(-1);
			break;
		default:
			return MRES_INVALID_ARGS;
	}
	
	acceptor->command = *command;

	mresult_t res;
	if ((res = merry_create_squeue(&acceptor->queue, acceptor->queue_size, sizeof(msocket_t))) != MRES_SUCCESS)
		return res;
	
	return MRES_SUCCESS;
}

_THRET_T_ merry_acceptor_run(mptr_t a) {
  MerryAcceptor *acceptor = (MerryAcceptor*)a;
  acceptor->started = mtrue;
  acceptor->READY = mfalse;
  while (!acceptor->STOP) {
    msocket_t fd;
 
    if (acceptor->accepted_connections == acceptor->connection_limit) {
    	// if the acceptor is configured to run ACC_RUN_INDEFINITELY, then this
    	// condition will never be true hence it must be true because of ACC_RUN_TILL_CUSTOM
    	acceptor->CONTINUE = mfalse;
    	acceptor->READY = mtrue;
    	while (!acceptor->CONTINUE)
	   		usleep(acceptor->interval);
        acceptor->READY = mfalse;
        acceptor->accepted_connections = 0;
        break; // No need to continue
    }

    if (merry_squeue_full(acceptor->queue)) {
    	acceptor->CONTINUE = mfalse;
    	acceptor->READY = mtrue;
    	while (!acceptor->CONTINUE)
    		usleep(acceptor->interval);
    	acceptor->READY = mfalse;
    	mresult_t res;
    	if ((res = merry_create_squeue(&acceptor->queue, acceptor->queue_size, sizeof(msocket_t))) != MRES_SUCCESS) {
			acceptor->CRASHED = res;
    		break;
   		}
   		acceptor->connection_limit -= acceptor->accepted_connections;
   		acceptor->accepted_connections = 0;
    }

    if (merry_socket_accept_conn(&acceptor->listening_socket, &fd) == MRES_SUCCESS) {
		merry_squeue_enqueue((acceptor->queue),(mptr_t)(&fd)); // will not fail
		acceptor->accepted_connections++;
		acceptor->total_accepted_connections++;
    }
    usleep(acceptor->interval);
  }
  acceptor->started = mfalse;
  return (_THRET_T_)0;
}
