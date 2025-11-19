#ifndef _MERRY_CORE_
#define _MERRY_CORE_

#include <merry_types.h>
#include <merry_utils.h>
#include <merry_platform.h>

typedef struct MerryCore MerryCore;

// Represents a core
struct MerryCore {
	msocket_t comms;           // for communication
	mbool_t connection_active; // Check if the connection is active or not
	mbool_t remote_connection; // if the core is running on the same host as Merry
	mbool_t merry_spawned;     // Was the core spawned by Merry?
	mpid_t process_id;         // The process ID of the core
	mqword_t verification;     // For verification
	mbool_t verified;          
};

#endif
