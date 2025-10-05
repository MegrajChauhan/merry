#include <merry_graves.h>

_MERRY_NO_RETURN_ void Merry_Graves_Run(int argc, char **argv) {
  if (merry_parse_arg(argc, argv) == RET_FAILURE) {
    merry_err("[Failed to parse argument]", NULL);
    exit(-1);
  }
  GRAVES._config = CONSTS();

  MerryErrorStack init_st;
  merry_error_stack_init(&init_st, -1, -1, -1);
  // Now we perform pre-initialization
  if (merry_graves_pre_init(&init_st) == RET_FAILURE)
    goto GRAVES_FAILED_TO_START;

  // We can now read the input file
  if (merry_graves_parse_input(&init_st) == RET_FAILURE)
    goto GRAVES_FAILED_TO_START;

  // Initialize Graves finally
  if (merry_graves_init(&init_st) == RET_FAILURE)
    goto GRAVES_FAILED_TO_START;

  // Ready Everything
  if (merry_graves_ready_everything(&init_st) == RET_FAILURE)
    goto GRAVES_FAILED_TO_START;

  // Finally Ready to RUN the VM
  merry_graves_START(NULL);

  // Exit
  exit(GRAVES.return_value);

GRAVES_FAILED_TO_START:
  // Graves couldn't fully initialize
  ERROR(&init_st);
  exit(-1);
  merry_err("GRAVES: Couldn't start the machine...", NULL);
  merry_graves_destroy(&init_st);
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
  GRAVES.core_count = 0;
  /*
   * More fields as added
   * */

  return RET_SUCCESS;
}

mret_t merry_graves_ready_everything(MerryErrorStack *st) {
  // Here we prepare everything before we boot up the first core

  if (merry_hord_init(st) == RET_FAILURE)
    return RET_FAILURE;
  // Acquaint with all cores
  merry_graves_acquaint_with_cores();

  // Create a new group[FIRST GROUP: 0]
  MerryGravesGroup *grp = merry_graves_group_create(0, st);
  if (!grp) {
    PUSH(st, NULL, "First group creation failed", "Readying Merry");
    return RET_FAILURE;
  }

  // Add the first core
  MerryGravesCoreRepr *first_core = merry_graves_group_add_core(grp, st);

  if (!first_core) {
    PUSH(st, NULL, "First core creation failed", "Readying Merry");
    merry_graves_group_destroy(grp);
    return RET_FAILURE;
  }

  // Initialize the first core
  if (merry_graves_init_a_core(first_core, GRAVES.input->itit.entries[0].type,
                               0x00, st) == RET_FAILURE) {
    PUSH(st, NULL, "First core Initialization failed", "Readying Merry");
    merry_graves_group_destroy(grp);
    return RET_FAILURE;
  }

  merry_graves_give_IDs_to_cores(first_core, grp);

  // Add the group
  if (merry_dynamic_list_push(GRAVES.GRPS, (mptr_t)grp, st) == RET_FAILURE) {
    PUSH(st, NULL, "First Group Registration Failed", "Readying Merry");
    first_core->base->deletec(first_core->core);
    first_core->core = NULL;
    merry_graves_group_destroy(grp);
    return RET_FAILURE;
  }

  // Finally Initialize the Request Queue
  if (merry_graves_req_queue_init(&GRAVES.graves_cond, st) == RET_FAILURE) {
    PUSH(st, NULL, "Request Queue Initialization Failed", "Readying Merry");
    first_core->base->deletec(first_core->core);
    first_core->core = NULL;
    merry_graves_group_destroy(grp);
    return RET_FAILURE;
  }

  return RET_SUCCESS;
}

void merry_graves_acquaint_with_cores() {
  // empty for now
}

void merry_graves_destroy(MerryErrorStack *st) {
  // Just basic cleanup
  // The cleanup step is comprised of mutliple steps
  // This is the last step
  if (GRAVES.GRPS)
    merry_destroy_dynamic_list(GRAVES.GRPS);
  if (GRAVES.input)
    merry_graves_reader_destroy(GRAVES.input, st);
  if (GRAVES.GRPS)
    merry_destroy_dynamic_list(GRAVES.GRPS);
  merry_cond_destroy(&GRAVES.graves_cond);
  merry_mutex_destroy(&GRAVES.graves_lock);
  merry_graves_req_queue_free();
  merry_hord_destroy();
  merry_destroy_memory_interface();
}

