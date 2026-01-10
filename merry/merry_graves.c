#include <merry_graves.h>

_MERRY_INTERNAL_ MerryCoreInterface interfaces[__CORE_TYPE_COUNT] = {0};
_MERRY_INTERNAL_ MerryGraves GRAVES;

void merry_register_core(mcore_t type, mcorecreate_t ccreate,
                         mcoredeletecore_t cdel, mcoreexec_t cexec,
                         mcorepredel_t cpredel, mcoresetinp_t csetinp,
                         mcoreprepcore_t cprepc, mcoreconfig_t cconf,
                         mcorehelp_t chelp, mcoresetflag_t csflag) {
  if (interfaces[type].set)
    return;
  if (!ccreate || !cdel || !cexec || !cpredel || !csetinp || !cprepc || !cconf || !csflag)
    return;
  MerryCoreInterface *i = &interfaces[type];
  i->ccreate = ccreate;
  i->cdel = cdel;
  i->cexec = cexec;
  i->cpredel = cpredel;
  i->csetinp = csetinp;
  i->cprepc = cprepc;
  i->cconf = cconf;
  i->chelp = chelp;
  i->csflag = csflag;
  i->set = mtrue;
}

_MERRY_DEFINE_STATIC_LIST_(Group, MerryGravesGroup *);

