#include <merry_consts.h>

mret_t merry_parse_arg(int argc, char **argv) {
  // ..... parsing
  // mvm -f [file name] -> for now
  consts.inp_file_index = 2;
  consts.argc = argc;
  consts.argv = argv;
  return RET_SUCCESS;
}

MerryConsts *CONSTS() { return &consts; }