void merry_graves_cleanup_groups() {
  // Before this function is called, it is necessary that
  // all of the cores have terminated execution
  for (msize_t i = 0; i < merry_dynamic_list_size(GRAVES.GRPS); i++) {
    MerryGravesGroup *grp = merry_dynamic_list_at(GRAVES.GRPS, i);
    MerryGravesCoreRepr *repr;
    for (msize_t j = 0;; j++) {
      repr = merry_graves_group_get_core(grp, j);
      if (!repr)
        break;
      repr->base->predel(repr->core);
      repr->base->deletec(repr->core);
      repr->core = NULL;
      GRAVES.HOW_TO_DESTROY_BASE[repr->base->type](repr->base);
    }
    merry_graves_group_destroy(grp);
  }
}

MerryGravesGroup *merry_graves_add_group(MerryErrorStack *st) {
  MerryGravesGroup *grp =
      merry_graves_group_create(merry_dynamic_list_size(GRAVES.GRPS), st);
  if (!grp) {
    PUSH(st, NULL, "Failed to add a group", "Adding Group");
    return RET_NULL;
  }
  if (merry_dynamic_list_push(GRAVES.GRPS, (mptr_t)grp, st) == RET_FAILURE) {
    merry_graves_group_destroy(grp);
    PUSH(st, NULL, "Failed to add a group", "Adding Group");
    return RET_NULL;
  }
  return grp;
}

MerryGravesCoreRepr *merry_graves_add_core(MerryGravesGroup *grp,
                                           MerryErrorStack *st) {
  merry_check_ptr(grp);
  MerryGravesCoreRepr *repr = merry_graves_group_find_dead_core(grp);

  if (repr)
    return repr;

  repr = merry_graves_group_add_core(grp, st);

  if (!repr) {
    PUSH(st, NULL, "Core creation failed", "Adding Core");
    return RET_NULL;
  }

  return repr;
}

mret_t merry_graves_init_a_core(MerryGravesCoreRepr *repr, mcore_t type,
                                maddress_t addr, MerryErrorStack *st) {
  merry_check_ptr(repr);

  if (type >= __CORE_TYPE_COUNT) {
    PUSH(st, "Invalid Core Type",
         "Invalid core type passed for creating new core",
         "Initializing New Core");
    return RET_FAILURE;
  }

  repr->base = GRAVES.HOW_TO_CREATE_BASE[type](st);

  if (!repr->base) {
    PUSH(st, NULL, "Failed to BUILD core base", "Initializing New Core");
    return RET_FAILURE;
  }

  repr->core = repr->base->createc(repr->base, addr, st);

  if (!repr->core) {
    PUSH(st, NULL, "Failed to BUILD core", "Initializing New Core");
    GRAVES.HOW_TO_DESTROY_BASE[type](repr->base);
    return RET_FAILURE;
  }

  MerryRAM *dram = merry_graves_reader_get_data_RAM(GRAVES.input, st);
  if (!dram) {
    PUSH(st, NULL, "Failed to GET data ram", "Initializing New Core");
    repr->base->deletec(repr->core);
    repr->core = NULL;
    GRAVES.HOW_TO_DESTROY_BASE[type](repr->base);
    return RET_FAILURE;
  }

  repr->base->iram = GRAVES.input->iram[type];
  repr->base->ram = dram;

  return RET_SUCCESS;
}

mret_t merry_graves_boot_a_core(MerryGravesCoreRepr *repr,
                                MerryErrorStack *st) {
  merry_check_ptr(repr);
  merry_check_ptr(repr->base);
  merry_check_ptr(repr->core);

  mthread_t th;
  if (merry_create_detached_thread(&th, repr->base->execc, repr->core, st) ==
      RET_FAILURE) {
    PUSH(st, NULL, "Failed to Boot a core", "Booting New Core");
    return RET_FAILURE;
  }

  MerryGravesGroup *grp = merry_dynamic_list_at(GRAVES.GRPS, repr->base->guid);
  merry_graves_group_register_new_core(grp);
  GRAVES.active_core_count++;

  return RET_SUCCESS;
}

