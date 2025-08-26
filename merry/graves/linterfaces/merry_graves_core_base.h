#ifndef _MERRY_GRAVES_CORE_BASE_
#define _MERRY_GRAVES_CORE_BASE_

// This is how Graves will identify each core

#include <merry_consts.h> // We have to properly start using this
#include <merry_dynamic_list.h>
#include <merry_error_stack.h>
#include <merry_graves_defs.h>
#include <merry_graves_memory_base.h>
#include <merry_protectors.h>
#include <merry_queue.h>
#include <merry_ram.h>
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
  // Function pointers
  mcorecreate_t createc;
  mcoredeletecore_t deletec;
  mcoreexec_t execc;

  // Flags
  mbool_t running;   // Set to mfalse iff the core has terminated
  mbool_t interrupt; // the core was just interrupted
  mbool_t terminate; // if set, the core will terminate
  // mbool_t pause;     // set to pause the core
  // mbool_t smsqen;    // Switchable Multi-State Queue Enabled?

  // Others....
  mid_t id;
  muid_t uid;
  mguid_t guid;

  MerryRAM *ram, *iram;

  // MerryDynamicQueue *sq; // State Queue
  // MerryDynamicList *all_memory_pages_owned;

  mcond_t cond; // Just the condition variable

  MerryErrorStack estack; // A personal error stack
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
