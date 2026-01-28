#include <merry_graves.h>

_MERRY_INTERNAL_ MerryGraves GRAVES;

_MERRY_DEFINE_STATIC_LIST_(Group, MerryGravesGroup *);

_MERRY_NO_RETURN_ void Merry_Graves_Run(int argc, char **argv) {
  // Now we perform pre-initialization
  MDBG("Running Merry", NULL);
  if (merry_graves_pre_init() != MRES_SUCCESS)
    goto GRAVES_FAILED_TO_START;

  if (merry_parse_arg(argc, argv) == RET_FAILURE) {
    MERR("Failed to parse input arguments", NULL);
    exit(-1);
  }
  GRAVES._config = CONSTS();

  // We can now read the input file
  if (merry_graves_parse_input() != MRES_SUCCESS)
    goto GRAVES_FAILED_TO_START;

  // Initialize Graves finally
  if (merry_graves_init() != MRES_SUCCESS)
    goto GRAVES_FAILED_TO_START;

  // Ready Everything
  if (merry_graves_ready_everything() != MRES_SUCCESS)
    goto GRAVES_FAILED_TO_START;

  // Finally Ready to RUN the VM
  MDBG("Successfully Initialized Merry", NULL);
  merry_graves_START();

  // Exit
  MDBG("Terminating", NULL);
  exit(0);

GRAVES_FAILED_TO_START:
  // Graves couldn't fully initialize
  MDBG("Failed to Initialize Merry", NULL);
  merry_graves_destroy();
  exit(-1);
}

mresult_t merry_graves_pre_init() {
  /*
   * If we ever have config files, this is the ideal point to
   * parse them
   * */
  MDBG("Getting Configurations...", NULL);
  if (GRAVES._config->graves_config.log_dbg)
    merry_init_logger(merry_log__, MERRY_LOG_DBG);
  else
    merry_init_logger(merry_log__, MERRY_LOG_ERR);
  return MRES_SUCCESS;
}

mresult_t merry_graves_parse_input() {
  MDBG("Parsing Input File", NULL);
  GRAVES.input = merry_input_init();
  if (!GRAVES.input) {
    MDBG("Failed to parse Input File", NULL);
  	return MRES_FAILURE;
  }
  MDBG("File: %s", GRAVES._config->argv[GRAVES._config->inp_file_index]);
  if (merry_input_read(GRAVES.input, GRAVES._config->argv[GRAVES._config->inp_file_index]) != MRES_SUCCESS) {
  	MDBG("Failed to parse Input File", NULL);
  	return MRES_FAILURE;
  }
  return MRES_SUCCESS;
}

mresult_t merry_graves_init() {
  mresult_t res;
  msize_t grp_count = 5;
  if (GRAVES._config->graves_config.group_count_lim)
    grp_count = GRAVES._config->group_count_limit;
  res = merry_Group_list_create(grp_count, &GRAVES.GRPS);
  if (!GRAVES.GRPS) {
    MERR("Failed to initialize MERRY: Failed to create GROUP List", NULL);
    return res;
  }

  if (merry_cond_init(&GRAVES.graves_cond) != MRES_SUCCESS) {
    MERR("Failed to initialize MERRY: Graves Initialization[Failed to obtain "
           "condition variable]",
           NULL);
    return MRES_FAILURE;
  }

  if (merry_mutex_init(&GRAVES.graves_lock) != MRES_SUCCESS) {
    MERR("Failed to initialize MERRY: Graves Initialization[Failed to obtain "
           "mutex lock]",
           NULL);
    return MRES_FAILURE;
  }

  GRAVES.current_req = NULL;
  GRAVES.core_count = 0;
  GRAVES.grp_count = 0;
  GRAVES.active_core_count = 0;
  /*
   * More fields as added
   * */

  return MRES_SUCCESS;
}

