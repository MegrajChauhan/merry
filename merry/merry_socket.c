#include <merry_socket.h>
#include <netinet/in.h>
#include <sys/socket.h>

// All connections we make are local so we only care about port
mresult_t merry_open_socket_conn(msocket_t *sock, msize_t port) {
  if (!sock)
    return MRES_INVALID_ARGS;

  *sock = socket(AF_INET, SOCK_STREAM, 0);
  if (*sock < 0)
    return MRES_SYS_FAILURE;

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (bind(*sock, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
    CLOSESOCKET(*sock);
    return MRES_SYS_FAILURE;
  }
  return MRES_SUCCESS;
}

mresult_t merry_socket_accept_conn(msocket_t *sock, msocket_t *client) {
  if (!sock || !client)
    return MRES_INVALID_ARGS;

  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  *client = accept(*sock, (struct sockaddr *)&addr, &len);
  if (*client < 0)
    return MRES_SYS_FAILURE;

  return MRES_SUCCESS;
}

mresult_t merry_socket_recv(msocket_t *sock, mbptr_t buf, msize_t len) {
  if (!sock || !buf || (len == 0))
    return MRES_INVALID_ARGS;

  if (recv(*sock, (mptr_t)buf, len, 0) < 0)
    return MRES_SYS_FAILURE;

  return MRES_SUCCESS;
}

mresult_t merry_socket_send(msocket_t *sock, mbptr_t msg, msize_t len) {
  if (!sock || !msg || (len == 0))
    return MRES_INVALID_ARGS;

  if (send(*sock, (mptr_t)msg, len, 0) < 0)
    return MRES_SYS_FAILURE;

  return MRES_SUCCESS;
}
