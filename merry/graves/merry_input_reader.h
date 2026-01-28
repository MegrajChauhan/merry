#ifndef _MERRY_INP_READER_
#define _MERRY_INP_READER_

#include <merry_file.h>
#include <merry_helpers.h>
#include <merry_logger.h>
#include <merry_mapped_file.h>
#include <merry_results.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <merry_core_defs.h>
#include <stdlib.h>

/*
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

typedef struct MerryInput MerryInput;

struct MerryInput {
  MerryFile *input_file;
  MerryMappedFile *mapped;
  mbptr_t instructions;
  msize_t instruction_len;
  mbptr_t data;
  msize_t data_len;
};

MerryInput *merry_input_init();

mresult_t merry_input_read(MerryInput *inp, mstr_t path);

void merry_input_destroy(MerryInput *inp);

#endif
