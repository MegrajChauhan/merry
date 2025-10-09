#include <test_core/tc/tc.h>

mptr_t tc_create_core(MerryCoreBase *base, maddress_t st) {
  TC *tc = (TC *)malloc(sizeof(TC));
  if (!tc) {
    MFATAL("TC", "Failed to allocate TC core", NULL);
    return RET_NULL;
  }
  tc->PC = st;
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
      if (base->terminate) {
        base->running = mfalse;
        tc_make_request(tc, KILL_SELF);
        tc_destroy_base(tc->base);
        break;
      }
      // other interrupts.....
      base->interrupt = mfalse;
    }
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
    default:
      MLOG("TC", "Unknown INSTRUCTION %u", curr);
    }
    tc->PC++;
  }
  return (_THRET_T_)0;
}

MerryCoreBase *tc_create_base() {
  MerryCoreBase *base = (MerryCoreBase *)malloc(sizeof(MerryCoreBase));
  // nort is not necessary for us right now so ignore
  // interfaces array also useless
  if (!base) {
    MFATAL("TC", "Failed to initialize core base", NULL);
    return RET_NULL;
  }
  if (merry_cond_init(&base->cond) == RET_FAILURE) {
    MFATAL("TC", "Failed to obtain condition variable", NULL);
    free(base);
    return RET_NULL;
  }
  base->createc = tc_create_core;
  base->deletec = tc_delete_core;
  base->execc = tc_run;
  base->getargs = tc_get_args;
  base->predel = tc_pre_delete_core;
  base->setinp = tc_set_inp;
  base->prepcore = tc_prep_core;
  base->share_resources = tc_share_resources;

  return base;
}

void tc_destroy_base(MerryCoreBase *base) {
  merry_check_ptr(base);
  merry_cond_destroy(&base->cond);
  free(base);
}

_MERRY_ALWAYS_INLINE_ MerryRequestArgs *tc_get_args(mptr_t c) {
  return &((TC *)c)->args;
}

void tc_pre_delete_core(mptr_t c) {
  // The main purpose of this function is to put the core in
  // a state suited for deletion.
  TC *tc = (TC *)c;
  tc->base->interrupt = mtrue;
  tc->base->terminate = mtrue;
  merry_cond_signal(&tc->base->cond); // if it is waiting
}

mret_t tc_set_inp(mptr_t c, mstr_t fname) {
  TC *tc = (TC *)c;
  return tc_read_input(fname, &tc->inp);
}

mret_t tc_prep_core(mptr_t c) {
  TC *tc = (TC *)c;
  tc->base->req_res = mtrue;
  tc->base->running = mfalse;
  tc->base->interrupt = mfalse;
  tc->base->terminate = mfalse;
  tc->_greq.guid = tc->base->guid;
  tc->_greq.id = tc->base->id;
  tc->_greq.uid = tc->base->uid;
  msize_t fsize = 0;
  merry_file_size(tc->inp.file, &fsize);
  tc->mem = tc_mem_init(tc->inp.mem, fsize);
  return (tc->mem == RET_NULL) ? RET_FAILURE : RET_SUCCESS;
}

mret_t tc_share_resources(mptr_t c1, mptr_t c2) { return RET_SUCCESS; }

void tc_make_request(TC *tc, mgreq_t req) {
  tc->_greq.type = req;
  merry_SEND_REQUEST(&tc->_greq, &tc->base->cond);
}
