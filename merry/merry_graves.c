#include <merry_graves.h>
#include <merry_graves_launcher.h>

_MERRY_DEFINE_STATIC_LIST_(Group, MerryGravesGroup *);

_MERRY_NO_RETURN_ void Merry_Graves_Run(int argc, char **argv) {
  MerryGraves GRAVES;
  // Now we perform pre-initialization
  if (merry_graves_pre_init(&GRAVES) != MRES_SUCCESS)
    goto GRAVES_FAILED_TO_START;

  if (merry_parse_arg(argc, argv, GRAVES.CONFIG_CORE) == RET_FAILURE) {
    MFATAL("Graves", "Failed to parse input arguments", NULL);
    exit(-1);
  }
  GRAVES._config = CONSTS();

  // We can now read the input file
  if (merry_graves_parse_input(&GRAVES) != MRES_SUCCESS)
    goto GRAVES_FAILED_TO_START;

  // Initialize Graves finally
  if (merry_graves_init(&GRAVES) != MRES_SUCCESS)
    goto GRAVES_FAILED_TO_START;

  // Ready Everything
  if (merry_graves_ready_everything(&GRAVES) != MRES_SUCCESS)
    goto GRAVES_FAILED_TO_START;

  // Finally Ready to RUN the VM
  merry_graves_START(&GRAVES);

  // Exit
  exit(GRAVES.return_value);

GRAVES_FAILED_TO_START:
  // Graves couldn't fully initialize
  exit(-1);
  merry_graves_destroy(&GRAVES);
  MLOG("Graves", "Failed to start MERRY: Terminating....", NULL);
  exit(-1);
}

mresult_t merry_graves_pre_init(MerryGraves *GRAVES) {
  /*
   * If we ever have config files, this is the ideal point to
   * parse them
   * */
  // GRAVES->CONFIG_CORE[__TEST_CORE] = tc_config_core;
  // GRAVES->CONFIG_CORE[__REGR_CORE] = rbc_config_core;
  merry_graves_launcher_set(GRAVES);
  return MRES_SUCCESS;
}

mresult_t merry_graves_parse_input(MerryGraves *GRAVES) {
  GRAVES->C_ENTRIES = merry_graves_parse_metadata_file(
      GRAVES->_config->argv[GRAVES->_config->inp_file_index]);
  if (!GRAVES->C_ENTRIES) {
    MFATAL("Graves", "Failed to initialize MERRY", NULL);
    return MRES_FAILURE;
  }

  return MRES_SUCCESS;
}

mresult_t merry_graves_init(MerryGraves *GRAVES) {
  mresult_t res;
  msize_t grp_count = 5;
  if (GRAVES->_config->graves_config.group_count_lim)
  	grp_count = GRAVES->_config->group_count_limit; 
  res = merry_Group_list_create(grp_count, &GRAVES->GRPS);
  if (!GRAVES->GRPS) {
    MFATAL("Graves", "Failed to initialize MERRY: Graves Initialization", NULL);
    return res;
  }

  if (merry_cond_init(&GRAVES->graves_cond) != MRES_SUCCESS) {
    MFATAL("Graves",
           "Failed to initialize MERRY: Graves Initialization[Failed to obtain "
           "condition variable]",
           NULL);
    return MRES_FAILURE;
  }

  if (merry_mutex_init(&GRAVES->graves_lock) != MRES_SUCCESS) {
    MFATAL("Graves",
           "Failed to initialize MERRY: Graves Initialization[Failed to obtain "
           "mutex lock]",
           NULL);
    return MRES_FAILURE;
  }

  GRAVES->current_req = NULL;
  GRAVES->core_count = 0;
  GRAVES->grp_count = 0;
  GRAVES->active_core_count = 0;
  /*
   * More fields as added
   * */

  return MRES_SUCCESS;
}

