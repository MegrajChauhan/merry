#include <merry_graves.h>

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
    req->result.result = MRES_INVALID_ARGS;
    return;
  }
  MerryGravesCoreRepr *new_repr;
  mresult_t res = MRES_SUCCESS;
  if (args->create_core.new_group == mtrue &&
      args->create_core.same_group == mfalse) {
    // A new group needed
    MerryGravesGroup *ngrp;
    res = merry_graves_add_group(GRAVES, &ngrp);
    if (res != MRES_SUCCESS)
      goto CC_FAILURE;
    grp = ngrp;
  } else if (args->create_core.new_group == mfalse &&
             args->create_core.same_group == mfalse) {
    // use an exisiting group
    MerryGravesGroup *ogrp;
    res = merry_Group_list_at(GRAVES->GRPS, &ogrp, args->create_core.gid);
    if (res != MRES_SUCCESS)
      goto CC_FAILURE;
    grp = ogrp;
  } else if (args->create_core.same_group == mtrue) {
  } else {
    // None of the possibilities matches hence an error
    res = MRES_INVALID_ARGS;
    goto CC_FAILURE;
  }
  res = merry_graves_add_core(GRAVES, grp, &new_repr);
  if (res != MRES_SUCCESS)
    goto CC_FAILURE;
  res = merry_graves_init_a_core(GRAVES, new_repr,
                                 args->create_core.new_core_type,
                                 args->create_core.st_addr);
  if (res != MRES_SUCCESS)
    goto CC_FAILURE;
  merry_graves_give_IDs_to_cores(GRAVES, new_repr, grp);
  if ((res = merry_graves_boot_a_core(GRAVES, new_repr)) != MRES_SUCCESS) {
    merry_graves_failed_core_booting(GRAVES);
    goto CC_FAILURE;
  }
  args->create_core.gid = grp->group_id;
  args->create_core.new_id = new_repr->base->id;
  args->create_core.new_uid = new_repr->base->uid;
  res = MRES_SUCCESS;
CC_FAILURE:
  req->result.result = res;
  req->result.ERRNO = (res == MRES_SYS_FAILURE) ? errno : 0;
  if (res == MRES_FAILURE)
    req->result.ic_res = GRAVES->result.ic_res;
  return;
}

REQ(create_group) {
  /*
   * Just create a new group
   * */
  MerryRequestArgs *args = req->args;
  MerryGravesGroup *ngrp;
  mresult_t res = merry_graves_add_group(GRAVES, &ngrp);
  if (!ngrp) {
    req->result.result = res;
    if (res == MRES_SYS_FAILURE)
      req->result.ERRNO = errno;
    return;
  }
  args->create_group.new_guid = ngrp->group_id;
  req->result.result = MRES_SUCCESS;
}

REQ(get_group_details) {
  /*
   * Get the details of some group you want.
   * The results include the number of cores and
   * cores that are active in that group
   * */
  MerryRequestArgs *args = req->args;
  MerryGravesGroup *ngrp;
  mresult_t res =
      merry_Group_list_at(GRAVES->GRPS, &ngrp, args->get_group_details.guid);
  if (res != MRES_SUCCESS) {
    req->result.result = res;
    if (res == MRES_SYS_FAILURE)
      req->result.ERRNO = errno;
    return;
  }
  args->get_group_details.core_count = (ngrp)->core_count;
  args->get_group_details.active_core_count = (ngrp)->active_core_count;
  req->result.result = MRES_SUCCESS;
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
  args->get_system_details.grp_count = GRAVES->grp_count;
  args->get_system_details.core_count = GRAVES->core_count;
  args->get_system_details.active_core_count = GRAVES->active_core_count;
  // will never fail so don't worry
  req->result.result = MRES_SUCCESS;
}
