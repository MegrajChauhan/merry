#ifndef _TEST_CORE_INP_
#define _TEST_CORE_INP_

#include <merry_config.h>
#include <merry_file.h>
#include <merry_logger.h>
#include <merry_memory.h>
#include <merry_operations.h>
#include <merry_types.h>
#include <test_core/comps/test_core_memory.h>
#include <test_core/defs/test_core_consts.h>

/*
 * The format for test core is:
 * just bytes as is.
 * There is no format really.
 * Since test core doesn't have any need for data memory, it
 * doesn't need any format.
 * Just plain Instructions
 * */

// Maps the file and returns the pointer
// It would be a good question:
// "Will using the bare pointer not prove to be a hazard?"
// Answer: absolutely! but no program(not even Graves) has the right
// to access the bare pointer or modify it unless it is being done
// in a controlled way

typedef struct TCInp TCInp;

struct TCInp {
  mbptr_t mem;
  MerryFile *file;
};

tcret_t tc_read_input(mstr_t fname, TCInp *inp);

void tc_destroy_input(TCInp *inp);

#endif
