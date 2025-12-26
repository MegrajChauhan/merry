#include <merry_graves_launcher.h>

void merry_graves_launcher_set(MerryGraves *G) {
  if (GRAVES) {
    MFATAL("---", "Don't try to USE INTERNAL THINGS!", NULL);
    merry_unreachable();
  }
  GRAVES = G;
}

_THRET_T_ merry_graves_launcher(mptr_t r) {
  if (!r) {
    MFATAL("GRAVES: Launcher", "Invalid Core Launched!!!", NULL);
    return RET_NULL;
  }
  MerryGravesCoreRepr *repr = (MerryGravesCoreRepr*)r;

  // Obtain the latest details
  merry_mutex_lock(&GRAVES->graves_lock);
  MerryGravesGroup *grp = GRAVES->GRPS->buf[base->guid];
  mptr_t core = repr->core;
  merry_mutex_unlock(&GRAVES->graves_lock);

  msize_t ret = base->execc(core);
  MNOTE("GRAVES: Launcher",
        "CORE[ID=%zu, UID=%zu, GUID=%zu] Terminated with return code %zu",
        repr->id, repr->uid, repr->guid, ret);

  // Obtain the latest details
  merry_mutex_lock(&GRAVES->graves_lock);
  GRAVES->active_core_count--;
  merry_cond_signal(&GRAVES->graves_cond); // if GRAVES is sleeping
  base->predel(repr->core);
  base->deletec(repr->core);
  repr->dead = mtrue;
  GRAVES->HOW_TO_DESTROY_BASE[repr->type](base);
  merry_mutex_unlock(&GRAVES->graves_lock);

  return RET_NULL;
}
