#ifndef _MERRY_REQUESTS_
#define _MERRY_REQUESTS_

/*
 * Request generally refers to just:
 * 1) Programs request to graves that it wants to be fulfilled
 * 2) Core requests that reports something to graves
 *
 * It is good to note that the approach is not perfect and has many
 * flaws. When a lot of requests are queued, each core has to wait for
 * a while to see its request fufilled which isn't ideal.
 *
 * This is why I have made a change in this attempt:
 * 1) Not all requests are fulfilled by Graves.
 * 2) Cores will fulfill what they can themselves
 *
 * Graves will fulfill only:
 * 1) Direct syscall requests under controlled environment.
 * 2) Direct core requests suggesting error
 * 3) Core requests to interact with other cores
 * 4) Core requests to modify its memory or share memory
 * 5) And more if necessary
 *
 * Previously, while Graves handled everything such as file IO, file
 * handling and so on, that is now going to be passed to the cores.
 *
 * */

#include <merry_core_types.h>
#include <merry_interface_defs.h>
#include <merry_protectors.h>
#include <merry_types.h>

typedef enum mgreq_t mgreq_t;

enum mgreq_t {
  NOP,                // do nothing(needed)
  CREATE_CORE,        // Creating a new core
                      // DESC: Create a new core to execute some more code
                      //       in parallel
  CREATE_GROUP,       // Create a new group
                      // DESC: Creates a new empty group
  GET_GROUP_DETAILS,  // DESC: Get the details of one specific group
  GET_SYSTEM_DETAILS, // DESC: Get the details of the system
};

typedef struct MerryGravesRequest MerryGravesRequest;
typedef union MerryRequestArgs MerryRequestArgs;
typedef struct MerryGravesRequestOperationResult
    MerryGravesRequestOperationResult;

// InterCore Result
typedef struct MerryICRes MerryICRes;
// The source of the IC result
typedef enum mICResSource_t mICResSource_t;

enum mICResSource_t {
  IC_SOURCE_INTERFACE,
  IC_SOURCE_MERRY,
  IC_SOURCE_CORE,
};

struct MerryICRes {
  mICResSource_t source;
  msize_t ERRNO;
  union {
    minterfaceRet_t _interface_ret;
    mresult_t _merry_ret;
    msize_t _core_code; // convention-based
  };
};

struct MerryCoreBase;

struct MerryGravesRequestOperationResult {
  mresult_t result;
  msize_t ERRNO;
  MerryICRes ic_res;
};

struct MerryGravesRequest {
  mgreq_t type;
  _Atomic mbool_t fufilled;
  struct MerryCoreBase *base;
  mcond_t *used_cond;
  MerryRequestArgs *args;
  MerryGravesRequestOperationResult result;
};

/*
 * Each core has it's own convention for argument passing
 * but will use this to convey the arguments to Graves
 * */
union MerryRequestArgs {
  struct {
    mcore_t new_core_type;
    maddress_t st_addr;
    mbool_t same_group; // If this is set, gid and new_group are ignored else
    mbool_t new_group;  // If set and !same_group then a new group is created by
    // ignoring gid
    mguid_t gid; // if !same_group and !new_group use this to find a group
    // Results
    mid_t new_id;   // the id of the new core
    muid_t new_uid; // the uid of the new core
                    // The guid will be passed via gid
  } create_core;

  struct {
    mguid_t new_guid; // result
  } create_group;

  struct {
    mguid_t guid; // what group you want
    // result
    msize_t core_count;
    msize_t active_core_count;
  } get_group_details;

  struct {
    msize_t grp_count;
    msize_t active_core_count;
    msize_t core_count;
  } get_system_details;
};

#endif
