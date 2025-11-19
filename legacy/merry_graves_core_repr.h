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
};

#endif
