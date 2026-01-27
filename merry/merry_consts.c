#include <merry_consts.h>

_MERRY_ALWAYS_INLINE_ void
merry_HELP_msg() {
  printf("%s", HELP_MSG);
}

mresult_t merry_parse_arg(int argc, char **argv) {
  // ..... parsing
  consts.argc = argc;
  consts.argv = argv;
  consts.inp_file_index = -1;
  consts.prog_args = NULL;
  consts.prog_args_count = 0;
  consts.group_count_limit = 0;
  consts.core_count_limit = 0;
  consts.graves_config.core_count_lim = mfalse;
  consts.graves_config.group_count_lim = mfalse;
  int st = 1;
  while (st < argc) {
    switch (argv[st][0]) {
    case '-': {
      msize_t len = strlen(argv[st]);
      if (len < 2) {
        printf("Invalid option '%s'", argv[st]);
        merry_HELP_msg();
        return MRES_FAILURE;
      }
      switch (argv[st][1]) {
      case '-': {
        if (strcmp(argv[st], "--help") == 0) {
          merry_HELP_msg();
        } else if (strcmp(argv[st], "--version") == 0) {
          // version
        } else if (strcmp(argv[st], "--f") == 0) {
          // file provided
          if ((st + 1) >= argc) {
            printf("Expected file path after --f but got nothing",
                   NULL);
            merry_HELP_msg();
            return MRES_FAILURE;
          }
          // everything else is program arguments
          consts.inp_file_index = st + 1;
          consts.prog_args = &argv[st + 1];
          consts.prog_args_count = argc - (st + 1);
          return MRES_SUCCESS;
        } else {
          printf("Unknown option %s", argv[st]);
          merry_HELP_msg();
          return MRES_FAILURE;
        }
        break;
      }
      case 'G': {
        mbool_t used_nxt = mfalse;
        if (merry_parse_cmd_option_Graves(
                argv[st], (((st + 1) < argc) ? argv[st + 1] : NULL),
                &used_nxt) == MRES_FAILURE) {
          merry_HELP_msg();
          return MRES_FAILURE;
        }
        if (used_nxt == mtrue)
          st++;
        break;
      }
      default:
        printf("Unknown option '%s'", argv[st]);
        merry_HELP_msg();
        return MRES_FAILURE;
      }
      break;
    }
    default:
      printf("Unknown option '%s'", argv[st]);
      merry_HELP_msg();
      return MRES_FAILURE;
    }
    st++;
  }
  printf( "No Input File Provided!!!", NULL);
  merry_HELP_msg();
  return MRES_FAILURE;
}

mresult_t merry_parse_cmd_option_Graves(mstr_t opt, const mstr_t nxt,
                                        mbool_t *used_nxt) {
  switch (opt[2]) {
  case 'l': {
    if (strcmp(opt, "-Glgrpclim") == 0) {
      // nxt must be a value
      if (!nxt) {
        printf("Expected a value after option %s", opt);
        return MRES_FAILURE;
      }
      msize_t val = strtoull(nxt, NULL, 10);
      if (val == 0) {
        printf("Invalid value %s provided for option %s", nxt, opt);
        return MRES_FAILURE;
      }
      *used_nxt = mtrue;
      consts.group_count_limit = val;
      consts.graves_config.group_count_lim = mtrue;
    } else if (strcmp(opt, "-Glcclim") == 0) {
      // nxt must be a value
      if (!nxt) {
        printf("Expected a value after option %s", opt);
        return MRES_FAILURE;
      }
      msize_t val = strtoull(nxt, NULL, 10);
      if (val == 0) {
        printf("Invalid value %s provided for option %s", nxt, opt);
        return MRES_FAILURE;
      }
      *used_nxt = mtrue;
      consts.core_count_limit = val;
      consts.graves_config.core_count_lim = mtrue;
    } else {
      printf("Unknown value option %s", opt);
      return MRES_FAILURE;
    }
    break;
  }
  default:
    printf("Unknown option type %s", opt);
    return MRES_FAILURE;
  }
  return MRES_SUCCESS;
}

_MERRY_ALWAYS_INLINE_ MerryConsts *CONSTS() { return &consts; }
