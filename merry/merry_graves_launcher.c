#include <merry_graves_launcher.h>

_THRET_T_ merry_graves_launcher(mptr_t b) {
  if (!b) {
    MFATAL("GRAVES: Launcher", "Invalid Core Launched!!!", NULL);
    return RET_NULL;
  }
  MerryCoreBase *base = b;
  mbool_t is_dead_tmp;

  // Obtain the latest details
  merry_mutex_lock(&GRAVES.graves_lock);

  MerryGravesGroup **grp = merry_Group_list_at(GRAVES.GRPS, base->guid);

  MerryGravesCoreRepr *repr =
      merry_graves_group_find_core(*grp, base->uid, base->id, &is_dead_tmp);
  mptr_t core = repr->core;

  merry_graves_group_register_new_core(*grp);
  GRAVES.active_core_count++;
  merry_mutex_unlock(&GRAVES.graves_lock);

  if (base->execc(core) == RET_FAILURE) {
    MFATAL("GRAVES: Launcher",
           "CORE[ID=%zu, UID=%zu, GUID=%zu] Terminated unsuccessfully",
           base->id, base->uid, base->guid);
  }

  // Obtain the latest details
  merry_mutex_lock(&GRAVES.graves_lock);
  repr = merry_graves_group_find_core(*grp, base->uid, base->id, &is_dead_tmp);
  GRAVES.active_core_count--;
  merry_cond_signal(&GRAVES.graves_cond); // if GRAVES is sleeping
  base->predel(repr->core);
  base->deletec(repr->core);
  GRAVES.HOW_TO_DESTROY_BASE[base->type](base);
  repr->core = NULL;
  merry_graves_group_register_dead_core(*grp);
  merry_mutex_unlock(&GRAVES.graves_lock);

  return RET_NULL;
}
