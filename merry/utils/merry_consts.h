#ifndef _MERRY_CONSTS_
#define _MERRY_CONSTS_

// This here defines all the constants that the virtual machine uses
// The ones in the config and utils are different category of contants

#include <ctype.h>
#include <merry_core_interface.h>
#include <merry_helpers.h>
#include <merry_results.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdlib.h>
#include <string.h>

typedef struct MerryConsts MerryConsts;

/*Configurable constants*/
struct MerryConsts {
  /*
   * Configure Graves
   * Options are prefixed with -G or --G
   * */
  struct {
    bit_group(group_count_lim, 1); // -Glgrpclim [value]
    bit_group(core_count_lim, 1);  // -Glcclim [value]
    bit_group(res, 6);
  } graves_config;
  /*
   * If the user wants to limit core count to specific group only then it can be
   * done but if the user wants to set limit on individual core type then it
   * isn't allowed. Graves cannot keep track of the number of cores for each
   * type and so it is the user's job to ensure they have the desired number of
   * core of each type.
   * */
  /*
   * Limit the number of groups that can be created
   * */
  msize_t group_count_limit;
  /*
   * The limit to the number of active cores. If there is a dead core then it
   * will be used to spawn a new core but the number of active cores will not
   * rise above the limit ever.
   * */
  msize_t core_count_limit;

  int argc;
  char **argv;
  int inp_file_index; // -f for input file
  char **prog_args;   // arguments meant for the program(everything after the
                      // input file)
  msize_t prog_args_count;
};

_MERRY_INTERNAL_ MerryConsts consts;

_MERRY_INTERNAL_ mstr_t HELP_MSG =
    "Merry Nexus- Runtime and collaboration of systems\n"
    "version v(Not available right now)\n"
    "Usage:\n"
    "mvm [OPTIONS] INPUT ARGS...\n"
    "Options:\n"
    "General:\n"
    "    --help                   print this help message\n"
    "    --version                print the version information\n\n"
    "    --f [input file]args...  Provide Path to Input File\n"
    "Graves:\n"
    "    -Glgrpclim [value]       set limit to the number of groups allowed\n"
    "    -Glcclim   [value]       set limit to the number of active core "
    "allowed\n";

void merry_HELP_msg(MerryCoreInterface interfaces[__CORE_TYPE_COUNT]);

mresult_t merry_parse_arg(int argc, char **argv,
                          MerryCoreInterface interfaces[__CORE_TYPE_COUNT]);

MerryConsts *CONSTS();

/*
 * Individual option parsing
 * */
mresult_t merry_parse_cmd_option_Graves(mstr_t opt, const mstr_t nxt,
                                        mbool_t *used_nxt);

#endif
