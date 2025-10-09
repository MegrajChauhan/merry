#ifndef _MERRY_PLATFORM_
#define _MERRY_PLATFORM_

#include <merry_config.h>
#include <merry_types.h>
#include <merry_utils.h>

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

#elif defined(_USE_WIN_)
#include <windows.h>

typedef HANDLE mthread_t;
typedef CRITICAL_SECTION mmutex_t;
typedef CONDITION_VARIABLE mcond_t;
typedef HANDLE mdataline_t;
typedef HANDLE mfd_t;

// For Pipes
#define _MERRY_INHERIT_IN_FD_ 0
#define _MERRY_INHERIT_OUT_FD_ 1

#endif

typedef mptr_t mdlentry_t; // same for windows and linux

_MERRY_DEFINE_FUNC_PTR_(_THRET_T_, mthexec_t, mptr_t);

mret_t merry_open_pipe(mdataline_t *rline, mdataline_t *wline);

mbool_t merry_is_path_a_directory(mstr_t path);

#endif
