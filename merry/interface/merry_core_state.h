#ifndef _MERRY_CORE_STATE_
#define _MERRY_CORE_STATE_

#include <merry_types.h>
#include <merry_platform.h>

typedef struct MerryCoreState MerryCoreState;

struct MerryCoreState {
	atm_mbool_t running; // Set to mfalse iff the core has terminated
	atm_mbool_t stopped; // set to mtrue iff the core was paused because of itself
	atm_mbool_t busy;    // set to mtrue iff the core is busy with something and cannot accept requests(interrupts will be sent nonetheless)
	 
	// Flags that Graves will use to send information
	atm_mbool_t interrupt; // interrupt the core
	
	msize_t core_errno;    // The errno for each core
};

#endif
