#include <merry_graves.h>

REQ(create_core) {
  /*
   * Creating a new core:
   * - Either create a new core in the same group
   * - Or create a new core in a new group
   * - Or create a new core in an exisiting group
   * */
  MerryRequestArgs *args = &req->args;
  MerryGravesGroup *grp;
  MerryCoreRepr *new_repr;
  if (args->create_core.new_core_type >= __CORE_TYPE_COUNT) {
    // Locally fatal i.e fatal to the core who will dump its
    // error messages and then terminate
    req->result = MRES_INVALID_ARGS;
    return;
  }
  mresult_t res = MRES_SUCCESS;
  grp = merry_graves_get_group(args->create_core.gid);
  if (!grp) {
	  res  =MRES_INVALID_ARGS;
      goto CC_FAILURE;
  }
  res = merry_graves_add_core(grp, &new_repr);
  if (res != MRES_SUCCESS)
    goto CC_FAILURE;
  res = merry_graves_init_a_core(new_repr,
                                 args->create_core.new_core_type,
                                 args->create_core.st_addr);
  if (res != MRES_SUCCESS)
    goto CC_FAILURE;
  merry_graves_give_IDs_to_cores(new_repr, grp);
  if ((res = merry_graves_boot_a_core(new_repr)) != MRES_SUCCESS) {
    goto CC_FAILURE;
  }
  args->create_core.new_id = new_repr->metadata.iden.id;
  args->create_core.new_uid = new_repr->metadata.iden.uid;
  res = MRES_SUCCESS;
CC_FAILURE:
  req->result = res;
  if (res == MRES_SYS_FAILURE) 
  	req->err = errno;
}

REQ(create_group) {
  /*
   * Just create a new group
   * */
  MerryRequestArgs *args = &req->args;
  MerryGravesGroup *ngrp;
  mresult_t res = merry_graves_add_group(&ngrp);
  if (!ngrp) {
    req->result = res;
    if (res == MRES_SYS_FAILURE)
      req->err = errno;
    return;
  }
  args->create_group.new_guid = ngrp->group_id;
  req->result = MRES_SUCCESS;
}

// REQ(get_system_details) {
//   /*
//    * Get system details but not what you think
//    * You get the number of groups that are active,
//    * the number of cores and the number of cores that
//    * are currently active. These numbers are global and not
//    * just one group.
//    * */
//   MerryRequestArgs *args = req->args;
//   args->get_system_details.grp_count = GRAVES->grp_count;
//   args->get_system_details.core_count = GRAVES->core_count;
//   args->get_system_details.active_core_count = GRAVES->active_core_count;
//   // will never fail so don't worry
//   req->result = MRES_SUCCESS;
// }
