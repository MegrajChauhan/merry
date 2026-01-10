#ifndef _MERRY_REQUEST_TYPES_
#define _MERRY_REQUEST_TYPES_

typedef enum mgreq_t mgreq_t;

enum mgreq_t {
  NOP,                // do nothing(needed)
  CORE_STOP,          // The core is terminating
  CREATE_CORE,        // Creating a new core
                      // DESC: Create a new core to execute some more code
                      //       in parallel
  CREATE_GROUP,       // Create a new group
                      // DESC: Creates a new empty group
  GET_SYSTEM_DETAILS, // DESC: Get the details of the system
};

#endif
