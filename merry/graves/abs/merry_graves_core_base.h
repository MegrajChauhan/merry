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

  mbool_t running;   // Set to mfalse iff the core has terminated
  mbool_t interrupt; // the core was just interrupted

  mid_t id;
  muid_t uid;
  mguid_t guid;
  mcore_t type;

 // mcond_t cond; // Just the condition variable
};

#endif
