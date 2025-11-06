#include <merry_graves.h>
#include <merry_graves_launcher.h>

MerryGraves GRAVES;

_MERRY_DEFINE_STATIC_LIST_(Group, MerryGravesGroup *);

_MERRY_NO_RETURN_ void Merry_Graves_Run(int argc, char **argv) {
  if (merry_parse_arg(argc, argv) == RET_FAILURE) {
    MFATAL("Graves", "Failed to parse input arguments", NULL);
    exit(-1);
  }
  GRAVES._config = CONSTS();

  // Now we perform pre-initialization
  if (merry_graves_pre_init() == RET_FAILURE)
    goto GRAVES_FAILED_TO_START;

  // We can now read the input file
  if (merry_graves_parse_input() == RET_FAILURE)
    goto GRAVES_FAILED_TO_START;

  // Initialize Graves finally
  if (merry_graves_init() == RET_FAILURE)
    goto GRAVES_FAILED_TO_START;

  // Ready Everything
  if (merry_graves_ready_everything() == RET_FAILURE)
    goto GRAVES_FAILED_TO_START;

  // Finally Ready to RUN the VM
  merry_graves_START(NULL);

  // Exit
  exit(GRAVES.return_value);

GRAVES_FAILED_TO_START:
  // Graves couldn't fully initialize
  exit(-1);
  merry_graves_destroy();
  MLOG("Graves", "Failed to start MERRY: Terminating....", NULL);
  exit(-1);
}

mret_t merry_graves_pre_init() {
  /*
   * If we ever have config files, this is the ideal point to
   * parse them
   * */
  return RET_SUCCESS;
}

mret_t merry_graves_parse_input() {
  GRAVES.C_ENTRIES = merry_graves_parse_metadata_file(
      GRAVES._config->argv[GRAVES._config->inp_file_index]);
  if (!GRAVES.C_ENTRIES) {
    MFATAL("Graves", "Failed to initialize MERRY", NULL);
    return RET_FAILURE;
  }

  return RET_SUCCESS;
}

mret_t merry_graves_init() {
  GRAVES.GRPS = merry_Group_list_create(5);
  if (!GRAVES.GRPS) {
    MFATAL("Graves", "Failed to initialize MERRY: Graves Initialization", NULL);
    return RET_FAILURE;
  }

  if (merry_cond_init(&GRAVES.graves_cond) == RET_FAILURE) {
    MFATAL("Graves",
           "Failed to initialize MERRY: Graves Initialization[Failed to obtain "
           "condition variable]",
           NULL);
    return RET_FAILURE;
  }

  if (merry_mutex_init(&GRAVES.graves_lock) == RET_FAILURE) {
    MFATAL("Graves",
           "Failed to initialize MERRY: Graves Initialization[Failed to obtain "
           "mutex lock]",
           NULL);
    return RET_FAILURE;
  }

  GRAVES.overall_core_count = 0;
  GRAVES.return_value = 0;
  GRAVES.core_count = 0;
  GRAVES.grp_count = 0;
  /*
   * More fields as added
   * */

  return RET_SUCCESS;
}

mret_t merry_graves_ready_everything() {
  // Here we prepare everything before we boot up the first core

  // Acquaint with all cores
  merry_graves_acquaint_with_cores();

  // Create a new group[FIRST GROUP: 0]
  MerryGravesGroup *grp = merry_graves_add_group();
  if (!(grp)) {
    MFATAL("Graves", "Failed to create first GROUP", NULL);
    return RET_FAILURE;
  }

  // Add the first core
  MerryGravesCoreRepr *first_core = merry_graves_add_core(grp);

  if (!first_core) {
    MFATAL("Graves", "Failed to add first CORE", NULL);
    merry_graves_group_destroy(grp);
    return RET_FAILURE;
  }

  // Initialize the first core
  mcore_t first_core_type = merry_graves_obtain_first_valid_c_entry();
  if (first_core_type == __CORE_TYPE_COUNT) {
    merry_unreachable(); // unreachable literally
  }
  if (merry_graves_init_a_core(first_core, first_core_type, 0x00) ==
      RET_FAILURE) {
    MFATAL("Graves", "Failed to initialize the first core...", NULL);
    merry_graves_group_destroy(grp);
    return RET_FAILURE;
  }
  merry_graves_give_IDs_to_cores(first_core, grp);

  // Finally Initialize the Request Queue
  if (merry_graves_req_queue_init() == RET_FAILURE) {
    MFATAL("Graves", "Failed to initialize REQUEST QUEUE HANDLER", NULL);
    first_core->base->deletec(first_core->core);
    first_core->core = NULL;
    merry_graves_group_destroy(grp);
    return RET_FAILURE;
  }
  merry_graves_req_register_wakeup(&GRAVES.graves_cond, &GRAVES.graves_lock);

  return RET_SUCCESS;
}

