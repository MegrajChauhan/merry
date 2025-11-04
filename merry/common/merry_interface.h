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
      msqword_t res;
      struct {
        mbyte_t file_opened : 1;
        mbyte_t read : 1;
        mbyte_t write : 1;
        mbyte_t append : 1;
        mbyte_t resb : 4;
      } flags;
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
      mbool_t resb;
    } cpipe; // communication pipe
    struct {
      // For now, we don't care about the type of mapping
      // and let the OS care for us
      mptr_t map;
      msize_t len;
      _Atomic msize_t children_count;
      MerryInterface *parent;
    } memory_map;
  };
};

MerryInterface *merry_interface_init(minterface_t type);

void merry_interface_destroy(MerryInterface *interface);

#endif