mresult_t merry_graves_ready_everything(MerryGraves *GRAVES) {
  // Here we prepare everything before we boot up the first core

  // Acquaint with all cores
  merry_graves_acquaint_with_cores(GRAVES);

  // Create a new group[FIRST GROUP: 0]
  MerryGravesGroup *grp;
  mresult_t res = merry_graves_add_group(GRAVES, &grp);
  if (!(grp)) {
    MFATAL("Graves", "Failed to create first GROUP", NULL);
    return res;
  }

  // Add the first core
  MerryGravesCoreRepr *first_core;
  res = merry_graves_add_core(GRAVES, grp, &first_core);

  if (!first_core) {
    MFATAL("Graves", "Failed to add first CORE", NULL);
    merry_graves_group_destroy(grp);
    return res;
  }

  // Initialize the first core
  mcore_t first_core_type = merry_graves_obtain_first_valid_c_entry(GRAVES);
  if (first_core_type >
      GRAVES->_config->current_mode_allowed_largest_ctype_id) {
    MFATAL("Graves", "Attempting to use a core that isn't ready or exists",
           NULL);
    merry_graves_group_destroy(grp);
    return MRES_FAILURE;
  }
  if (merry_graves_init_a_core(GRAVES, first_core, first_core_type, 0x00) !=
      MRES_SUCCESS) {
    MFATAL("Graves", "Failed to initialize the first core...", NULL);
    merry_graves_group_destroy(grp);
    return MRES_FAILURE;
  }
  merry_graves_give_IDs_to_cores(GRAVES, first_core, grp);

  // Finally Initialize the Request Queue
  if (merry_graves_req_queue_init() == RET_FAILURE) {
    MFATAL("Graves", "Failed to initialize REQUEST QUEUE HANDLER", NULL);
    first_core->base->deletec(first_core->core);
    first_core->core = NULL;
    merry_graves_group_destroy(grp);
    return MRES_FAILURE;
  }
  merry_graves_req_register_wakeup(&GRAVES->graves_cond, &GRAVES->graves_lock);

  return MRES_SUCCESS;
}

void merry_graves_acquaint_with_cores(MerryGraves *GRAVES) {
  // empty for now
  GRAVES->HOW_TO_CREATE_BASE[__TEST_CORE] = tc_create_base;
  GRAVES->HOW_TO_DESTROY_BASE[__TEST_CORE] = tc_destroy_base;
  GRAVES->HOW_TO_CREATE_BASE[__REGR_CORE] = rbc_master_core_create_base;
  GRAVES->HOW_TO_DESTROY_BASE[__REGR_CORE] = rbc_master_core_destroy_base;
  if (GRAVES->_config->graves_config.test_mode == mtrue) {
    // List all of the test mode cores too!
  }
}

void merry_graves_destroy(MerryGraves *GRAVES) {
  // Just basic cleanup
  // The cleanup step is comprised of mutliple steps
  // This is the last step
  if (GRAVES->GRPS)
    merry_Group_list_destroy(GRAVES->GRPS);
  if (GRAVES->C_ENTRIES) {
    for (msize_t i = 0; i < __CORE_TYPE_COUNT; i++) {
      if (GRAVES->C_ENTRIES->buf[i])
        free(GRAVES->C_ENTRIES->buf[i]);
    }
    merry_Entry_list_destroy(GRAVES->C_ENTRIES);
  }
  merry_cond_destroy(&GRAVES->graves_cond);
  merry_mutex_destroy(&GRAVES->graves_lock);
  merry_graves_req_queue_free();
}

void merry_graves_cleanup_groups(MerryGraves *GRAVES) {
  // Before this function is called, it is necessary that
  // all of the cores have terminated execution
  for (msize_t i = 0; i < GRAVES->grp_count; i++) {
    merry_graves_group_destroy(GRAVES->GRPS->buf[i]);
  }
}

mresult_t merry_graves_add_group(MerryGraves *GRAVES, MerryGravesGroup **grp) {
  if (GRAVES->_config->graves_config.group_count_lim &&
      GRAVES->grp_count == GRAVES->_config->group_count_limit) {
    return MRES_RESOURCE_LIM_REACHED;
  }
  mresult_t res = merry_graves_group_create(grp, GRAVES->grp_count);
  if (res != MRES_SUCCESS) {
    MERROR("Graves", "Failed to add a new group: GUID: %zu", GRAVES->grp_count);
    return res;
  }
  if (merry_Group_list_push(GRAVES->GRPS, grp) != MRES_SUCCESS) {
    if ((res = merry_Group_list_resize(GRAVES->GRPS, 2)) != MRES_SUCCESS) {
      MERROR("Graves", "Failed to resize the GROUP LIST", NULL);
      return res;
    }
    merry_Group_list_push(GRAVES->GRPS, grp);
  }
  GRAVES->grp_count++;
  return MRES_SUCCESS;
}

