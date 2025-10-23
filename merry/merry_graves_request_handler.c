#include <merry_graves.h>

REQ(kill_self) {
  /*
   * This process involves:
   * - Commanding the destruction of its core
   * - Declare dead
   * - Destroy the base
   * - Register the death
   * */
  repr->base->interrupt = mtrue;
  repr->core = NULL; // Declare dead
  merry_graves_group_register_dead_core(grp);
  GRAVES.active_core_count--;
}

REQ(create_core) {
  /*
   * Creating a new core:
   * - Either create a new core in the same group
   * - Or create a new core in a new group
   * - Or create a new core in an exisiting group
   * */
  MerryRequestArgs *args = req->args;
  ;
  if (args->create_core.new_core_type >= __CORE_TYPE_COUNT) {
    // Locally fatal i.e fatal to the core who will dump its
    // error messages and then terminate
    req->res = GREQUEST_FAILURE;
    req->failed.mfailure = GREQUEST_INVALID_ARGS;
    return;
  }
  MerryGravesCoreRepr *new_repr;
  if (args->create_core.new_group == mtrue &&
      args->create_core.same_group == mfalse) {
    // A new group needed
    MerryGravesGroup *ngrp = merry_graves_add_group();
    if (!ngrp) {
      req->res = GREQUEST_MERRY_FAILURE;
      req->failed.errno_if_err = errno;
      return;
    }
    grp = ngrp;
  } else if (args->create_core.new_group == mfalse &&
             args->create_core.same_group == mfalse) {
    // use an exisiting group
    MerryGravesGroup **ogrp =
        merry_Group_list_at(GRAVES.GRPS, args->create_core.gid);
    if (!ogrp) {
      req->res = GREQUEST_MERRY_FAILURE;
      req->failed.errno_if_err = errno;
      return;
    }
    grp = *ogrp;
  } else if (args->create_core.same_group == mtrue) {
  } else {
    // None of the possibilities matches hence an error
    req->res = GREQUEST_FAILURE;
    req->failed.mfailure = GREQUEST_INVALID_ARGS;
    return;
  }
  new_repr = merry_graves_add_core(grp);
  if (!new_repr) {
    req->res = GREQUEST_MERRY_FAILURE;
    req->failed.errno_if_err = errno;
    return;
  }
  mret_t _r = merry_graves_init_a_core(
      new_repr, args->create_core.new_core_type, args->create_core.st_addr);
  if (!_r) {
    req->res = GREQUEST_MERRY_FAILURE;
    req->failed.errno_if_err = errno;
    return;
  }
  merry_graves_give_IDs_to_cores(new_repr, grp);
  if (merry_graves_boot_a_core(new_repr) == RET_FAILURE) {
    req->res = GREQUEST_MERRY_FAILURE;
    req->failed.errno_if_err = errno;
    new_repr->base->deletec(new_repr->core);
    GRAVES.HOW_TO_DESTROY_BASE[args->create_core.new_core_type](new_repr->base);
    merry_graves_failed_core_booting();
    return;
  }
  req->res = GREQUEST_SUCCESS;
  args->create_core.gid = grp->group_id;
  args->create_core.new_id = new_repr->base->id;
  args->create_core.new_uid = new_repr->base->uid;
}

REQ(create_group) {
  /*
   * Just create a new group
   * */
  MerryRequestArgs *args = req->args;
  MerryGravesGroup *ngrp = merry_graves_add_group();
  if (!ngrp) {
    req->res = GREQUEST_MERRY_FAILURE;
    req->failed.errno_if_err = errno;
    return;
  }
  args->create_group.new_guid = ngrp->group_id;
  req->res = GREQUEST_SUCCESS;
}

REQ(get_group_details) {
  /*
   * Get the details of some group you want.
   * The results include the number of cores and
   * cores that are active in that group
   * */
  MerryRequestArgs *args = req->args;
  MerryGravesGroup **ngrp =
      merry_Group_list_at(GRAVES.GRPS, args->get_group_details.guid);
  if (!ngrp) {
    req->res = GREQUEST_MERRY_FAILURE;
    req->failed.errno_if_err = errno;
    return;
  }
  args->get_group_details.core_count = (*ngrp)->core_count;
  args->get_group_details.active_core_count = (*ngrp)->active_core_count;
  req->res = GREQUEST_SUCCESS;
}

REQ(get_system_details) {
  /*
   * Get system details but not what you think
   * You get the number of groups that are active,
   * the number of cores and the number of cores that
   * are currently active. These numbers are global and not
   * just one group.
   * */
  MerryRequestArgs *args = req->args;
  args->get_system_details.grp_count = GRAVES.grp_count;
  args->get_system_details.core_count = GRAVES.core_count;
  args->get_system_details.active_core_count = GRAVES.active_core_count;
  // will never fail so don't worry
  req->res = GREQUEST_SUCCESS;
}
