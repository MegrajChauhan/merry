#ifndef _MERRY_PLATFORM_
#define _MERRY_PLATFORM_

#include <merry_config.h>
#include <merry_operations.h>
#include <merry_types.h>
#include <merry_utils.h>

#ifdef _USE_LINUX_
#define _GNU_SOURCE
#include <arpa/inet.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <liburing.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/random.h>

typedef pthread_t mthread_t;
typedef pthread_mutex_t mmutex_t;
typedef pthread_cond_t mcond_t;
typedef int mdataline_t;
typedef int mfd_t;
typedef pid_t mpid_t;
typedef int msocket_t;

#define CLOSESOCKET close

#elif defined(_USE_WIN_)
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <bcrypt.h>

#pragma comment(lib, "ws2_32.lib")

typedef HANDLE mthread_t;
typedef CRITICAL_SECTION mmutex_t;
typedef CONDITION_VARIABLE mcond_t;
typedef HANDLE mdataline_t;
typedef HANDLE mfd_t;
typedef SOCKET msocket_t;

#define CLOSESCLOSESOCKET closesocket

// For Pipes
// #define _MERRY_INHERIT_IN_FD_ 0
// #define _MERRY_INHERIT_OUT_FD_ 1

#endif

// typedef mptr_t mdlentry_t; // same for windows and linux

// mresult_t merry_open_pipe(mdataline_t *rline, mdataline_t *wline);

mbool_t merry_is_path_a_directory(mstr_t path);

mbool_t merry_non_blocking_socket(msocket_t *sock);

mbool_t merry_get_random_number(mqptr_t res);

mresult_t merry_lock_file(mfd_t fd);

#endif