mresult_t merry_graves_add_core(MerryGraves *GRAVES, MerryGravesGroup *grp,
                                MerryGravesCoreRepr **repr) {
  merry_check_ptr(grp);
  *repr = merry_graves_group_find_dead_core(grp);

  if (*repr)
    return MRES_SUCCESS;
  if (GRAVES->_config->graves_config.core_count_lim &&
      GRAVES->active_core_count == GRAVES->_config->core_count_limit) {
    return MRES_RESOURCE_LIM_REACHED;
  }
  *repr = (MerryGravesCoreRepr*)malloc(sizeof(MerryGravesCoreRepr));
  if (!(*repr)) {
  	MERROR("Graves", "Failed to allocate memory for new core", NULL);
  	return MRES_SYS_FAILURE;
  }
  (*repr)->base = NULL;
  (*repr)->core = NULL;
  (*repr)->dead = mtrue;
  (*repr)->core_creation_lim = -1;
  (*repr)->group_creation_lim = -1;
  (*repr)->sub_core_creation_lim = -1;
  mresult_t res = merry_graves_group_add_core(grp, repr);

  if (res != MRES_SUCCESS) {
    MERROR("Graves", "Failed to add a new CORE", NULL);
	free(repr);
    return res;
  }

  return MRES_SUCCESS;
}

mresult_t merry_graves_init_a_core(MerryGraves *GRAVES,
                                   MerryGravesCoreRepr *repr, mcore_t type,
                                   maddress_t addr) {
  merry_check_ptr(repr);

  if (type > GRAVES->_config->current_mode_allowed_largest_ctype_id)
    return MRES_NOT_ALLOWED;

  mresult_t res = GRAVES->HOW_TO_CREATE_BASE[type](&repr->base);

  if (!repr->base) {
    MERROR("Graves", "Failed to obtain a CORE BASE for a new core: TYPE=%zu",
           (msize_t)type);
    return res;
  }

  res = repr->base->createc(repr->base, addr,(mptr_t)repr, &repr->core);

  if (!repr->core) {
    MERROR("Graves", "A core failed to initialize", NULL);
    GRAVES->HOW_TO_DESTROY_BASE[type](repr->base);
    return res;
  }

  if ((res = repr->base->setinp(repr->core, GRAVES->C_ENTRIES->buf[type])) != MRES_SUCCESS) {
    MERROR("Graves", "A core failed to initialize", NULL);
    repr->base->deletec(repr->core);
    GRAVES->HOW_TO_DESTROY_BASE[type](repr->base);
    return res;
  }
  repr->type = type;

  return MRES_SUCCESS;
}

mresult_t merry_graves_boot_a_core(MerryGraves *GRAVES,
                                   MerryGravesCoreRepr *repr) {
  merry_check_ptr(repr);
  merry_check_ptr(repr->base);
  merry_check_ptr(repr->core);

  mresult_t res;
  if ((res = repr->base->prepcore(repr->core)) != MRES_SUCCESS) {
    MERROR("Graves", "A core failed to BOOT[ID=%zu, UID=%zu, GUID=%zu]",
           repr->id, repr->uid, repr->guid);
    repr->base->deletec(repr->core);
    GRAVES->HOW_TO_DESTROY_BASE[repr->type](repr->base);
    return res;
  }

  mthread_t th;
  if (merry_create_detached_thread(&th, merry_graves_launcher,
                                   (mptr_t)repr) != MRES_SUCCESS) {
    MERROR("Graves", "A core failed to BOOT[ID=%zu, UID=%zu, GUID=%zu]",
           repr->id, repr->uid, repr->guid);
    repr->base->deletec(repr->core);
    GRAVES->HOW_TO_DESTROY_BASE[repr->type](repr->base);
    return MRES_SYS_FAILURE;
  }
  repr->dead = mfalse;
  repr->active_core_count++;
  
  return MRES_SUCCESS;
}

