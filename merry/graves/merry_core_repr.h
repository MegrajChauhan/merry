#ifndef _MERRY_CORE_REPR_
#define _MERRY_CORE_REPR_

/**
 * A core can be represented as having:
 * It's state(Maintained by Graves)
 * It's execution backend
 * It's properties or permissions (Maintained by Graves)
 */

#include <merry_core.h>
#include <merry_types.h>
#include <merry_utils.h>

typedef struct MerryCoreState MerryCoreState;
typedef struct MerryCoreProperties MerryCoreProperties;
typedef struct MerryCoreIdentity MerryCoreIdentity;
typedef struct MerryCoreRepr MerryCoreRepr;

struct MerryCoreState {
  atm_mbool_t running;
  atm_mbool_t busy;
};

struct MerryCoreProperties {
  atm_mbool_t tmp; // Yet to be defined
};

struct MerryCoreIdentity {
  mid_t id;
  muid_t uid;
  mguid_t gid;
};

struct MerryCoreRepr {
  MerryCoreState state;
  MerryCoreProperties props;
  MerryCoreIdentity iden;
  MerryCore *core;
};

#endif
