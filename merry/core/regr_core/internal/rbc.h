#ifndef _RBC_
#define _RBC_

/*
 * Structure of RBC(RBC's execution model):
 * The first thread will be the master thread that responds to Graves's
 * interrupts. This "master" thread will be the last to die. Each
 * thread will have its own private states and everything.
 * */

#include <merry_flags_regr.h>
#include <merry_graves_core_base.h>
#include <merry_list.h>
#include <merry_logger.h>
#include <merry_protectors.h>
#include <merry_requests.h>
#include <merry_stack.h>
#include <merry_threads.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <regr_core/comp/inp/rbc_inp_reader.h>
#include <regr_core/comp/mem/rbc_ram.h>
#include <regr_core/def/consts/rbc_opcodes.h>
#include <regr_core/def/consts/rbc_registers.h>
#include <regr_core/def/consts/rbc_sysint.h>
#include <regr_core/def/declr/rbc_internals.h>
#include <stdlib.h>

/*
 * The reason for calling this method BLOATED and useless
 * */
_MERRY_DECLARE_STATIC_LIST_(RBCThread, mthread_t);
_MERRY_DECLARE_STATIC_LIST_(Interface, MerryInterface *);
_MERRY_DECLARE_STACK_(RBCProcFrame, RBCStackFrame);
/* We are going to need so many more different lists and stacks! Oh Lord! */

typedef struct RBCCoreBase RBCCoreBase;
typedef struct RBCCore RBCCore;
typedef struct RBCMasterCore RBCMasterCore;

struct RBCCoreBase {
  MerryRBCThreadList *child_threads;
  MerryInterfaceList *interfaces;
  mqword_t REGISTER_FILE[RBC_REG_COUNT];
  MerryGravesRequest req;
  MerryRequestArgs args;
  mstr_t inp_path;
  RBCMemory *iram, *dram;
  maddress_t PC;
  maddress_t SP, BP;
  mqptr_t stack;
  MerryRBCProcFrameStack *stack_frames;
  RBCFlagsRegr flags;
  RBCFFlagsRegr fflags;
  mbool_t terminate;
  msize_t check_after;
};

struct RBCMasterCore {
  MerryCoreBase *base;
  RBCCoreBase rbc_cbase;
  RBCInput *inp;

  // The child threads to the master core will use
  // the following shared variables for internal management
  mcond_t local_shared_cond;
  _Atomic mbool_t interrupt;
  _Atomic mbool_t pause;
  _Atomic mbool_t terminate;

  _Atomic mbool_t kill_core; // comes from within
};

struct RBCCore {
  RBCCoreBase rbc_cbase;
  mcond_t cond;

  mcond_t *local_shared_cond;
  _Atomic mbool_t *interrupt;
  _Atomic mbool_t *pause;
  _Atomic mbool_t *terminate;

  _Atomic mbool_t *kill_core;
};

// We will need the API functions that use the master core
// and then other functions that do the exact same thing but for the
// child cores

mptr_t rbc_master_core_create(MerryCoreBase *base, maddress_t st_addr);

void rbc_master_core_destroy(mptr_t c);

_THRET_T_ rbc_master_core_run(mptr_t c);

MerryCoreBase *rbc_master_core_create_base();

void rbc_master_core_destroy_base(MerryCoreBase *base);

void rbc_master_core_prep_for_deletion(mptr_t c);

mret_t rbc_master_core_set_input(mptr_t c, mstr_t path);

mret_t rbc_master_core_prepare_core(mptr_t c);

// For the child cores

// The caller will initialize some of the fields
RBCCore *rbc_core_create();

void rbc_core_destroy(RBCCore *core);

_THRET_T_ rbc_core_run(mptr_t c);

mret_t rbc_core_start(RBCCore *core, mthread_t *th);

#endif
