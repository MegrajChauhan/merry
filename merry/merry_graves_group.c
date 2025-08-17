#include <merry_graves_groups.h>

MerryGravesGroup *merry_graves_group_create(msize_t gid, MerryErrorStack *st) {
  MerryGravesGroup *grp = (MerryGravesGroup *)malloc(sizeof(MerryGravesGroup));

  if (!grp) {
    PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
         "Creating a New Group");
    merry_error_stack_errno(st);
    merry_error_stack_fatality(st);
    return RET_NULL;
  }

  if ((grp->all_cores = merry_create_dynamic_list(
           1, sizeof(MerryGravesCoreRepr), st)) == RET_NULL) {
    PUSH(st, NULL, "Failed to initialize group core buffer",
         "Creating a new Group");
    free(grp);
    return RET_NULL;
  }

  grp->core_count = 0;
  grp->group_id = gid;
  return grp;
}

MerryGravesCoreRepr *merry_graves_group_add_core(MerryGravesGroup *grp,
                                                 MerryErrorStack *st) {
  // Here we just create space for a new core and then let Graves
  // initialize it.
  merry_check_ptr(grp);
  merry_check_ptr(grp->all_cores);

  MerryGravesCoreRepr repr;
  repr.core = NULL;
  repr.base = NULL;
  if (merry_dynamic_list_push(grp->all_cores, &repr, st) == RET_FAILURE) {
    // Not totally fatal
    PUSH(st, NULL, "Failed to add a new core(UNINIT)",
         "Couldn't create space for a new core");
    return RET_NULL;
  }
  return (MerryGravesCoreRepr *)merry_dynamic_list_at(grp->all_cores,
                                                      grp->core_count - 1);
}

void merry_graves_group_last_add_failed(MerryGravesGroup *grp) {
  merry_check_ptr(grp);
  merry_check_ptr(grp->all_cores);

  // The last core that was added wasn't successfully initialized.
  // KILL it
  merry_dynamic_list_pop(grp->all_cores);
  return;
}

MerryGravesCoreRepr *merry_graves_group_find_dead_core(MerryGravesGroup *grp) {
  merry_check_ptr(grp);
  merry_check_ptr(grp->all_cores);

  for (msize_t i = 0; i < merry_dynamic_list_size(grp->all_cores); i++) {
    MerryGravesCoreRepr *repr =
        (MerryGravesCoreRepr *)merry_dynamic_list_at(grp->all_cores, i);
    if (!repr)
      merry_unreachable("Searching for dead core led to NULL core", NULL);
    if (!repr->core)
      return repr;
  }
  return RET_NULL;
}

MerryGravesCoreRepr *merry_graves_group_find_core(MerryGravesGroup *grp,
                                                  msize_t uid, msize_t id,
                                                  mbool_t *is_dead) {
  // Find the core and return it
  // if it is dead, return NULL and set is_dead to mtrue
  merry_check_ptr(grp);
  merry_check_ptr(grp->all_cores);
  merry_check_ptr(is_dead);

  // "id" basically refers to the core's index into the group's
  // dynamic array
  MerryGravesCoreRepr *repr =
      (MerryGravesCoreRepr *)merry_dynamic_list_at(grp->all_cores, id);
  if (!repr)
    merry_unreachable("Searching for dead core led to NULL core", NULL);
  if (!repr->core || repr->base->uid != uid) {
    *is_dead = mtrue;
    return RET_NULL;
  }
  return repr;
}

void merry_graves_group_destroy(MerryGravesGroup *grp) {
  // We delete a group and delete all cores too!
  // This is a risky operation so Graves will have to, by default,
  // terminate all cores belonging to this group
  merry_check_ptr(grp);
  merry_check_ptr(grp->all_cores);

  for (msize_t i = 0; i < grp->core_count; i++) {
    MerryGravesCoreRepr *repr =
        (MerryGravesCoreRepr *)merry_dynamic_list_at(grp->all_cores, i);
    if (!repr)
      merry_unreachable("NULL core in the group???", NULL);

    /*
     * Graves will force all member cores of the group to terminate
     * before the group is deleted and hence the "core" member of
     * every representation will be NULL
     * */
    if (repr->core)
      merry_unreachable("Alive core in a to-be-dead group", NULL);
  }

  free(grp->all_cores);
  free(grp);
}
