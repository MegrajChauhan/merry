#include <merry_core.h>
#include <merry_core_instruction_list.h>

_MERRY_DEFINE_STATIC_LIST_(Interface, MerryInterface *);
_MERRY_DEFINE_STACK_(CoreProcFrame, MerryCoreStackFrame);

mresult_t merry_core_create(maddress_t st_addr, MerryCore **core) {
  mresult_t ret;
  MerryCore *c = (MerryCore *)malloc(sizeof(MerryCore));
  if (!c) {
    MERR("Failed to allocate memory for the core", NULL);
    ret = MRES_SYS_FAILURE;
    goto MERRY_CC_FAILED;
  }
  if ((ret = merry_Interface_list_create(10, &c->interfaces)) !=
      MRES_SUCCESS) {
    MERR("Failed to allocate memory for a component", NULL);
    free(c);
    goto MERRY_CC_FAILED;
  }
  if ((ret = merry_mapped_memory_create(&c->st)) != MRES_SUCCESS) {
    MERR("Failed to initialize the stack", NULL);
    merry_Interface_list_destroy(c->interfaces);
    free(c);
    goto MERRY_CC_FAILED;
  }
  if ((ret = merry_mapped_memory_map(c->st, _MERRY_CORE_STACK_LEN_)) !=
      MRES_SUCCESS) {
    MERR("Failed to initialize the stack", NULL);
    merry_Interface_list_destroy(c->interfaces);
    merry_mapped_memory_destroy(c->st);
    free(c);
    goto MERRY_CC_FAILED;
  }
  merry_mapped_memory_obtain_ptr(c->st, &c->stack);
  if ((ret = merry_CoreProcFrame_stack_init(&c->stack_frames,
                                           _MERRY_CORE_CALL_DEPTH_)) != MRES_SUCCESS) {
    MERR("Failed to initialize the stack", NULL);
    merry_Interface_list_destroy(c->interfaces);
    merry_mapped_memory_unmap(c->st);
    merry_mapped_memory_destroy(c->st);
    free(c);
    goto MERRY_CC_FAILED;
  }

  c->SP = 0;
  c->BP = 0;
  c->PC = st_addr;
  *core = c;
  return MRES_SUCCESS;
MERRY_CC_FAILED:
  return ret;
}

void merry_core_destroy(MerryCore* c) {
  merry_check_ptr(c);

  merry_Interface_list_destroy(c->interfaces);
  merry_CoreProcFrame_stack_destroy(c->stack_frames);
  merry_core_memory_destroy(c->iram);
  merry_core_memory_destroy(c->dram);
  merry_mapped_memory_unmap(c->st);
  merry_mapped_memory_destroy(c->st);
  free(c);
}

msize_t merry_core_run(MerryCore* c) {
  merry_check_ptr(c);
  mbool_t running = mtrue;
  while (running) {
    if (surelyF(
            merry_core_memory_read_qword(c->iram, c->PC, &c->IR.whole_word) != MRES_SUCCESS)) {
          MERR("Page Fault: PC=%zu",c->PC);
          c->ret = 1;
          break;
    } else {
      running = c->HDLRS[c->IR.bytes.b0](c);
      c->PC += 8;
    }
  }
  return c->ret;
}

mresult_t merry_core_prepare_inst(MerryCore* c, mbptr_t inst, msize_t len) {
	merry_check_ptr(c);
	merry_check_ptr(inst);
	c->iram = merry_core_memory_init();
	if (!c->iram) 
		return MRES_SYS_FAILURE;    
	return merry_core_memory_populate(c->iram, len, inst);
}

mresult_t merry_core_prepare_data(MerryCore* c, mbptr_t data, msize_t len) {
	merry_check_ptr(c);
	merry_check_ptr(data);
	c->dram = merry_core_memory_init();
	if (!c->dram) 
		return MRES_SYS_FAILURE;    
	return merry_core_memory_populate(c->iram, len, data);
}
