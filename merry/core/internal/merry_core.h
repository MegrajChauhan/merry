#ifndef _MERRY_CORE_
#define _MERRY_CORE_

#include <merry_core_interface.h>
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
#include <stdlib.h>

_MERRY_DECLARE_STATIC_LIST_(Interface, MerryInterface *);
_MERRY_DECLARE_STACK_(CoreProcFrame, MerryCoreStackFrame);
/* We are going to need so many more different lists and stacks! Oh Lord! */

typedef struct MerryCore MerryCore;

struct MerryCore {
  MerryCoreRAM *ram;
  MerryInterfaceList *interfaces; // we will need a much more complicated data structure
  MerryCoreProcFrameStack *frame_stack;

  MerryCoreFlagsRegr flags;
  MerryCoreFFlagsRegr fflags;
  mqword_t _registers[MERRY_CORE_REG_COUNT];
  maddress_t BP, SP;
  maddress_t PC;
  
};

mresult_t rbc_core_create(MerryCoreIdentity iden,
                          maddress_t st_addr, mptr_t *ptr);

void rbc_core_destroy(mptr_t c);

msize_t rbc_core_run(mptr_t c);

void rbc_core_prep_for_deletion(mptr_t c);

mresult_t rbc_core_set_input(mptr_t c, mstr_t path);

mresult_t rbc_core_prepare_core(mptr_t c);

mresult_t rbc_core_config(mstr_t opt, mcstr_t val, mbool_t *used);

mresult_t rbc_core_set_flags(mptr_t core, msize_t op);

#endif