mresult_t merry_graves_ready_everything() {
  // Here we prepare everything before we boot up the first core

  // Create a new group[FIRST GROUP: 0]
  MerryGravesGroup *grp;
  mresult_t res = merry_graves_add_group(&grp);
  if (!(grp)) {
    MERR("Failed to create first GROUP", NULL);
    return res;
  }

  // Add the first core
  MerryCoreRepr *first_core;
  res = merry_graves_add_core(grp, &first_core);

  if (!first_core) {
    MERR("Failed to add first CORE", NULL);
    merry_graves_group_destroy(grp);
    return res;
  }

  // Initialize the first core
  if (merry_graves_init_a_core(first_core, 0x00) !=
      MRES_SUCCESS) {
    MERR("Failed to initialize the first core...", NULL);
    merry_graves_group_destroy(grp);
    return MRES_FAILURE;
  }
  merry_graves_give_IDs_to_cores(first_core);

  // Finally Initialize the Request Queue
  if (merry_graves_req_queue_init() == RET_FAILURE) {
    MERR("Failed to initialize REQUEST QUEUE HANDLER", NULL);
    merry_core_destroy(first_core->core);
    first_core->core = NULL;
    merry_graves_group_destroy(grp);
    return MRES_FAILURE;
  }
  merry_graves_req_register_wakeup(&GRAVES.graves_cond, &GRAVES.graves_lock);

  return MRES_SUCCESS;
}

void merry_graves_destroy() {
  // Just basic cleanup
  // The cleanup step is comprised of mutliple steps
  // This is the last step
  if (GRAVES.GRPS)
    merry_Group_list_destroy(GRAVES.GRPS);
  merry_cond_destroy(&GRAVES.graves_cond);
  merry_mutex_destroy(&GRAVES.graves_lock);
  merry_graves_req_queue_free();
}

void merry_graves_cleanup_groups() {
  // Before this function is called, it is necessary that
  // all of the cores have terminated execution
  for (msize_t i = 0; i < GRAVES.grp_count; i++) {
    merry_graves_group_destroy(GRAVES.GRPS->buf[i]);
  }
}

mresult_t merry_graves_add_group(MerryGravesGroup **grp) {
  if (GRAVES._config->graves_config.group_count_lim &&
      GRAVES.grp_count == GRAVES._config->group_count_limit) {
    return MRES_RESOURCE_LIM_REACHED;
  }
  mresult_t res = merry_graves_group_create(grp, GRAVES.grp_count);
  if (res != MRES_SUCCESS) {
    MERR("Failed to add a new group: GUID: %zu", GRAVES.grp_count);
    return res;
  }
  if (merry_Group_list_push(GRAVES.GRPS, grp) != MRES_SUCCESS) {
    if ((res = merry_Group_list_resize(GRAVES.GRPS, 2)) != MRES_SUCCESS) {
      MERR("Failed to resize the GROUP LIST", NULL);
      return res;
    }
    merry_Group_list_push(GRAVES.GRPS, grp);
  }
  GRAVES.grp_count++;
  return MRES_SUCCESS;
}

mresult_t merry_graves_add_core(MerryGravesGroup *grp, MerryCoreRepr **repr) {
  *repr = merry_graves_group_find_dead_core(grp);

  if (*repr)
    return MRES_SUCCESS;
  if (GRAVES._config->graves_config.core_count_lim &&
      GRAVES.active_core_count == GRAVES._config->core_count_limit) {
    return MRES_RESOURCE_LIM_REACHED;
  }
  *repr = (MerryCoreRepr *)malloc(sizeof(MerryCoreRepr));
  if (!(*repr)) {
    MERR("Failed to allocate memory for new core", NULL);
    return MRES_SYS_FAILURE;
  }
  (*repr)->core = NULL;
  mresult_t res = merry_graves_group_add_core(grp, *repr);

  if (res != MRES_SUCCESS) {
    MERR("Failed to add a new CORE", NULL);
    free(repr);
    return res;
  }

  return MRES_SUCCESS;
}

mresult_t merry_graves_init_a_core(MerryCoreRepr *repr,
                                   maddress_t addr) {
  mresult_t res;

  res = merry_core_create(addr, &repr->core);

  if (res != MRES_SUCCESS) {
    MERR("A core failed to initialize", NULL);
    repr->core = NULL;
    return res;
  }

  return MRES_SUCCESS;
}

