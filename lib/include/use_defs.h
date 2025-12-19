#ifndef _USE_DEFS_
#define _USE_DEFS_

#include <utils/merry_platform.h>

enum minterface_t;
enum mresult_t;

typedef enum minterface_t interface_t;
typedef enum mresult_t result_t;

typedef mthread_t thread_t;
typedef mmutex_t mutex_t;
typedef mcond_t cond_t;
typedef mdataline_t dataline_t;
typedef mfd_t fd_t;

#endif
