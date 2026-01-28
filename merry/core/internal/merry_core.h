#ifndef _MERRY_CORE_
#define _MERRY_CORE_

#include <merry_flags_regr.h>
#include <merry_list.h>
#include <merry_logger.h>
#include <merry_mapped_memory.h>
#include <merry_protectors.h>
#include <merry_requests.h>
#include <merry_stack.h>
#include <merry_threads.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <merry_graves_request_queue.h>
#include <merry_core_internals.h>
#include <merry_core_defs.h>
#include <merry_core_registers.h>
#include <merry_core_sysint.h>
#include <merry_core_instruction_handler.h>
#include <merry_core_ram.h>
#include <stdlib.h>

_MERRY_DECLARE_STATIC_LIST_(Interface, MerryInterface *);
_MERRY_DECLARE_STACK_(CoreProcFrame, MerryCoreStackFrame);
/* We are going to need so many more different lists and stacks! Oh Lord! */

typedef struct MerryCore MerryCore;

struct MerryCore {
  MerryCoreRAM *iram, *dram;
  MerryInterfaceList *interfaces; // we will need a much more complicated data structure
  MerryCoreProcFrameStack *stack_frames;
  MerryMappedMemory *st;

  MerryCoreFlagsRegr flags;
  MerryCoreFFlagsRegr fflags;
  mqword_t REGISTER_FILE[MERRY_CORE_REG_COUNT];
  maddress_t BP, SP;
  maddress_t PC;  
  MerryHostMemLayout IR;
  mqptr_t stack;
  msize_t ret;
};

mresult_t merry_core_create(maddress_t st_addr, MerryCore **core);

void merry_core_destroy(MerryCore* c);

msize_t merry_core_run(MerryCore* c);

mresult_t merry_core_prepare_inst(MerryCore* c, mbptr_t inst, msize_t len);

mresult_t merry_core_prepare_data(MerryCore* c, mbptr_t data, msize_t len);

#endif
