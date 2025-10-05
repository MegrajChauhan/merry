#ifndef _MERRY_TEST_CORE_
#define _MERRY_TEST_CORE_

#include <merry_config.h>
#include <merry_graves_core_base.h>
#include <merry_hord.h>
#include <merry_hord_task.h>
#include <merry_nort.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdlib.h>

typedef struct MerryTestCore MerryTestCore;

struct MerryTestCore {
  MerryCoreBase *base;
  msize_t PC;
};

mptr_t merry_test_core_create(MerryCoreBase *base, maddress_t st_addr,
                              MerryErrorStack *st);

void merry_test_core_delete(mptr_t core);

_THRET_T_ merry_test_core_run(mptr_t c);

MerryCoreBase *merry_test_core_base_create(MerryErrorStack *st);

void merry_test_core_base_delete(MerryCoreBase *base);

MerryRequestArgs *merry_test_core_get_args(mptr_t c);

void merry_test_core_prepare_for_deletion(mptr_t c);

#endif
