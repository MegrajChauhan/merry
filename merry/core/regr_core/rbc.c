#include <internal/rbc.h>
#include <internal/rbc_instruction_handler.h>

_MERRY_DEFINE_STATIC_LIST_(RBCThread, mthread_t);
_MERRY_DEFINE_STATIC_LIST_(Interface, MerryInterface *);
_MERRY_DEFINE_STACK_(RBCProcFrame, RBCStackFrame);

mptr_t rbc_master_core_create(MerryCoreBase *base, maddress_t st_addr) {
  merry_check_ptr(base);
  RBCMasterCore *core = (RBCMasterCore *)malloc(sizeof(RBCMasterCore));
  if (!core) {
    MFATAL("RBC", "Failed to allocate memory for the master core", NULL);
    return RET_NULL;
  }

  core->base = base;

  if ((core->rbc_cbase.child_threads = merry_RBCThread_list_create(10)) ==
      RET_NULL) {
    MFATAL("RBC", "Failed to allocate memory for a component", NULL);
    free(core);
    return RET_NULL;
  }

  if ((core->rbc_cbase.interfaces = merry_Interface_list_create(10)) ==
      RET_NULL) {
    MFATAL("RBC", "Failed to allocate memory for a component", NULL);
    merry_RBCThread_list_destroy(core->rbc_cbase.child_threads);
    free(core);
    return RET_NULL;
  }

  core->rbc_cbase.PC = st_addr;

  if ((core->rbc_cbase.stack = merry_get_anonymous_memory(_RBC_STACK_LEN_)) ==
      RET_NULL) {
    MFATAL("RBC", "Failed to initialize the stack", NULL);
    merry_RBCThread_list_destroy(core->rbc_cbase.child_threads);
    merry_Interface_list_destroy(core->rbc_cbase.interfaces);
    free(core);
    return RET_NULL;
  }

  core->rbc_cbase.SP = 0;
  core->rbc_cbase.BP = 0;

  if ((core->rbc_cbase.stack_frames =
           merry_RBCProcFrame_stack_init(_RBC_CALL_DEPTH_)) == RET_NULL) {
    MFATAL("RBC", "Failed to initialize the stack", NULL);
    merry_RBCThread_list_destroy(core->rbc_cbase.child_threads);
    merry_Interface_list_destroy(core->rbc_cbase.interfaces);
    merry_return_memory(core->rbc_cbase.stack, _RBC_STACK_LEN_);
    free(core);
    return RET_NULL;
  }

  if (merry_cond_init(&core->local_shared_cond) == RET_FAILURE) {
    MFATAL("RBC", "Failed to initialize component", NULL);
    merry_RBCThread_list_destroy(core->rbc_cbase.child_threads);
    merry_Interface_list_destroy(core->rbc_cbase.interfaces);
    merry_return_memory(core->rbc_cbase.stack, _RBC_STACK_LEN_);
    merry_RBCProcFrame_stack_destroy(core->rbc_cbase.stack_frames);
    free(core);
    return RET_NULL;
  }

  return (mptr_t)core;
}

void rbc_master_core_destroy(mptr_t c) {
  merry_check_ptr(c);
  RBCMasterCore *core = (RBCMasterCore *)c;

  if (!core->rbc_cbase.child_threads)
    merry_RBCThread_list_destroy(core->rbc_cbase.child_threads);

  if (!core->rbc_cbase.interfaces)
    merry_Interface_list_destroy(core->rbc_cbase.interfaces);

  if (!core->rbc_cbase.stack_frames)
    merry_RBCProcFrame_stack_destroy(core->rbc_cbase.stack_frames);

  if (!core->rbc_cbase.iram)
    rbc_memory_destroy(core->rbc_cbase.iram);

  if (!core->rbc_cbase.dram)
    rbc_memory_destroy(core->rbc_cbase.dram);

  if (!core->inp)
    rbc_input_destroy(core->inp);

  if (!core->rbc_cbase.stack)
    merry_return_memory(core->rbc_cbase.stack, _RBC_STACK_LEN_);

  merry_cond_destroy(&core->local_shared_cond);
  free(core);
}

