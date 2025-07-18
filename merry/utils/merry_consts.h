#ifndef _MERRY_CONSTS_
#define _MERRY_CONSTS_

// This here defines all the constants that the virtual machine uses
// The ones in the config and utils are different category of contants

#include <ctype.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdlib.h>
#include <string.h>

typedef struct MerryConsts MerryConsts;

/*Configurable constants*/
struct MerryConsts {
  msize_t stack_len; // The number of pages for a stack(not the number of bytes)
  // ....
  int *program_args;
  int inp_file_index;
};

#define _MERRY_HELP_MSG_                                                       \
  "Usage: mvm [Options] [Path to Input File]...\n"                             \
  "Options:\n"                                                                 \
  "-f, --file             --> Provide Path to Input File\n"                    \
  "-h, --help             --> Display this help message\n"                     \
  "-v, --version          --> Display Current Version\n"

mret_t merry_parse_arg(int argc, char **argv, MerryConsts *consts);

#endif
