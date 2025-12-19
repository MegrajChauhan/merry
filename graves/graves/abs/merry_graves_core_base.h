#ifndef _MERRY_GRAVES_CORE_BASE_
#define _MERRY_GRAVES_CORE_BASE_

// This is how Graves will identify each core

#include <merry_consts.h> // We have to properly start using this
#include <merry_graves_defs.h>
#include <merry_interface.h>
#include <merry_list.h>
#include <merry_protectors.h>
#include <merry_requests.h>
#include <merry_types.h>
#include <merry_utils.h>

typedef struct MerryCoreBase MerryCoreBase;

struct MerryCoreBase {
  mcorecreate_t createc;
  mcoredeletecore_t deletec;
  mcoreexec_t execc;
  mcorepredel_t predel;
  mcoresetinp_t setinp;
  mcoreprepcore_t prepcore;

  // Flags that the core should use to provide status
  atm_mbool_t running;   // Set to mfalse iff the core has terminated
  atm_mbool_t paused; // set to mtrue iff the core was paused by Graves's signal
  atm_mbool_t stopped; // set to mtrue iff the core was paused because of itself
  atm_mbool_t busy; // set to mtrue iff the core is busy with something and cannot accept requests(interrupts will be sent nonetheless)
   
  // Flags that Graves will use to send information
  atm_mbool_t interrupt; // interrupt the core
  atm_mbool_t pause; // pause execution until told to continue
  
  msize_t core_errno; // The errno for each core

  // Passing interrupt information here....
  // .....

  mcond_t pausing_cond; // cores use this condition variable to pause and Graves will use this to wake them up it
};

#endif
