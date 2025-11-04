#ifndef _RBC_INP_READER_
#define _RBC_INP_READER_

#include <merry_file.h>
#include <merry_helpers.h>
#include <merry_logger.h>
#include <merry_memory.h>
#include <merry_mmem.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <regr_core/def/consts/rbc_mem_defs.h>
#include <regr_core/internal/rbc_utils.h>
#include <stdlib.h>

/*
 * RBC will use a similar file format to MVM
 * The format will look something like this:
 * <IDENTIFICATION[Magic Numbers]> <ENDIANNESS OF THE INSTRUCTIONS AND DATA> ->
 * Identification Header[We will have a few bytes left to be used for something
 * else if it comes to that] <Instruction Section Length> The length of the
 * instruction section <Data Section Length> The number of bytes that the Data
 * Section has <Debugging Information Table Length> -> A custom section
 *
 * <Instruction Section>
 * <Data Section>
 * <Debugging Information>: [None of RBC's business]
 *
 * A detailed explanation for each of the sections is as follow:
 * Data Section: Contains all of the data needed by the core.
 * DI: This section is a sort of "do-what-you-will" section.
 * As Merry is planned to provide a debugging framework which may be utilized by
 * people to write debuggers. For this case,
 * people may come up with conventions as to how this section should be
 * structured. Based on those standards and conventions, assemblers and
 * compilers may produce the necessary information. Thus, this is a customizable
 * section.
 *
 * The header and the lengths will all be in big endian format. The
 * instructions and data will all follow the endianness of the host
 * where they were generated.
 *
 * The reader will provide the read instructions and data and it is upto the
 * memory to structure it as it sees fit
 * */

typedef struct RBCInput RBCInput;

struct RBCInput {
  MerryFile *input_file;
  MerryMMem *mapped;
  mbptr_t instructions;
  msize_t instruction_len;
  mbptr_t data;
  msize_t data_len;
};

RBCInput *rbc_input_init();

mret_t rbc_input_read(RBCInput *inp, mstr_t path);

void rbc_input_destroy(RBCInput *inp);

#endif
