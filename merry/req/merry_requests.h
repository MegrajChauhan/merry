#ifndef _MERRY_REQUESTS_
#define _MERRY_REQUESTS_

#include <merry_platform.h>
#include <merry_request_types.h>
#include <merry_results.h>
#include <merry_types.h>

typedef struct MerryGravesRequest MerryGravesRequest;
typedef union MerryRequestArgs MerryRequestArgs;

union MerryRequestArgs {
  struct {
  	msize_t ret;
  } stop_core;
  struct {
    mcore_t new_core_type;
    maddress_t st_addr;
    mguid_t gid; // The group that the core should belong to
    // Results
    mid_t new_id;   // the id of the new core
    muid_t new_uid; // the uid of the new core
  } create_core;

  struct {
    mguid_t new_guid; // result
  } create_group;
};

struct MerryGravesRequest {
  mgreq_t type;
  atm_mbool_t fufilled;
  mcond_t *used_cond;
  MerryRequestArgs args;
  mresult_t result;
  msize_t err;
};

#endif
