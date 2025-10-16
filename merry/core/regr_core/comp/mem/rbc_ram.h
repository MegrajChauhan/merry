#ifndef _RBC_RAM_
#define _RBC_RAM_

#include <merry_config.h>
#include <merry_list.h>
#include <merry_logger.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <regr_core/def/consts/rbc_mem_defs.h>
#include <regr_core/def/declr/rbc_mem_declr.h>
#include <stdlib.h>

_MERRY_DECLARE_STATIC_LIST_(RBCMemPage, RBCMemPage);

typedef struct RBCMemory RBCMemory;

struct RBCMemory {
  MerryRBCMemPageList *pg_list;
  maddress_t max_address;
};

// Initialize a memory(Just Initialization. Pages not populated)
RBCMemory *rbc_memory_init(msize_t pg_count);

mret_t rbc_memory_populate(RBCMemory *mem, msize_t pg_num, mbptr_t addr_space);

void rbc_memory_destroy(RBCMemory *mem);

#endif