void merry_graves_acquaint_with_cores() {
  // empty for now
  GRAVES.HOW_TO_CREATE_BASE[__TEST_CORE] = tc_create_base;
  GRAVES.HOW_TO_DESTROY_BASE[__TEST_CORE] = tc_destroy_base;
  GRAVES.HOW_TO_CREATE_BASE[__REGR_CORE] = rbc_master_core_create_base;
  GRAVES.HOW_TO_DESTROY_BASE[__REGR_CORE] = rbc_master_core_destroy_base;
}

void merry_graves_destroy() {
  // Just basic cleanup
  // The cleanup step is comprised of mutliple steps
  // This is the last step
  if (GRAVES.GRPS)
    merry_Group_list_destroy(GRAVES.GRPS);
  if (GRAVES.C_ENTRIES) {
    for (msize_t i = 0; i < __CORE_TYPE_COUNT; i++) {
      if (GRAVES.C_ENTRIES->buf[i])
        free(GRAVES.C_ENTRIES->buf[i]);
    }
    merry_Entry_list_destroy(GRAVES.C_ENTRIES);
  }
  merry_cond_destroy(&GRAVES.graves_cond);
  merry_mutex_destroy(&GRAVES.graves_lock);
  merry_graves_req_queue_free();
}

void merry_graves_cleanup_groups() {
  // Before this function is called, it is necessary that
  // all of the cores have terminated execution
  for (msize_t i = 0; i < GRAVES.grp_count; i++) {
    merry_graves_group_destroy(*merry_Group_list_at(GRAVES.GRPS, i));
  }
}

MerryGravesGroup *merry_graves_add_group() {
  MerryGravesGroup *grp = merry_graves_group_create(GRAVES.grp_count);
  if (!grp) {
    MERROR("Graves", "Failed to add a new group: GUID: %zu", GRAVES.grp_count);
    return RET_NULL;
  }
  if (merry_Group_list_push(GRAVES.GRPS, &grp) == RET_FAILURE) {
    MerryGroupList *grps;
    if ((grps = merry_Group_list_resize(GRAVES.GRPS, 2)) == RET_NULL) {
      MERROR("Graves", "Failed to resize the GROUP LIST", NULL);
      return RET_NULL;
    }
    GRAVES.GRPS = grps;
    merry_graves_group_destroy(grp);
    MERROR("Graves", "Failed to add a new group: GUID: %zu", GRAVES.grp_count);
    return RET_NULL;
  }
  GRAVES.grp_count++;
  return grp;
}

MerryGravesCoreRepr *merry_graves_add_core(MerryGravesGroup *grp) {
  merry_check_ptr(grp);
  MerryGravesCoreRepr *repr = merry_graves_group_find_dead_core(grp);

  if (repr)
    return repr;

  repr = merry_graves_group_add_core(grp);

  if (!repr) {
    MERROR("Graves", "Failed to add a new CORE", NULL);
    return RET_NULL;
  }

  return repr;
}

mret_t merry_graves_init_a_core(MerryGravesCoreRepr *repr, mcore_t type,
                                maddress_t addr) {
  merry_check_ptr(repr);

  if (type >= __CORE_TYPE_COUNT) {
    MERROR("Graves", "Invalid Core Type for a new core: TYPE=%zu",
           (msize_t)type);
    return RET_FAILURE;
  }

  repr->base = GRAVES.HOW_TO_CREATE_BASE[type]();

  if (!repr->base) {
    MERROR("Graves", "Failed to obtain a CORE BASE for a new core: TYPE=%zu",
           (msize_t)type);
    return RET_FAILURE;
  }

  repr->core = repr->base->createc(repr->base, addr);

  if (!repr->core) {
    MERROR("Graves", "A core failed to initialize", NULL);
    GRAVES.HOW_TO_DESTROY_BASE[type](repr->base);
    return RET_FAILURE;
  }

  if (repr->base->setinp(repr->core,
                         *merry_Entry_list_at(GRAVES.C_ENTRIES, type)) ==
      RET_FAILURE) {
    MERROR("Graves", "A core failed to initialize", NULL);
    repr->base->deletec(repr->core);
    GRAVES.HOW_TO_DESTROY_BASE[type](repr->base);
    return RET_FAILURE;
  }

  return RET_SUCCESS;
}