mresult_t merry_graves_boot_a_core(MerryCoreRepr *repr) {
  mresult_t res;
  res = merry_core_prepare_inst(repr->core, GRAVES.input->instructions, GRAVES.input->instruction_len);
  if (res != MRES_SUCCESS) {
    MERR("A core failed to BOOT[ID=%zu, UID=%zu, GUID=%zu]",
           repr->iden.id, repr->iden.uid,
           repr->iden.gid);
    merry_core_destroy(repr->core);
    repr->core = NULL;
    return res;
  }
  res = merry_core_prepare_data(repr->core, GRAVES.input->data, GRAVES.input->data_len);
  if (res != MRES_SUCCESS) {
    MERR("A core failed to BOOT[ID=%zu, UID=%zu, GUID=%zu]",
           repr->iden.id, repr->iden.uid,
           repr->iden.gid);
    merry_core_destroy(repr->core);
    repr->core = NULL;
    return res;
  }
  mthread_t th;
  if (merry_create_detached_thread(&th, merry_graves_core_launcher,
                                   (mptr_t)repr) != MRES_SUCCESS) {
    MERR("A core failed to BOOT[ID=%zu, UID=%zu, GUID=%zu]",
           repr->iden.id, repr->iden.uid,
           repr->iden.gid);
    merry_core_destroy(repr->core);
    repr->core = NULL;
    return MRES_SYS_FAILURE;
  }
  GRAVES.active_core_count++;
  return MRES_SUCCESS;
}

void merry_graves_give_IDs_to_cores(MerryCoreRepr *repr) {
  // Initialized and ready to RUN
  // Assigning IDs needed Here
  repr->iden.uid = GRAVES.core_count++;
}

MerryGravesGroup *merry_graves_get_group(mguid_t gid) {
  if (gid >= GRAVES.grp_count)
    return RET_NULL;
  return GRAVES.GRPS->buf[gid];
}

void merry_graves_START() {
  // Boot the first core
  // Since the first steps have been successfull
  // Let's extract the first core directly with no shame
  mresult_t res;
  MerryCoreRepr *first_core;
  res = merry_graves_group_get_core(GRAVES.GRPS->buf[0], &first_core, 0);
  if (res != MRES_SUCCESS)
    return;

  if (merry_graves_boot_a_core(first_core) != MRES_SUCCESS) {
    MERR("[<BOOT>] Failed to start the VM [First core boot failed]",
           NULL);
    goto GRAVES_OVERSEER_END;
  }
  while (1) {
    merry_mutex_lock(&GRAVES.graves_lock);
    if (merry_graves_wants_work(&GRAVES.current_req) == MRES_FAILURE) {
      if (GRAVES.active_core_count == 0) {
        merry_mutex_unlock(&GRAVES.graves_lock);
        goto GRAVES_OVERSEER_END;
      }
      merry_cond_wait(&GRAVES.graves_cond, &GRAVES.graves_lock);
      merry_mutex_unlock(&GRAVES.graves_lock);
    } else {
      // Hanling Requests
      merry_mutex_unlock(&GRAVES.graves_lock);
      switch (GRAVES.current_req->type) {
      default:
        // Unknown requests will result in a panic by default
        MNOTE("Unknown REQUEST made: %zu", GRAVES.current_req->type);
      }
      // After handling the request
      GRAVES.current_req->fufilled = mtrue;
      if (GRAVES.current_req->used_cond)
        merry_cond_signal(GRAVES.current_req->used_cond);
    }
  }

GRAVES_OVERSEER_END:
  merry_graves_cleanup_groups();
  return;
}

_THRET_T_ merry_graves_core_launcher(mptr_t r) {
  MerryCoreRepr *repr = (MerryCoreRepr *)r;
  repr->state.running = mtrue;
  msize_t res = merry_core_run(repr->core);
  merry_core_destroy(repr->core);
  repr->core = NULL;
  MNOTE("Core[ID=%zu UID=%zu GUID=%zu] terminated with result %zu",
       repr->iden.id, repr->iden.uid, repr->iden.gid,
       res);
  GRAVES.active_core_count--;
  return (_THRET_T_)0;
}