_MERRY_NO_RETURN_ void Merry_Graves_Run(int argc, char **argv) {
  // Now we perform pre-initialization
  if (merry_graves_pre_init() != MRES_SUCCESS)
    goto GRAVES_FAILED_TO_START;

  if (merry_parse_arg(argc, argv, interfaces) == RET_FAILURE) {
    MFATAL("Graves", "Failed to parse input arguments", NULL);
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
  merry_graves_START();

  // Exit
  exit(0);

GRAVES_FAILED_TO_START:
  // Graves couldn't fully initialize
  exit(-1);
  merry_graves_destroy();
  MLOG("Graves", "Failed to start MERRY: Terminating....", NULL);
  exit(-1);
}

mresult_t merry_graves_pre_init() {
  /*
   * If we ever have config files, this is the ideal point to
   * parse them
   * */
  for (msize_t i = 0; i < __CORE_TYPE_COUNT; i++) {
    if (!interfaces[i].set) {
      MFATAL("Graves", "Interface missing for core type %zu", i);
      return MRES_FAILURE;
    }
  }
  return MRES_SUCCESS;
}

mresult_t merry_graves_parse_input() {
  GRAVES.C_ENTRIES = merry_graves_parse_metadata_file(
      GRAVES._config->argv[GRAVES._config->inp_file_index]);
  if (!GRAVES.C_ENTRIES) {
    MFATAL("Graves", "Failed to initialize MERRY", NULL);
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
    MFATAL("Graves", "Failed to initialize MERRY: Graves Initialization", NULL);
    return res;
  }

  if (merry_cond_init(&GRAVES.graves_cond) != MRES_SUCCESS) {
    MFATAL("Graves",
           "Failed to initialize MERRY: Graves Initialization[Failed to obtain "
           "condition variable]",
           NULL);
    return MRES_FAILURE;
  }

  if (merry_mutex_init(&GRAVES.graves_lock) != MRES_SUCCESS) {
    MFATAL("Graves",
           "Failed to initialize MERRY: Graves Initialization[Failed to obtain "
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
    MFATAL("Graves", "Failed to create first GROUP", NULL);
    return res;
  }

  // Add the first core
  MerryCoreRepr *first_core;
  res = merry_graves_add_core(grp, &first_core);

  if (!first_core) {
    MFATAL("Graves", "Failed to add first CORE", NULL);
    merry_graves_group_destroy(grp);
    return res;
  }

  // Initialize the first core
  mcore_t first_core_type = merry_graves_obtain_first_valid_c_entry();
  if (merry_graves_init_a_core(first_core, first_core_type, 0x00) !=
      MRES_SUCCESS) {
    MFATAL("Graves", "Failed to initialize the first core...", NULL);
    merry_graves_group_destroy(grp);
    return MRES_FAILURE;
  }
  merry_graves_give_IDs_to_cores(first_core, grp);

  // Finally Initialize the Request Queue
  if (merry_graves_req_queue_init() == RET_FAILURE) {
    MFATAL("Graves", "Failed to initialize REQUEST QUEUE HANDLER", NULL);
    interfaces[first_core_type].cdel(first_core->core);
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
    MERROR("Graves", "Failed to add a new group: GUID: %zu", GRAVES.grp_count);
    return res;
  }
  if (merry_Group_list_push(GRAVES.GRPS, grp) != MRES_SUCCESS) {
    if ((res = merry_Group_list_resize(GRAVES.GRPS, 2)) != MRES_SUCCESS) {
      MERROR("Graves", "Failed to resize the GROUP LIST", NULL);
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
    MERROR("Graves", "Failed to allocate memory for new core", NULL);
    return MRES_SYS_FAILURE;
  }
  (*repr)->core = NULL;
  mresult_t res = merry_graves_group_add_core(grp, *repr);

  if (res != MRES_SUCCESS) {
    MERROR("Graves", "Failed to add a new CORE", NULL);
    free(repr);
    return res;
  }

  return MRES_SUCCESS;
}

mresult_t merry_graves_init_a_core(MerryCoreRepr *repr, mcore_t type,
                                   maddress_t addr) {
  MerryCoreInterface *i = &interfaces[type];
  mresult_t res;

  res = i->ccreate(repr->metadata.iden, addr, &repr->core);

  if (res != MRES_SUCCESS) {
    MERROR("Graves", "A core failed to initialize", NULL);
    return res;
  }

  if ((res = i->csetinp(repr->core, GRAVES.C_ENTRIES->buf[type])) !=
      MRES_SUCCESS) {
    MERROR("Graves", "A core failed to initialize", NULL);
    i->cdel(repr->core);
    return res;
  }
  repr->type = type;
  return MRES_SUCCESS;
}

mresult_t merry_graves_boot_a_core(MerryCoreRepr *repr) {
  MerryCoreInterface *i = &interfaces[repr->type];
  mresult_t res;
  if ((res = i->cprepc(repr->core)) != MRES_SUCCESS) {
    MERROR("Graves", "A core failed to BOOT[ID=%zu, UID=%zu, GUID=%zu]",
           repr->metadata.iden.id, repr->metadata.iden.uid,
           repr->metadata.iden.gid);
    i->cdel(repr->core);
    return res;
  }

  mthread_t th;
  if (merry_create_detached_thread(&th, merry_graves_core_launcher,
                                   (mptr_t)repr) != MRES_SUCCESS) {
    MERROR("Graves", "A core failed to BOOT[ID=%zu, UID=%zu, GUID=%zu]",
           repr->metadata.iden.id, repr->metadata.iden.uid,
           repr->metadata.iden.gid);
    i->cdel(repr->core);
    return MRES_SYS_FAILURE;
  }
  GRAVES.active_core_count++;
  return MRES_SUCCESS;
}

void merry_graves_give_IDs_to_cores(MerryCoreRepr *repr,
                                    MerryGravesGroup *grp) {
  // Initialized and ready to RUN
  // Assigning IDs needed Here
  repr->metadata.iden.uid = GRAVES.core_count++;
}

_MERRY_ALWAYS_INLINE_ void merry_graves_failed_core_booting() {
  GRAVES.active_core_count--;
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
    MFATAL("Graves", "[<BOOT>] Failed to start the VM [First core boot failed]",
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
        // .. Requests
      case NOP: // will define purpose
        break;
      case CREATE_CORE:
        req_create_core(GRAVES.current_req);
        break;
      case CREATE_GROUP:
        req_create_group(GRAVES.current_req);
        break;
      default:
        // Unknown requests will result in a panic by default
        MLOG("Graves", "Unknown REQUEST made: %zu", GRAVES.current_req->type);
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

mcore_t merry_graves_obtain_first_valid_c_entry() {
  for (msize_t i = 0; i < (msize_t)__CORE_TYPE_COUNT; i++) {
    if (GRAVES.C_ENTRIES->buf[i])
      return (mcore_t)i;
  }
  return __CORE_TYPE_COUNT;
}

_THRET_T_ merry_graves_core_launcher(mptr_t r) {
  MerryCoreRepr *repr = (MerryCoreRepr *)r;
  MerryCoreInterface *i = &interfaces[repr->type];
  mresult_t res = i->cexec(repr->core);
  i->cdel(repr->core);
  repr->core = NULL;
  MLOG("Graves", "Core[ID=%zu UID=%zu GUID=%zu] terminated with result %zu",
       repr->metadata.iden.id, repr->metadata.iden.uid, repr->metadata.iden.gid,
       res);
  GRAVES.active_core_count--;
  return (_THRET_T_)0;
}
