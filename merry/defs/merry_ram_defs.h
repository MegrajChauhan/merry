#ifndef _MERRY_RAM_DEFS_
#define _MERRY_RAM_DEFS_

typedef enum mram_t mram_t;
typedef enum mramstate_t mramstate_t;

enum mram_t {
  PUBLIC_RAM,
  PRIVATE_RAM,
  SHARED_RAM,
};

enum mramstate_t {
  DEAD,
  ALIVE,
};

#endif
