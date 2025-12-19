#ifndef _MERRY_INTERFACE_
#define _MERRY_INTERFACE_

#include <merry_results.h>
#include <merry_platform.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <merry_interface_types.h>
#include <stdlib.h>

typedef struct MerryInterface MerryInterface;

struct MerryInterface {
  minterface_t interface_t;
  union {
    struct {
      mfd_t fd;
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
      mmap_t map;
      msize_t len;
    } memory;
    struct {
    	MerryInterface *file;
    	mmap_t map;
    	msize_t len; // length of file
    	struct {
    		mbyte_t update_backing_file:1; // if set, changes to the map will be reflected on the backing file
    		mbyte_t resb: 7;
    	} flags;
    } mem_mapped_file;
  };
};

mresult_t merry_interface_init(MerryInterface **interface, minterface_t type);

void merry_interface_destroy(MerryInterface *interface);

#endif
