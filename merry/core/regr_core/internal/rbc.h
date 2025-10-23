#ifndef _RBC_
#define _RBC_

/*
 * Structure of RBC(RBC's execution model):
 * The first thread will be the master thread that responds to Graves's
 * interrupts. This "master" thread will be the last to die. Each
 * thread will have its own private states and everything.
 * */

#include <merry_graves_core_base.h>
#include <merry_list.h>
#include <merry_logger.h>
#include <merry_protectors.h>
#include <merry_requests.h>
#include <merry_threads.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <regr_core/comp/inp/rbc_inp_reader.h>
#include <regr_core/comp/mem/rbc_ram.h>
#include <stdlib.h>

/*
 * The reason for calling this method BLOATED and useless
 * */
_MERRY_DECLARE_STATIC_LIST_(RBCThread, mthread_t);
_MERRY_DECLARE_STATIC_LIST_(Interface, MerryInterface *);
/* We are going to need so many more different lists! Oh Lord! */

typedef struct RBCCoreBase RBCCoreBase;
typedef struct RBCCore RBCCore;
typedef struct RBCMasterCore RBCMasterCore;

struct RBCMasterCore {
  MerryCoreBase *base;

  MerryRBCThreadList *child_threads;
  MerryInterfaceList *interfaces;

  MerryGravesRequest req;
  MerryRequestArgs args;

  // The child threads to the master core will use
  // the following shared variables for internal management
  mcond_t local_shared_cond;
  _Atomic mbool_t interrupt;
  _Atomic mbool_t pause;
  _Atomic mbool_t terminate;
};

struct RBCCore {
  MerryRBCThreadList *child_threads;
  MerryInterfaceList *interfaces;

  MerryGravesRequest req;
  MerryRequestArgs args;

  mcond_t *local_shared_cond;
  _Atomic mbool_t *interrupt;
  _Atomic mbool_t *pause;
  _Atomic mbool_t *terminate;
};

#endif
