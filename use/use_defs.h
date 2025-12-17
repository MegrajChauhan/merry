#ifndef _USE_DEFS_
#define _USE_DEFS_

#include <merry_platform.h>

enum mcore_t;
enum minterface_t;
enum mgreq_t;
enum mresult_t;

struct MerryGravesRequest;
struct MerryCoreBase;
struct MerryRequestArgs;

#define CORE_TYPE_COUNT __CORE_TYPE_COUNT

typedef enum mcore_t core_t;
typedef enum minterface_t interface_t;
typedef enum mgreq_t greq_t;
typedef enum mresult_t result_t;

#undef REQ

typedef struct MerryGravesRequest GravesRequest;
typedef struct MerryCoreBase CoreBase;
typedef union MerryRequestArgs RequestArgs;

typedef mthread_t thread_t;
typedef mmutex_t mutex_t;
typedef mcond_t cond_t;
typedef mdataline_t dataline_t;
typedef mfd_t fd_t;

#endif
