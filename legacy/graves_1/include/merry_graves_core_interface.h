#ifndef _MERRY_GRAVES_CORE_INTERFACE_
#define _MERRY_GRAVES_CORE_INTERFACE_

#include <merry_core_interface.h>

typedef struct MerryGravesViewOfCore MerryGravesViewOfCore;
typedef struct MerryCoreIdentity MerryCoreIdentity;

struct MerryGravesViewOfCore {
	MerryCoreViewOfCore *core;
	mcore_t type;
	// More stuff
};

#endif
