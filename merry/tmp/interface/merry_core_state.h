#ifndef _MERRY_CORE_STATE_
#define _MERRY_CORE_STATE_

#include <merry_platform.h>
#include <merry_types.h>

typedef struct MerryCoreState MerryCoreState;

struct MerryCoreState {
  // The state of the core
  atm_mbool_t running;
};

#endif
