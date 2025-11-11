#ifndef _USE_DEFS_
#define _USE_DEFS_

#include <merry_core_types.h>
#include <merry_graves_defs.h>
#include <merry_interface_defs.h>
#include <merry_operations.h>
#include <merry_platform.h>
#include <merry_requests.h>

#define CORE_TYPE_COUNT __CORE_TYPE_COUNT

typedef mcore_t core_t;
typedef minterface_t interface_t;
typedef minterfaceRet_t interfaceRet_t;
typedef mgreq_t greq_t;
typedef mICResSource_t ICResSource_t;
typedef mresult_t result_t;

#undef REQ

typedef MerryGravesRequest GravesRequest;
typedef MerryGravesRequestOperationResult GravesRequestOperationResult;
typedef struct MerryCoreBase CoreBase;
typedef union MerryRequestArgs RequestArgs;
typedef struct MerryICRes ICRes;

typedef mthread_t thread_t;
typedef mmutex_t mutex_t;
typedef mcond_t cond_t;
typedef mdataline_t dataline_t;
typedef mfd_t fd_t;

typedef mthexec_t thexec_t;

#endif
