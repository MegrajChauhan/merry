#include <merry_graves.h>

REQ(kill_self) {
  /*
   * This process involves:
   * - Commanding the destruction of its core
   * - Declare dead
   * - Destroy the base
   * - Register the death
   * */
  repr->base->predel(repr->core);
  repr->base->deletec(repr->core);
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
  MerryRequestArgs *args = repr->base->getargs(repr->core);
  if (args->create_core.new_core_type >= __CORE_TYPE_COUNT) {
    // Locally fatal i.e fatal to the core who will dump its
    // error messages and then terminate
    repr->base->req_res = mfalse;
    return;
  }
  MerryGravesCoreRepr *new_repr;
  if (args->create_core.new_group == mtrue &&
      args->create_core.same_group == mfalse) {
    // A new group needed
    MerryGravesGroup *ngrp = merry_graves_add_group();
    if (!ngrp) {
      repr->base->req_res = mfalse;
      return;
    }
    grp = ngrp;
  } else if (args->create_core.new_group == args->create_core.same_group ==
             mfalse) {
    // use an exisiting group
    MerryGravesGroup **ogrp = merry_list_at(GRAVES.GRPS, args->create_core.gid);
    if (!ogrp) {
      repr->base->req_res = mfalse;
      return;
    }
    grp = *ogrp;
  } else if (args->create_core.same_group == mtrue) {
  } else {
    // None of the possibilities matches hence an error
    repr->base->req_res = mfalse;
    return;
  }
  new_repr = merry_graves_add_core(grp);
  if (!new_repr) {
    repr->base->req_res = mfalse;
    return;
  }
  mret_t _r;
  if (args->create_core.share_resources)
    _r = merry_graves_init_a_core_no_prep(
        new_repr, args->create_core.new_core_type, args->create_core.st_addr);
  else
    _r = merry_graves_init_a_core(new_repr, args->create_core.new_core_type,
                                  args->create_core.st_addr);
  if (!_r) {
    repr->base->req_res = mfalse;
    return;
  }
  if (args->create_core.share_resources) {
    if (repr->base->share_resources(repr->core, new_repr->core) ==
        RET_FAILURE) {
      new_repr->base->deletec(new_repr->core);
      GRAVES.HOW_TO_DESTROY_BASE[args->create_core.new_core_type](
          new_repr->base);
      repr->base->req_res = mfalse;
      return;
    }
  }
  merry_graves_give_IDs_to_cores(new_repr, grp);
  if (merry_graves_boot_a_core(new_repr) == RET_FAILURE) {
    new_repr->base->deletec(new_repr->core);
    GRAVES.HOW_TO_DESTROY_BASE[args->create_core.new_core_type](new_repr->base);
    repr->base->req_res = mfalse;
    return;
  }
  repr->base->req_res = mtrue; // success
  args->create_core.gid = grp->group_id;
  args->create_core.new_id = new_repr->base->id;
  args->create_core.new_uid = new_repr->base->uid;
}

REQ(create_group) {
  /*
   * Just create a new group
   * */
  MerryRequestArgs *args = repr->base->getargs(repr->core);
  MerryGravesGroup *ngrp = merry_graves_add_group();
  if (!ngrp) {
    repr->base->req_res = mfalse;
    return;
  }
  args->create_group.new_guid = ngrp->group_id;
  repr->base->req_res = mtrue; // success
}

REQ(get_group_details) {
  /*
   * Get the details of some group you want.
   * The results include the number of cores and
   * cores that are active in that group
   * */
  MerryRequestArgs *args = repr->base->getargs(repr->core);
  MerryGravesGroup **ngrp = merry_list_at(GRAVES.GRPS, repr->base->guid);
  if (!ngrp) {
    repr->base->req_res = mfalse;
    return;
  }
  args->get_group_details.core_count = (*ngrp)->core_count;
  args->get_group_details.active_core_count = (*ngrp)->active_core_count;
  repr->base->req_res = mtrue; // success
}

REQ(get_system_details) {
  /*
   * Get system details but not what you think
   * You get the number of groups that are active,
   * the number of cores and the number of cores that
   * are currently active. These numbers are global and not
   * just one group.
   * */
  MerryRequestArgs *args = repr->base->getargs(repr->core);
  args->get_system_details.grp_count = GRAVES.grp_count;
  args->get_system_details.core_count = GRAVES.core_count;
  args->get_system_details.active_core_count = GRAVES.active_core_count;
  // will never fail so don't worry
}
