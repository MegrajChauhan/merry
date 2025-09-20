#ifndef _MERRY_INTERFACE_
#define _MERRY_INTERFACE_

#include <merry_error_stack.h>
#include <merry_interface_defs.h>
#include <merry_platform.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdlib.h>

typedef struct MerryInterface MerryInterface;

struct MerryInterface {
  minterface_t interface_t;
  union {
    struct {
      mfd_t fd;
      mbool_t blocking; // is the file descriptor blocking?
    } file;
  };
};

MerryInterface *merry_interface_init(minterface_t type, MerryErrorStack *st);

void merry_interface_destroy(MerryInterface *interface);

#endif