_THRET_T_ rbc_master_core_run(mptr_t c) {
  merry_check_ptr(c);

  RBCMasterCore *core = (RBCMasterCore *)c;
  register MerryCoreBase *base = core->base;
  RBCCoreBase cbase = core->rbc_cbase;
  MerryHostMemLayout layout;

  while (mtrue) {
    if (!surelyF(cbase.check_after)) {
      if (core->kill_core)
        break;
      if (surelyF(atomic_load_explicit((_Atomic mbool_t *)&base->interrupt,
                                       memory_order_relaxed))) {
        if (cbase.terminate)
          break; // break
        // interrupts....(coming soon...)
        base->interrupt = mfalse;
      }
      cbase.check_after = 3;
    }
    if (surelyF(
            rbc_memory_read_qword(cbase.iram, cbase.PC, &layout.whole_word) ==
            RBC_MEM_OPER_ACCESS_INVALID)) {
      MFATAL(
          "RBC",
          "Memory access invalid: Accessing address that doesn't exist PC=%zu",
          cbase.PC);
      cbase.terminate = mtrue;
      cbase.check_after = 0;
      base->interrupt = mtrue;
    } else {
      switch (layout.bytes.b0) {
      case RBC_OP_NOP:
        break;
      case RBC_OP_HALT:
        cbase.terminate = mtrue;
        base->interrupt = mtrue;
        cbase.check_after = 0;
        break;
      case RBC_OP_SYSINT:
        rbc_isysint(&cbase, &core->kill_core);
        break;
      case RBC_OP_MINT:
        rbc_imint(&cbase, &core->kill_core);
        break;
      default:
        break;
      }
      cbase.PC += 8;
      cbase.check_after--;
    }
  }
  core->terminate = mtrue;
  core->interrupt = mtrue;
  for (msize_t i = 0;
       i < merry_RBCThread_list_size(core->rbc_cbase.child_threads); i++) {
    merry_thread_join(core->rbc_cbase.child_threads->buf[i], NULL);
  }
  return (_THRET_T_)0;
}

MerryCoreBase *rbc_master_core_create_base() {
  MerryCoreBase *base = (MerryCoreBase *)malloc(sizeof(MerryCoreBase));
  if (!base) {
    MFATAL("RBC", "Failed to initialize core base", NULL);
    return RET_NULL;
  }
  if (merry_cond_init(&base->cond) == RET_FAILURE) {
    MFATAL("RBC", "Failed to obtain condition variable", NULL);
    free(base);
    return RET_NULL;
  }
  base->createc = rbc_master_core_create;
  base->deletec = rbc_master_core_destroy;
  base->execc = rbc_master_core_run;
  base->predel = rbc_master_core_prep_for_deletion;
  base->setinp = rbc_master_core_set_input;
  base->prepcore = rbc_master_core_prepare_core;

  return base;
}

void rbc_master_core_destroy_base(MerryCoreBase *base) {
  merry_check_ptr(base);
  merry_cond_destroy(&base->cond);
  free(base);
}

void rbc_master_core_prep_for_deletion(mptr_t c) {
  // This function's job is to prepare the core
  // for deletion by setting the correct flags
  // and such. It is a procedure in of itself
  merry_check_ptr(c);

  RBCMasterCore *core = (RBCMasterCore *)c;

  // For now, this is everything we need
  // This will command the child cores to terminate
  // and since Graves will command the master core, it will
  // work
  core->terminate = mtrue;
  core->interrupt = mtrue;
}

mret_t rbc_master_core_set_input(mptr_t c, mstr_t path) {
  merry_check_ptr(c);
  merry_check_ptr(path);

  // For the moment, all it does is store the path of
  // the input Locally and allocate the reader
  RBCMasterCore *core = (RBCMasterCore *)c;
  core->rbc_cbase.inp_path = path;
  if ((core->inp = rbc_input_init()) == RET_NULL) {
    MLOG("RBC", "Failed to initialize the reader for input file %s", path);
    return RET_FAILURE;
  }
  return RET_SUCCESS;
}

