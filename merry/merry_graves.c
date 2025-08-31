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

  // Initialize Graves finally

  // Exit
  exit(GRAVES.return_value);

GRAVES_FAILED_TO_START:
  // Graves couldn't fully initialize
  merry_err("GRAVES: Couldn't start the machine...", NULL);
  merry_graves_destroy();
  exit(-1);
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

mret_t merry_graves_init(MerryErrorStack *st) {
  GRAVES.GRPS = merry_create_dynamic_list(1, sizeof(MerryGravesGroup *), st);
  if (!GRAVES.GRPS) {
    PUSH(st, NULL, "Failed to create GRPS list", "Initializing Graves");
    return RET_FAILURE;
  }

  if (merry_cond_init(&GRAVES.graves_cond) == RET_FAILURE) {
    PUSH(st, NULL, "Failed to initialize GRAVES COND", "Initializing Graves");
    return RET_FAILURE;
  }

  if (merry_mutex_init(&GRAVES.graves_lock) == RET_FAILURE) {
    PUSH(st, NULL, "Failed to initialize GRAVES LOCK", "Initializing Graves");
    return RET_FAILURE;
  }

  GRAVES.overall_core_count = 0;
  GRAVES.overall_active_core_count = 0;
  GRAVES.initial_data_mem_page_count = GRAVES.input->data_ram->page_count;
  GRAVES.return_value = 0;

  /*
   * More fields as added
   * */

  return RET_FAILURE;
}

void merry_graves_destroy(MerryErrorStack *st) {
  // Just basic cleanup
  if (GRAVES.GRPS)
    merry_destroy_dynamic_list(GRAVES.GRPS);
  if (GRAVES.input) {
  }
  merry_graves_reader_destroy(GRAVES.input, st);
}
