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
}

REQ(create_group) {
  /*
   * How do we pass on the results to the requester?
   * */
}
