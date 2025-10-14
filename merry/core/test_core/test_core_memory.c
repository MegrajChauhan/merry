#include <test_core/comps/test_core_memory.h>

TCMem *tc_mem_init(mbptr_t inst, msize_t len) {
  TCMem *mem = (TCMem *)malloc(sizeof(TCMem));
  if (!mem) {
    MFATAL("TC", "Failed to allocate for memory", NULL);
    return RET_NULL;
  }
  mem->all_instructions = inst;
  mem->len = len;
  return mem;
}

void tc_mem_destroy(TCMem *mem) {
  merry_check_ptr(mem);
  merry_check_ptr(mem->all_instructions);
  // The reader will free the instructions
  free(mem);
}

_MERRY_ALWAYS_INLINE_ mret_t tc_read(TCMem *mem, maddress_t addr,
                                     mbptr_t store_in) {
  if (addr >= mem->len)
    return RET_FAILURE;
  *store_in = mem->all_instructions[addr];
  return RET_SUCCESS;
}
