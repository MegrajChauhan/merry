#include <merry_graves_groups.h>

MerryGravesGroup *merry_graves_group_create(msize_t gid) {
  MerryGravesGroup *grp = (MerryGravesGroup *)malloc(sizeof(MerryGravesGroup));

  if (!grp) {
    MERROR("Graves Group", "Failed to create a new group", NULL);
    return RET_NULL;
  }

  if ((grp->all_cores = merry_list_create(MerryGravesCoreRepr, 10)) ==
      RET_NULL) {
    MERROR("Graves Group", "Failed to create a new group", NULL);
    free(grp);
    return RET_NULL;
  }

  grp->core_count = 0;
  grp->active_core_count = 0;
  grp->group_id = gid;
  return grp;
}

MerryGravesCoreRepr *merry_graves_group_add_core(MerryGravesGroup *grp) {
  // Here we just create space for a new core and then let Graves
  // initialize it.
  merry_check_ptr(grp);
  merry_check_ptr(grp->all_cores);

  MerryGravesCoreRepr repr;
  repr.core = NULL;
  repr.base = NULL;
  if (merry_list_push(grp->all_cores, &repr) == RET_FAILURE) {
    // Not totally fatal
    if ((grp->all_cores = merry_list_resize(grp->all_cores, 2)) == RET_NULL)
      return RET_NULL;
  }
  grp->core_count++;
  return (MerryGravesCoreRepr *)merry_list_at(grp->all_cores,
                                              grp->core_count - 1);
}

void merry_graves_group_last_add_failed(MerryGravesGroup *grp) {
  merry_check_ptr(grp);
  merry_check_ptr(grp->all_cores);

  // The last core that was added wasn't successfully initialized.
  // KILL it
  merry_list_pop(grp->all_cores);
  grp->core_count--;
  return;
}

MerryGravesCoreRepr *merry_graves_group_find_dead_core(MerryGravesGroup *grp) {
  merry_check_ptr(grp);
  merry_check_ptr(grp->all_cores);

  for (msize_t i = 0; i < grp->core_count; i++) {
    MerryGravesCoreRepr *repr =
        (MerryGravesCoreRepr *)merry_list_at(grp->all_cores, i);
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
      (MerryGravesCoreRepr *)merry_list_at(grp->all_cores, id);
  if (!repr)
    return RET_NULL;
  if (!repr->core || repr->base->uid != uid) {
    *is_dead = mtrue;
    return RET_NULL;
  }
  return repr;
}

MerryGravesCoreRepr *merry_graves_group_get_core(MerryGravesGroup *grp,
                                                 msize_t id) {
  merry_check_ptr(grp);
  merry_check_ptr(grp->all_cores);
  return (MerryGravesCoreRepr *)merry_list_at(grp->all_cores, id);
}

void merry_graves_group_destroy(MerryGravesGroup *grp) {
  // We delete a group and delete all cores too!
  // This is a risky operation so Graves will have to, by default,
  // terminate all cores belonging to this group
  merry_check_ptr(grp);
  merry_check_ptr(grp->all_cores);

  merry_list_destroy(grp->all_cores);
  free(grp);
}
