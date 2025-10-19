#ifndef _MERRY_INTERFACE_DEFS_
#define _MERRY_INTERFACE_DEFS_

#define _EBADINTERFACE -1

typedef enum minterface_t minterface_t;
typedef enum minterfaceRet_t minterfaceRet_t;

enum minterface_t {
  INTERFACE_TYPE_FILE,
  INTERFACE_TYPE_PIPE,
};

enum minterfaceRet_t {
  INTERFACE_FAILURE,
  INTERFACE_SUCCESS,
  INTERFACE_TYPE_INVALID,
  INTERFACE_HOST_FAILURE,
  INTERFACE_MISCONFIGURED,
  INTERFACE_INVALID_ARGS,
  INTERFACE_INVALID_STATE,
};
#endif
