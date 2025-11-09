#include <merry_graves_launcher.h>

void merry_graves_launcher_set(MerryGraves *G) {
  if (GRAVES) {
    MFATAL("---", "Don't try to USE INTERNAL THINGS!", NULL);
    merry_unreachable();
  }
  GRAVES = G;
}

_THRET_T_ merry_graves_launcher(mptr_t b) {
  if (!b) {
    MFATAL("GRAVES: Launcher", "Invalid Core Launched!!!", NULL);
    return RET_NULL;
  }
  MerryCoreBase *base = b;

  // Obtain the latest details
  merry_mutex_lock(&GRAVES->graves_lock);
  MerryGravesGroup *grp = GRAVES->GRPS->buf[base->guid];
  MerryGravesCoreRepr *repr;
  mresult_t res = merry_graves_group_find_core(grp, &repr, base->uid, base->id);

  mptr_t core = repr->core;
  merry_graves_group_register_new_core(grp);
  GRAVES->active_core_count++;
  if (GRAVES->active_core_count == 1)
    merry_cond_signal(&GRAVES->graves_cond);

  merry_mutex_unlock(&GRAVES->graves_lock);

  if (base->execc(core) == RET_FAILURE) {
    MFATAL("GRAVES: Launcher",
           "CORE[ID=%zu, UID=%zu, GUID=%zu] Terminated unsuccessfully",
           base->id, base->uid, base->guid);
  }

  // Obtain the latest details
  merry_mutex_lock(&GRAVES->graves_lock);
  grp = GRAVES->GRPS->buf[base->guid];
  res = merry_graves_group_find_core(grp, &repr, base->uid, base->id);
  GRAVES->active_core_count--;
  merry_cond_signal(&GRAVES->graves_cond); // if GRAVES is sleeping
  base->predel(repr->core);
  base->deletec(repr->core);
  repr->core = NULL;
  GRAVES->HOW_TO_DESTROY_BASE[base->type](base);
  merry_graves_group_register_dead_core(grp);
  merry_mutex_unlock(&GRAVES->graves_lock);

  return RET_NULL;
}
