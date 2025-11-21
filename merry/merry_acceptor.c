#include <merry_acceptor.h>

mresult_t merry_acceptor_init(MerryAcceptor **acceptor, mmutex_t *shared_lock, MerrySQueue **queue, msocket_t sock) {
  if (!acceptor || !shared_lock || !queue)
       return MRES_INVALID_ARGS;
  MerryAcceptor* a = (MerryAcceptor*)malloc(sizeof(MerryAcceptor));
 if (!a)
    return MRES_SYS_FAILURE;
 
 a->shared_lock = shared_lock;
 a->queue = queue;
 a->listening_socket = sock;
 a->accepted_connections = 0;
 a->connection_limit = 0;
 a->interval = 100; // default 
 a->STOP = mfalse;
 *acceptor = a;
 return MRES_SUCCESS;
}

void merry_acceptor_destroy(MerryAcceptor *acceptor) {
  free(acceptor);
}

_THRET_T_ merry_acceptor_run(mptr_t a);
    
