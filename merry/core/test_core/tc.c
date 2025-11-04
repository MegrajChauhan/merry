#include <test_core/tc/tc.h>

mptr_t tc_create_core(MerryCoreBase *base, maddress_t st) {
  TC *tc = (TC *)malloc(sizeof(TC));
  if (!tc) {
    MFATAL("TC", "Failed to allocate TC core", NULL);
    return RET_NULL;
  }
  tc->PC = st;
  tc->base = base;
  tc->terminate = mfalse;
  return tc;
}

void tc_delete_core(mptr_t c) {
  TC *tc = (TC *)c;
  tc_mem_destroy(tc->mem);
  tc_destroy_input(&tc->inp);
  free(tc);
}

_THRET_T_ tc_run(mptr_t c) {
  TC *tc = (TC *)c;
  MerryCoreBase *base = tc->base;
  base->running = mtrue;
  mbyte_t curr;
  while (1) {
    if (atomic_load_explicit((_Atomic mbool_t *)&base->interrupt,
                             memory_order_relaxed)) {
      if (tc->terminate) {
        base->running = mfalse;
        tc_destroy_base(tc->base);
        tc_delete_core(c);
        break;
      }
      base->interrupt = mfalse;
    }
    // other interrupts.....
    if (tc_read(tc->mem, tc->PC, &curr) == RET_FAILURE) {
      // Done
      MLOG("TC", "End of accessible memory reached", NULL);
      base->running = mfalse;
      tc_make_request(tc, KILL_SELF);
      tc_destroy_base(tc->base);
      break;
    }
    switch (curr) {
    case TEST_1:
      tc_TEST_1(tc);
      break;
    case TEST_2:
      tc_TEST_2(tc);
      break;
    case TEST_3:
      tc_TEST_3(tc);
      break;
    case TEST_4:
      tc_TEST_4(tc);
      break;
    case TEST_5:
      tc_TEST_5(tc);
      break;
    case TEST_6:
      tc_TEST_6(tc);
      break;
    case TEST_7:
      tc_TEST_7(tc);
      break;
    case TEST_8:
      tc_TEST_8(tc);
      break;
    case TEST_9:
      tc_TEST_9(tc);
      break;
    default:
      MLOG("TC", "[ID=%zu, UID=%zu, GUID=%zu] Unknown INSTRUCTION %u", base->id,
           base->uid, base->guid, curr);
    }
    tc->PC++;
  }
  return (_THRET_T_)0;
}

MerryCoreBase *tc_create_base() {
  MerryCoreBase *base = (MerryCoreBase *)malloc(sizeof(MerryCoreBase));
  if (!base) {
    MFATAL("TC", "Failed to initialize core base", NULL);
    return RET_NULL;
  }
  if (merry_cond_init(&base->cond) == RET_FAILURE) {
    MFATAL("TC", "Failed to obtain condition variable", NULL);
    free(base);
    return RET_NULL;
  }
  base->type = __TEST_CORE;
  base->createc = tc_create_core;
  base->deletec = tc_delete_core;
  base->execc = tc_run;
  base->predel = tc_pre_delete_core;
  base->setinp = tc_set_inp;
  base->prepcore = tc_prep_core;

  return base;
}

void tc_destroy_base(MerryCoreBase *base) {
  merry_check_ptr(base);
  merry_cond_destroy(&base->cond);
  free(base);
}

void tc_pre_delete_core(mptr_t c) {
  // The main purpose of this function is to put the core in
  // a state suited for deletion.
  TC *tc = (TC *)c;
  tc->base->interrupt = mtrue;
  tc->terminate = mtrue;
  merry_cond_signal(&tc->base->cond); // if it is waiting
}

mret_t tc_set_inp(mptr_t c, mstr_t fname) {
  TC *tc = (TC *)c;
  return tc_read_input(fname, &tc->inp);
}

mret_t tc_prep_core(mptr_t c) {
  TC *tc = (TC *)c;
  tc->base->running = mfalse;
  tc->base->interrupt = mfalse;
  tc->terminate = mfalse;
  tc->_greq.base = tc->base;
  tc->_greq.used_cond = &tc->base->cond;
  tc->_greq.args = &tc->args;
  msize_t fsize = 0;
  merry_file_size(tc->inp.file, &fsize);
  tc->mem = tc_mem_init(tc->inp.mem, fsize);
  return (tc->mem == RET_NULL) ? RET_FAILURE : RET_SUCCESS;
}

void tc_make_request(TC *tc, mgreq_t req) {
  tc->_greq.type = req;
  merry_SEND_REQUEST(&tc->_greq);
}