void merry_graves_give_IDs_to_cores(MerryGraves *GRAVES,
                                    MerryGravesCoreRepr *repr,
                                    MerryGravesGroup *grp) {
  merry_check_ptr(repr);
  merry_check_ptr(repr->core);
  merry_check_ptr(grp);

  // Initialized and ready to RUN
  // Assigning IDs needed Here
  repr->guid = grp->group_id;
  repr->id = merry_graves_group_index_for(grp, repr);
  repr->uid = GRAVES->core_count++;
}

_MERRY_ALWAYS_INLINE_ void
merry_graves_failed_core_booting(MerryGraves *GRAVES) {
  GRAVES->core_count--;
}

void merry_graves_START(MerryGraves* GRAVES) {
  // Boot the first core
  // Since the first steps have been successfull
  // Let's extract the first core directly with no shame
  mresult_t res;
  MerryGravesCoreRepr *first_core;
  res = merry_graves_group_get_core(GRAVES->GRPS->buf[0], &first_core, 0);
  if (res != MRES_SUCCESS)
    return;

  if (merry_graves_boot_a_core(GRAVES, first_core) != MRES_SUCCESS) {
    MFATAL("Graves", "[<BOOT>] Failed to start the VM [First core boot failed]",
           NULL);
    goto GRAVES_OVERSEER_END;
  }
  while (1) {
    merry_mutex_lock(&GRAVES->graves_lock);
    if (merry_graves_wants_work(&GRAVES->current_req) == MRES_FAILURE) {
      if (GRAVES->active_core_count == 0) {
        merry_mutex_unlock(&GRAVES->graves_lock);
        goto GRAVES_OVERSEER_END;
      }
      merry_cond_wait(&GRAVES->graves_cond, &GRAVES->graves_lock);
      merry_mutex_unlock(&GRAVES->graves_lock);
    } else {
      // Hanling Requests
      merry_mutex_unlock(&GRAVES->graves_lock);
      MerryGravesCoreRepr *repr = (MerryGravesCoreRepr*)GRAVES->current_req->repr;
	  if (!repr || repr->dead) {
	  	// Invalid request
	  	MERROR("Graves", "Invalid Request posted by UNKNOWN CORE.", NULL);
	  	continue;
	  }
      MerryGravesGroup *grp =
          GRAVES->GRPS->buf[repr->guid];
      MLOG("GRAVES", "REQUEST: ID=%zu, UID=%zu, GUID=%zu",
           repr->id, repr->uid,
           repr->guid);
      switch (GRAVES->current_req->type) {
        // .. Requests
      case NOP: // will define purpose
        break;
      case CREATE_CORE:
        req_create_core(GRAVES, grp, GRAVES->current_req);
        break;
      case CREATE_GROUP:
        req_create_group(GRAVES, grp, GRAVES->current_req);
        break;
      case GET_GROUP_DETAILS:
        req_get_group_details(GRAVES, grp, GRAVES->current_req);
        break;
      case GET_SYSTEM_DETAILS:
        req_get_system_details(GRAVES, grp, GRAVES->current_req);
        break;
      default:
        // Unknown requests will result in a panic by default
        MLOG("Graves", "Unknown REQUEST made: ID=%zu, UID=%zu, GUID=%zu",
             repr->id, repr->uid,
             repr->guid);
      }
      // After handling the request
      GRAVES->current_req->fufilled = mtrue;
      if (GRAVES->current_req->used_cond)
      	merry_cond_signal(GRAVES->current_req->used_cond);
    }
  }

GRAVES_OVERSEER_END:
  merry_graves_cleanup_groups(GRAVES);
  return;
}

mcore_t merry_graves_obtain_first_valid_c_entry(MerryGraves *GRAVES) {
  for (msize_t i = 0; i < (msize_t)__CORE_TYPE_COUNT; i++) {
    if (GRAVES->C_ENTRIES->buf[i])
      return (mcore_t)i;
  }
  return __CORE_TYPE_COUNT;
}