mret_t rbc_master_core_prepare_core(mptr_t c) {
  // The only job of this function is to prepare the master
  // core for execution
  merry_check_ptr(c);

  RBCMasterCore *core = (RBCMasterCore *)c;

  if (!core->inp) {
    MFATAL("RBC", "Input not initialized before preparation", NULL);
    return RET_FAILURE;
  }

  // Setup the RAMs
  if ((core->rbc_cbase.iram = rbc_memory_init()) == RET_NULL) {
    MFATAL("RBC", "Failed to initialize instrucion memory: PATH=%s",
           core->rbc_cbase.inp_path);
    return RET_FAILURE;
  }

  if ((core->rbc_cbase.dram = rbc_memory_init()) == RET_NULL) {
    MFATAL("RBC", "Failed to initialize data memory: PATH=%s",
           core->rbc_cbase.inp_path);
    return RET_FAILURE;
  }

  // Read the input file
  if (rbc_input_read(core->inp, core->rbc_cbase.inp_path) == RET_FAILURE) {
    MFATAL("RBC", "Failed to read input file %s", core->rbc_cbase.inp_path);
    return RET_FAILURE;
  }

  // Prep the memories
  if (rbc_memory_populate(core->rbc_cbase.iram, core->inp->instruction_len,
                          core->inp->instructions) == RET_FAILURE) {
    MFATAL("RBC", "Failed to populate the instrucion memory: PATH=%s",
           core->rbc_cbase.inp_path);
    return RET_FAILURE;
  }

  if (rbc_memory_populate(core->rbc_cbase.dram, core->inp->data_len,
                          core->inp->data) == RET_FAILURE) {
    MFATAL("RBC", "Failed to populate the data memory: PATH=%s",
           core->rbc_cbase.inp_path);
    return RET_FAILURE;
  }

  // Now other fields
  core->rbc_cbase.req.args = &core->rbc_cbase.args;
  core->rbc_cbase.req.base = core->base;
  core->rbc_cbase.req.used_cond = &core->base->cond;

  core->interrupt = mfalse;
  core->terminate = mfalse;
  core->pause = mfalse;
  core->rbc_cbase.terminate = mfalse;
  core->kill_core = mfalse;

  core->rbc_cbase.check_after = 3;

  return RET_SUCCESS;
}

RBCCore *rbc_core_create() {
  RBCCore *core = (RBCCore *)malloc(sizeof(RBCCore));
  if (!core) {
    MFATAL("RBC:C", "Failed to allocate memory for a new core", NULL);
    return RET_NULL;
  }
  if ((core->rbc_cbase.stack = merry_get_anonymous_memory(_RBC_STACK_LEN_)) ==
      RET_NULL) {
    MFATAL("RBC:C", "Failed to initialize the stack", NULL);
    merry_RBCThread_list_destroy(core->rbc_cbase.child_threads);
    merry_Interface_list_destroy(core->rbc_cbase.interfaces);
    free(core);
    return RET_NULL;
  }

  core->rbc_cbase.SP = 0;
  core->rbc_cbase.BP = 0;

  if ((core->rbc_cbase.stack_frames =
           merry_RBCProcFrame_stack_init(_RBC_CALL_DEPTH_)) == RET_NULL) {
    MFATAL("RBC:C", "Failed to initialize the stack", NULL);
    merry_RBCThread_list_destroy(core->rbc_cbase.child_threads);
    merry_Interface_list_destroy(core->rbc_cbase.interfaces);
    merry_return_memory(core->rbc_cbase.stack, _RBC_STACK_LEN_);
    free(core);
    return RET_NULL;
  }

  if (merry_cond_init(&core->cond) == RET_FAILURE) {
    MFATAL("RBC:C", "Failed to initialize component", NULL);
    merry_RBCThread_list_destroy(core->rbc_cbase.child_threads);
    merry_Interface_list_destroy(core->rbc_cbase.interfaces);
    merry_return_memory(core->rbc_cbase.stack, _RBC_STACK_LEN_);
    merry_RBCProcFrame_stack_destroy(core->rbc_cbase.stack_frames);
    free(core);
    return RET_NULL;
  }
  return core;
}

void rbc_core_destroy(RBCCore *core) {
  merry_check_ptr(core);
  merry_cond_destroy(&core->cond);
  free(core);
}

_THRET_T_ rbc_core_run(mptr_t c) {
  merry_check_ptr(c);

  RBCCore *core = (RBCCore *)c;

  while (mtrue) {
  }

  for (msize_t i = 0;
       i < merry_RBCThread_list_size(core->rbc_cbase.child_threads); i++) {
    merry_thread_join(core->rbc_cbase.child_threads->buf[i], NULL);
  }
}

mret_t rbc_core_start(RBCCore *core, mthread_t *th) {
  merry_check_ptr(core);
  merry_check_ptr(th);
  return merry_thread_create(th, rbc_core_run, core);
}
