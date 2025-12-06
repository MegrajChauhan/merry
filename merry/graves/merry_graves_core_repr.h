#ifndef _MERRY_GRAVES_CORE_REPR_
#define _MERRY_GRAVES_CORE_REPR_

// The merry_graves_core_base.h only describes the framework
// that Graves expects every core to adhere to

#include <merry_graves_core_base.h>
#include <stdlib.h>

typedef struct MerryGravesCoreRepr MerryGravesCoreRepr;

struct MerryGravesCoreRepr {
  MerryCoreBase *base;
  mptr_t core;
  atm_mbool_t dead;
  mid_t id;
  muid_t uid;
  mguid_t guid;
  mcore_t type;

  msqword_t core_creation_lim; // -1 for None
  msqword_t group_creation_lim; // -1 for None
  msqword_t sub_core_creation_lim; // -1 for None
};

#endif
