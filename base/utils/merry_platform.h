#ifndef _MERRY_PLATFORM_
#define _MERRY_PLATFORM_

#include <merry_config.h>

#ifdef _USE_LINUX_
#include <dlfcn.h>
#include <fcntl.h>
#include <liburing.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef pthread_t mthread_t;
typedef pthread_mutex_t mmutex_t;
typedef pthread_cond_t mcond_t;
typedef int mdataline_t;
typedef int mfd_t;
typedef void* mmap_t;

#elif defined(_USE_WIN_)
#include <windows.h>

typedef HANDLE mthread_t;
typedef CRITICAL_SECTION mmutex_t;
typedef CONDITION_VARIABLE mcond_t;
typedef HANDLE mdataline_t;
typedef HANDLE mfd_t;

#endif

#endif
