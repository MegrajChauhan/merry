#ifndef _MERRY_GRAVES_REQUEST_
#define _MERRY_GRAVES_REQUEST_

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

struct MerryCoreBase;

struct MerryGravesRequest {
  mgreq_t type;
  atm_mbool_t fufilled;
  mptr_t repr;
  mcond_t *used_cond;
  MerryRequestArgs *args;
  mresult_t result;
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
  } get_group_details;

  struct {
    msize_t grp_count;
    msize_t active_core_count;
    msize_t core_count;
  } get_system_details;
};

#endif
