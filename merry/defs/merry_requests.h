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
#include <merry_types.h>

typedef enum mgreq_t mgreq_t;

enum mgreq_t {
  KILL_SELF,          // A core killing itself
                      // DESC: If a core has done what it can and wants to
                      //       terminate itself gracefully
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

struct MerryGravesRequest {
  mgreq_t type;
  mid_t id;
  muid_t uid;
  mguid_t guid;
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
    mbool_t share_resources; // If the requesting core wants to share some
                             // resources to the new core In this case, sharing
                             // resources implies if the new core, if same type
                             // as the requester, should re-parse the input file
                             // and setup everything or if it can share what the
                             // requester is using. Sharing other resources will
                             // prove to be hard specially if both cores are of
                             // different types. Hence until we come
                             // up with something to share resources,
                             // this is it
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
