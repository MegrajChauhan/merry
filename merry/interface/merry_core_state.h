#ifndef _MERRY_CORE_STATE_
#define _MERRY_CORE_STATE_

#include <merry_platform.h>
#include <merry_types.h>

typedef struct MerryCoreState MerryCoreState;

struct MerryCoreState {
  // Flags that Graves will use to send information
  atm_mbool_t interrupt; // interrupt the core
  atm_mbool_t stop;

  msize_t core_errno; // The errno for each core
};

#endif