void merry_graves_give_IDs_to_cores(MerryGravesCoreRepr *repr,
                                    MerryGravesGroup *grp) {
  merry_check_ptr(repr);
  merry_check_ptr(repr->core);
  merry_check_ptr(grp);

  // Initialized and ready to RUN
  // Assigning IDs needed Here
  repr->base->guid = grp->group_id;
  repr->base->id = merry_graves_group_index_for(grp, (mptr_t)repr);
  repr->base->uid = GRAVES.core_count++;
}

void merry_graves_START(mptr_t __) {
  // Boot the first core
  // Since the first steps have been successfull
  // Let's extract the first core directly with no shame

  MerryErrorStack merry_stack;
  merry_error_stack_init(&merry_stack, -1, -1, -1);

  MerryGravesCoreRepr *first_core =
      (MerryGravesCoreRepr *)(((MerryGravesGroup *)(GRAVES.GRPS->buf))
                                  ->all_cores->buf);
  if (merry_graves_boot_a_core(first_core, &merry_stack) == RET_FAILURE) {
    ERROR(&merry_stack);
    merry_err("[<BOOT>]: Failed to start the VM", NULL);
    goto GRAVES_OVERSEER_END;
  }
  MerryGravesRequest *req;
  mbool_t is_dead_tmp;
  while (1) {
    if (merry_graves_wants_work(&req) == RET_FAILURE) {
      if (GRAVES.active_core_count == 0) {
        goto GRAVES_OVERSEER_END;
      } else if (merry_graves_get_req_queue_error_stack()->fatality) {
        goto GRAVES_OVERSEER_END;
      }
      merry_cond_wait(&GRAVES.graves_cond, &GRAVES.graves_lock);
    } else {
      // Hanling Requests
      MerryGravesGroup *grp = merry_dynamic_list_at(GRAVES.GRPS, req->guid);
      MerryGravesCoreRepr *repr =
          merry_graves_group_find_core(grp, req->uid, req->id, &is_dead_tmp);
      /*
       * Since a core cannot change any of its IDs and nor can it be
       * dead if it is making a request, the above won't fail
       * */
      if (req->sys_request == mtrue) {
        switch (req->stype) {
        case _SYS_FAILURE: {
          // When a core fails in anyway, even if
          // it was because of a System failure such
          // as memory allocation failure then
          // it will handle it in a way it sees
          // fit. It must call KILL_SELF before
          // terminating if it wants to
          // We simply dump the error of the core
          ERROR(&repr->base->estack);
          merry_err("CORE FAILED[SYS FAILURE]", NULL);
          break;
        }
        case _GRAVES_HORD_FAILED: {
          if (merry_hords_status() == mfalse) {
            // Hords has failed
            ERROR(merry_hord_error_status());
            merry_err("HORD FAILED[SYSTEM FAILURE]", NULL);
            // Hords failing isn't funny and it calls for total
            // shutdown
            GRAVES.return_value = -1;
            goto GRAVES_OVERSEER_END;
          }
        }
        }
      } else {
        switch (req->type) {
          // .. Requests
        case KILL_SELF:
          req_kill_self(repr, grp);
          continue;
        case CREATE_CORE:
          req_create_core(repr, grp);
          break;
        case CREATE_GROUP:
          req_create_group(repr, grp);
          break;
        case GET_GROUP_DETAILS:
          req_get_group_details(repr, grp);
          break;
        case GET_SYSTEM_DETAILS:
          req_get_system_details(repr, grp);
          break;
        default:
          // Unknown requests will result in a panic by default
          merry_unreachable(
              "UNKNOWN REQUEST: Core ID: %zu, UID: %zu, GUID: %zu "
              "made request %u which is not valid.",
              req->id, req->uid, req->guid, req->type);
        }
      }
      // After handling the request
      merry_cond_signal(&repr->base->cond);
    }
  }

GRAVES_OVERSEER_END:
  merry_graves_cleanup_groups();
  return;
}
