#ifndef _TEST_CORE_MEM_
#define _TEST_CORE_MEM_

#include <merry_config.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdlib.h>
#include <test_core/defs/test_core_consts.h>

/*
 * The memory of test core is just an array of bytes
 * No pages and all
 * There will be basic error handling too
 * */

typedef struct TCMem TCMem;

struct TCMem {
  mbptr_t all_instructions; /* We will actually map the input file into the
                              memory we get and let the operating system handle
                              lazy loading for us */
  msize_t len;              // number of bytes
};

TCMem *tc_mem_init(mbptr_t inst, msize_t len);

void tc_mem_destroy(TCMem *mem);

tcret_t tc_read(TCMem *mem, maddress_t addr, mbptr_t store_in);

// No need to write(No data memory)

#endif
