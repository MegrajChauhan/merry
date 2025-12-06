#ifndef _MERRY_GRAVES_LAUNCHER_
#define _MERRY_GRAVES_LAUNCHER_

#include <merry_graves.h>

_MERRY_INTERNAL_ MerryGraves *GRAVES;

void merry_graves_launcher_set(MerryGraves *G);

_THRET_T_ merry_graves_launcher(mptr_t r);

#endif
