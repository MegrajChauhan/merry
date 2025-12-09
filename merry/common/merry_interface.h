#ifndef _MERRY_INTERFACE_
#define _MERRY_INTERFACE_

#include <merry_operations.h>
#include <merry_platform.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <merry_interface_defs.h>
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
	  atm_msize_t refr_count;
      struct {
      	mbyte_t share : 1; // is this map sharable with other cores?
      	mbyte_t resb : 7;      	
      } flags;
    } memory;
    struct {
    	MerryInterface *file;
    	
    } mem_mapped_file;
  };
};

mresult_t merry_interface_init(MerryInterface **interface, minterface_t type);

void merry_interface_destroy(MerryInterface *interface);

#endif
