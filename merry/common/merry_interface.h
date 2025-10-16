#ifndef _MERRY_INTERFACE_
#define _MERRY_INTERFACE_

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
      mbool_t file_opened;
      msqword_t res;
    } file;
    struct {
      union {
        mfd_t pfd[2]; // the file descriptors
        struct {
          mdataline_t _read_fd;
          mdataline_t _write_fd;
        };
      };
      mbool_t _in_use;
      mbool_t _rclosed;
      mbool_t _wclosed;
    } cpipe; // communication pipe
  };
};

MerryInterface *merry_interface_init(minterface_t type);

void merry_interface_destroy(MerryInterface *interface);

#endif
