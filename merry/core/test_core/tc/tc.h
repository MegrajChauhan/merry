#ifndef _TC_
#define _TC_

#include <merry_graves_core_base.h>
#include <merry_graves_request_queue.h>
#include <stdlib.h>
#include <test_core/comps/test_core_input.h>
#include <test_core/comps/test_core_memory.h>
#include <test_core/defs/test_core_instructions.h>

typedef struct TC TC;

struct TC {
  TCMem *mem;
  TCInp inp;
  maddress_t PC;
  MerryCoreBase *base;
  MerryGravesRequest _greq;
  MerryRequestArgs args;
  mbool_t req_failed;
};

mptr_t tc_create_core(MerryCoreBase *base, maddress_t st);

void tc_delete_core(mptr_t c);

_THRET_T_ tc_run(mptr_t c);

MerryCoreBase *tc_create_base();

void tc_destroy_base(MerryCoreBase *base);

MerryRequestArgs *tc_get_args(mptr_t c);

void tc_pre_delete_core(mptr_t c);

mret_t tc_set_inp(mptr_t c, mstr_t fname);

mret_t tc_prep_core(mptr_t c);

void tc_make_request(TC *tc, mgreq_t req);

void tc_TEST_1(TC *tc);
void tc_TEST_2(TC *tc);
void tc_TEST_3(TC *tc);
void tc_TEST_4(TC *tc);
void tc_TEST_5(TC *tc);
void tc_TEST_6(TC *tc);
void tc_TEST_7(TC *tc);
void tc_TEST_8(TC *tc);
void tc_TEST_9(TC *tc);
void tc_TEST_10(TC *tc);
void tc_TEST_11(TC *tc);
void tc_TEST_12(TC *tc);
void tc_TEST_13(TC *tc);
void tc_TEST_14(TC *tc);
void tc_TEST_15(TC *tc);
void tc_TEST_16(TC *tc);
void tc_TEST_17(TC *tc);
void tc_TEST_18(TC *tc);
void tc_TEST_19(TC *tc);
void tc_TEST_20(TC *tc);

#endif
