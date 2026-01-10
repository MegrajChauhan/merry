#ifndef _MERRY_CORE_METADATA_
#define _MERRY_CORE_METADATA_

#include <merry_types.h>
#include <merry_core_types.h>

typedef struct MerryCoreMetadata MerryCoreMetadata;
typedef struct MerryCoreIdentity MerryCoreIdentity;

struct MerryCoreIdentity {
  mid_t id;
  muid_t uid;
  mguid_t gid;
};

struct MerryCoreMetadata {
  MerryCoreIdentity iden;
  mcore_t type;
};

#endif
