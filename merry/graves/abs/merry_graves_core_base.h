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

/*
 * ram, iram -> cleaned up by Graves
 * sq -> To be cleaned up by the core
 * all_memory_pages_owned -> return all memory to Graves by core
 *
 * */
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

  mcond_t cond; // Just the condition variable
};

/*
 * Basically, each vcore will have its own data memory but share
 * the same public pages across all of the cores. If a vcore wants
 * new pages private to itself, then it may add it to its own private
 * memory or share that page with others as well. In a sense, each
 * vcore runs its own "process" that has its own "virtual address space"
 * These features provide lots of control and flexibility while making
 * Merry memory hungry.
 *
 * These structures such as the MerryCoreBase, MerryGravesMemPg are
 * just the interfaces that Graves will use to communicate with the
 * vcores. In truth, the vcores are free to use them as they like
 * internally in the way that suits their needs the best as long as
 * the protocol is adhered to.
 * */

#endif
