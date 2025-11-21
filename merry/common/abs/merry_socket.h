#ifndef _MERRY_SOCKET_
#define _MERRY_SOCKET_

#include <merry_operations.h>
#include <merry_platform.h>

// All connections we make are local so we only care about port
mresult_t merry_open_socket_conn(msocket_t *sock, msize_t port);

mresult_t merry_socket_accept_conn(msocket_t *sock, msocket_t *client);

mresult_t merry_socket_recv(msocket_t *sock, mbptr_t buf, msize_t len);

mresult_t merry_socket_send(msocket_t *sock, mbptr_t msg, msize_t len);

#endif