mret_t merry_graves_boot_a_core(MerryGravesCoreRepr *repr) {
  merry_check_ptr(repr);
  merry_check_ptr(repr->base);
  merry_check_ptr(repr->core);

  if (repr->base->prepcore(repr->core) == RET_FAILURE) {
    MERROR("Graves", "A core failed to BOOT[ID=%zu, UID=%zu, GUID=%zu]",
           repr->base->id, repr->base->uid, repr->base->guid);
    repr->base->deletec(repr->core);
    GRAVES.HOW_TO_DESTROY_BASE[repr->base->type](repr->base);
    return RET_FAILURE;
  }

  mthread_t th;
  if (merry_create_detached_thread(&th, merry_graves_launcher,
                                   (mptr_t)repr->base) == RET_FAILURE) {
    MERROR("Graves", "A core failed to BOOT[ID=%zu, UID=%zu, GUID=%zu]",
           repr->base->id, repr->base->uid, repr->base->guid);
    repr->base->deletec(repr->core);
    GRAVES.HOW_TO_DESTROY_BASE[repr->base->type](repr->base);
    return RET_FAILURE;
  }

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
  repr->base->id = merry_graves_group_index_for(grp, repr);
  repr->base->uid = GRAVES.core_count++;
}

_MERRY_ALWAYS_INLINE_ void merry_graves_failed_core_booting() {
  GRAVES.core_count--;
}

void merry_graves_START(mptr_t __) {
  // Boot the first core
  // Since the first steps have been successfull
  // Let's extract the first core directly with no shame
  MerryGravesCoreRepr *first_core =
      merry_graves_group_get_core(*merry_Group_list_at(GRAVES.GRPS, 0), 0);

  if (merry_graves_boot_a_core(first_core) == RET_FAILURE) {
    MFATAL("Graves", "[<BOOT>] Failed to start the VM [First core boot failed]",
           NULL);
    goto GRAVES_OVERSEER_END;
  }
  MerryGravesRequest *req;
  mbool_t is_dead_tmp;
  while (GRAVES.active_core_count == 0){}
  while (1) {
    merry_mutex_lock(&GRAVES.graves_lock);
    if (merry_graves_wants_work(&req) == RET_FAILURE) {
      if (GRAVES.active_core_count == 0) {
        merry_mutex_unlock(&GRAVES.graves_lock);
        goto GRAVES_OVERSEER_END;
      }
      merry_cond_wait(&GRAVES.graves_cond, &GRAVES.graves_lock);
      merry_mutex_unlock(&GRAVES.graves_lock);
    } else {
      // Hanling Requests
      merry_mutex_unlock(&GRAVES.graves_lock);
      MerryGravesGroup **grp =
          merry_Group_list_at(GRAVES.GRPS, req->base->guid);
      MerryGravesCoreRepr *repr = merry_graves_group_find_core(
          *grp, req->base->uid, req->base->id, &is_dead_tmp);
      /*
       * Since a core cannot change any of its IDs and nor can it be
       * dead if it is making a request, the above won't fail
       * */
      MLOG("GRAVES", "REQUEST: ID=%zu, UID=%zu, GUID=%zu", req->base->id,
           req->base->uid, req->base->guid);
      switch (req->type) {
        // .. Requests
      case CREATE_CORE:
        req_create_core(repr, *grp, req);
        break;
      case CREATE_GROUP:
        req_create_group(repr, *grp, req);
        break;
      case GET_GROUP_DETAILS:
        req_get_group_details(repr, *grp, req);
        break;
      case GET_SYSTEM_DETAILS:
        req_get_system_details(repr, *grp, req);
        break;
      default:
        // Unknown requests will result in a panic by default
        MLOG("Graves", "Unknown REQUEST made: ID=%zu, UID=%zu, GUID=%zu",
             req->base->id, req->base->uid, req->base->guid);
      }
      // After handling the request
      merry_cond_signal(req->used_cond);
    }
  }

GRAVES_OVERSEER_END:
  merry_graves_cleanup_groups();
  return;
}

mcore_t merry_graves_obtain_first_valid_c_entry() {
  for (msize_t i = 0; i < (msize_t)__CORE_TYPE_COUNT; i++) {
    if (GRAVES.C_ENTRIES->buf[i])
      return (mcore_t)i;
  }
  return __CORE_TYPE_COUNT;
}
