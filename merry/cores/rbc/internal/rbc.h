#ifndef _RBC_
#define _RBC_

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
#include <rbc/comp/inp/rbc_inp_reader.h>
#include <rbc/comp/mem/rbc_ram.h>
#include <rbc/def/consts/rbc_consts.h>
#include <rbc/def/consts/rbc_opcodes.h>
#include <rbc/def/consts/rbc_registers.h>
#include <rbc/def/consts/rbc_sysint.h>
#include <rbc/def/declr/rbc_internals.h>
#include <stdlib.h>

_MERRY_DECLARE_STATIC_LIST_(Interface, MerryInterface *);
_MERRY_DECLARE_STACK_(RBCProcFrame, RBCStackFrame);
/* We are going to need so many more different lists and stacks! Oh Lord! */

typedef struct RBCCore RBCCore;

struct RBCCore {
  MerryCoreState *state;
  MerryCoreIdentity *iden;
  MerryInterfaceList *interfaces;
  mqword_t REGISTER_FILE[RBC_REG_COUNT];
  mstr_t inp_path;
  RBCMemory *iram, *dram;
  maddress_t PC;
  maddress_t SP, BP;
  MerryMappedMemory *st;
  mqptr_t stack;
  MerryRBCProcFrameStack *stack_frames;
  RBCFlagsRegr flags;
  RBCFFlagsRegr fflags;
  msize_t check_after;
  RBCInput *inp;
};

// for internal requests
void rbc_make_internal_request(RBCCore *core, mgreq_t req);

mresult_t rbc_core_create(MerryCoreState *state, MerryCoreIdentity *iden,
                          maddress_t st_addr, mptr_t *ptr);

void rbc_core_destroy(mptr_t c);

msize_t rbc_core_run(mptr_t c);

void rbc_core_prep_for_deletion(mptr_t c);

mresult_t rbc_core_set_input(mptr_t c, mstr_t path);

mresult_t rbc_core_prepare_core(mptr_t c);

mresult_t rbc_core_config(mstr_t opt, mcstr_t val, mbool_t *used);

#endif
