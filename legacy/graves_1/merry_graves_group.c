#include <merry_graves_groups.h>

_MERRY_DEFINE_STATIC_LIST_(CoreRepr, MerryGravesCoreRepr*);

mresult_t merry_graves_group_create(MerryGravesGroup **grp, msize_t gid) {
  *grp = (MerryGravesGroup *)malloc(sizeof(MerryGravesGroup));

  if (!(*grp)) {
    return MRES_SYS_FAILURE;
  }

  mresult_t res;
  if ((res = merry_CoreRepr_list_create(10, &(*grp)->all_cores)) !=
      MRES_SUCCESS) {
    free(*grp);
    return res;
  }

  (*grp)->core_count = 0;
  (*grp)->active_core_count = 0;
  (*grp)->group_id = gid;
  return MRES_SUCCESS;
}

mresult_t merry_graves_group_add_core(MerryGravesGroup *grp,
                                      MerryGravesCoreRepr *core) {
  // Here we just create space for a new core and then let Graves
  // initialize it.
  merry_check_ptr(grp);
  merry_check_ptr(grp->all_cores);
  merry_check_ptr(core);

  if (merry_CoreRepr_list_push(grp->all_cores, &core) != MRES_SUCCESS) {
    // Not totally fatal
    mresult_t resl;
    if ((resl = merry_CoreRepr_list_resize(grp->all_cores, 2)) != MRES_SUCCESS)
      return resl;
    merry_CoreRepr_list_push(grp->all_cores, &core);
  }
  grp->core_count++;
  return MRES_SUCCESS;
}

mresult_t merry_graves_group_find_core(MerryGravesGroup *grp,
                                       MerryGravesCoreRepr **res, msize_t uid,
                                       msize_t id) {
  // Find the core and return it
  // if it is dead, return NULL and set is_dead to mtrue
  merry_check_ptr(grp);
  merry_check_ptr(grp->all_cores);

  // "id" basically refers to the core's index into the group's
  // dynamic array
  MerryGravesCoreRepr *repr;
  mresult_t result = merry_CoreRepr_list_at(grp->all_cores, &repr, id);
  if (result != MRES_SUCCESS)
    return MRES_INVALID_ARGS;
  if ((repr)->dead)
    return MRES_NOT_EXISTS;
  if ((repr)->base->uid != uid) {
    return MRES_NOT_FULFILLED;
  }
  *res = repr;
  return MRES_SUCCESS;
}

MerryGravesCoreRepr *merry_graves_group_find_dead_core(MerryGravesGroup *grp) {
  merry_check_ptr(grp);
  merry_check_ptr(grp->all_cores);

  for (msize_t i = 0; i < grp->core_count; i++) {
    if (grp->all_cores->buf[i]->dead)
      return grp->all_cores->buf[i];
  }
  return RET_NULL;
}

mresult_t merry_graves_group_get_core(MerryGravesGroup *grp,
                                      MerryGravesCoreRepr **res, msize_t id) {
  merry_check_ptr(grp);
  merry_check_ptr(grp->all_cores);
  *res = grp->all_cores->buf[id];
  return MRES_SUCCESS;
}

void merry_graves_group_destroy(MerryGravesGroup *grp) {
  // We delete a group and delete all cores too!
  // This is a risky operation so Graves will have to, by default,
  // terminate all cores belonging to this group
  merry_check_ptr(grp);
  merry_check_ptr(grp->all_cores);

  for (msize_t i = 0; i < grp->core_count; i++) {
     free(grp->all_cores->buf[i]);
  }
  
  merry_CoreRepr_list_destroy(grp->all_cores);
  free(grp);
}
