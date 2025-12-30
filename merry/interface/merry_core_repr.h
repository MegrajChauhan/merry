#ifndef _MERRY_CORE_REPR_
#define _MERRY_CORE_REPR_

#include <merry_core_metadata.h>
#include <merry_core_state.h>
#include <merry_core_types.h>
#include <merry_types.h>

typedef struct MerryCoreRepr MerryCoreRepr;

struct MerryCoreRepr {
  MerryCoreMetadata metadata;
  MerryCoreState state;
  mptr_t core;
  mcore_t type;
};

#endif
