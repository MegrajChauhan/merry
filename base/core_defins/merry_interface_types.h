#ifndef _MERRY_INTERFACE_DEFS_
#define _MERRY_INTERFACE_DEFS_

#define _EBADINTERFACE -1

typedef enum minterface_t minterface_t;

enum minterface_t {
  INTERFACE_TYPE_FILE,
  INTERFACE_TYPE_PIPE,
  INTERFACE_TYPE_MAPPED_MEMORY,
  INTERFACE_TYPE_MAPPED_FILE,
};

#endif
