#include <merry_graves.h>

REQ(kill_self) {
  /*
   * This process involves:
   * - Commanding the destruction of its core
   * - Declare dead
   * - Destroy the base
   * - Register the death
   * */
  repr->base->deletec(repr->core);
  repr->core = NULL; // Declare dead
  GRAVES.HOW_TO_DESTROY_BASE[repr->base->type](repr->base);
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
  MerryErrorStack *st = &repr->base->estack;
  if (args->create_core.new_core_type >= __CORE_TYPE_COUNT) {
    // Locally fatal i.e fatal to the core who will dump its
    // error messages and then terminate
    PUSH(st, "Invalid Core Type", "Invalid core type provided for new core",
         "Creating New Core");
    merry_error_stack_fatality(st);
    repr->base->req_res = mfalse;
    return;
  }
  MerryGravesCoreRepr *new_repr;
  if (args->create_core.new_group == mtrue &&
      args->create_core.same_group == mfalse) {
    // A new group needed
    MerryGravesGroup *ngrp = merry_graves_add_group(st);
    if (!ngrp) {
      PUSH(st, NULL, "Failed to create a new GROUP for a new core",
           "Creating New Core");
      repr->base->req_res = mfalse;
      return;
    }
    grp = ngrp;
  } else if (args->create_core.new_group == args->create_core.same_group ==
             mfalse) {
    // use an exisiting group
    MerryGravesGroup *ogrp =
        merry_dynamic_list_at(GRAVES.GRPS, args->create_core.gid);
    if (!ogrp) {
      PUSH(st, NULL, "Invalid GUID given to create a new core",
           "Creating New Core");
      repr->base->req_res = mfalse;
      return;
    }
    grp = ogrp;
  } else if (args->create_core.same_group == mtrue) {
  } else {
    // None of the possibilities matches hence an error
    PUSH(st, "Unknown Possibility", "None of the cases matched",
         "Creating New Core");
    repr->base->req_res = mfalse;
    return;
  }
  new_repr = merry_graves_add_core(grp, st);
  if (!new_repr) {
    PUSH(st, NULL, "Failed to add a new core", "Creating New Core");
    repr->base->req_res = mfalse;
    return;
  }
  if (merry_graves_init_a_core(new_repr, args->create_core.new_core_type,
                               args->create_core.st_addr, st) == RET_FAILURE) {
    PUSH(st, NULL, "New Core Initialization Failed", "Creating New Core");
    repr->base->req_res = mfalse;
    return;
  }
  merry_graves_give_IDs_to_cores(new_repr, grp);
  if (merry_graves_boot_a_core(new_repr, st) == RET_FAILURE) {
    PUSH(st, NULL, "New Core BOOTING Failed", "Creating New Core");
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
  MerryErrorStack *st = &repr->base->estack;
  MerryGravesGroup *ngrp = merry_graves_add_group(st);
  if (!ngrp) {
    PUSH(st, NULL, "Failed to create a new GROUP", "Creating New Group");
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
  MerryErrorStack *st = &repr->base->estack;
  MerryGravesGroup *ngrp = merry_dynamic_list_at(GRAVES.GRPS, repr->base->guid);
  if (!ngrp) {
    PUSH(st, "Invalid GUID", "Group Details Requested but GUID doesn't exisi",
         "Get Group Details");
    repr->base->req_res = mfalse;
    return;
  }
  args->get_group_details.core_count = ngrp->core_count;
  args->get_group_details.active_core_count = ngrp->active_core_count;
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
  args->get_system_details.grp_count = merry_dynamic_list_size(GRAVES.GRPS);
  args->get_system_details.core_count = GRAVES.core_count;
  args->get_system_details.active_core_count = GRAVES.active_core_count;
  // will never fail so don't worry
}
