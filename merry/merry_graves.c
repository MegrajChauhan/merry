#include <merry_graves.h>

void Merry_Graves_Run(int argc, char **argv) {
  if (merry_parse_arg(argc, argv) == RET_FAILURE) {
    merry_err("[Failed to parse argument]", NULL);
    exit(-1);
  }
  GRAVES._config = CONSTS();

  MerryErrorStack init_st;

  // Now we perform pre-initialization
  if (merry_graves_pre_init(&init_st) == RET_FAILURE) {
    ERROR(&init_st);
    exit(-1);
  }

  // We can now read the input file
  if (merry_graves_parse_input(&init_st) == RET_FAILURE) {
    ERROR(&init_st);
    exit(-1);
  }

  // Exit
  exit(GRAVES.return_value);
}

mret_t merry_graves_pre_init(MerryErrorStack *st) {
  if (merry_initialize_memory_interface(st) == RET_FAILURE) {
    PUSH(st, NULL, "Pre-initialization failed",
         "Performing Pre-initialization");
    return RET_FAILURE;
  }
  /*
   * If we ever have config files, this is the ideal point to
   * parse them
   * */
  return RET_SUCCESS;
}

mret_t merry_graves_parse_input(MerryErrorStack *st) {
  GRAVES.input = merry_graves_initialize_reader(
      GRAVES._config->argv[GRAVES._config->inp_file_index], st);
  if (!GRAVES.input) {
    PUSH(st, NULL, "Failed to initialize reader", "Initializing Graves");
    return RET_FAILURE;
  }

  if (merry_graves_reader_read_input(GRAVES.input, st) == RET_FAILURE) {
    PUSH(st, NULL, "Failed to read Input", "Initializing Graves");
    return RET_FAILURE;
  }

  return RET_SUCCESS;
}
