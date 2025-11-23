#ifndef _OWNER_PLATFORM_
#define _OWNER_PLATFORM_

#include <merry_config.h>

#ifdef _USE_LINUX_
#define _GNU_SOURCE
#include <arpa/inet.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/epoll.h>
#include <unistd.h>

typedef pthread_t thread_t;
typedef pthread_mutex_t mutex_t;
typedef pthread_cond_t cond_t;
typedef int fd_t;
typedef int socket_t;

#define CLOSESOCKET close

#elif defined(_USE_WIN_)
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <bcrypt.h>

#pragma comment(lib, "ws2_32.lib")

typedef HANDLE thread_t;
typedef CRITICAL_SECTION mutex_t;
typedef CONDITION_VARIABLE cond_t;
typedef HANDLE fd_t;
typedef SOCKET socket_t;

#define CLOSESCLOSESOCKET closesocket

#endif

#endif
