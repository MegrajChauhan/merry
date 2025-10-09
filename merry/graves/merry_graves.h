#ifndef _MERRY_GRAVES_
#define _MERRY_GRAVES_

// The Big Boss
#include <merry_config.h>
#include <merry_consts.h>
#include <merry_core_types.h>
#include <merry_default_consts.h>
#include <merry_graves_core_base.h>
#include <merry_graves_core_repr.h>
#include <merry_graves_defs.h>
#include <merry_graves_groups.h>
#include <merry_graves_input.h>
#include <merry_graves_request_queue.h>
#include <merry_list.h>
#include <merry_logger.h>
#include <merry_memory.h>
#include <merry_nort.h>
#include <merry_protectors.h>
#include <merry_requests.h>
#include <merry_threads.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdlib.h>
#include <test_core/tc/tc.h>

typedef struct MerryGraves MerryGraves;

struct MerryGraves {
  MerryGravesGroup **GRPS; // all of the groups
  msize_t overall_core_count;
  msize_t overall_active_core_count;
  msize_t grp_count;
  mstr_t *C_ENTRIES;
  mbool_t DIE;

  // needed fields
  mcond_t graves_cond;
  mmutex_t graves_lock;
  msize_t return_value;
  msize_t core_count; // for the unique ID
  msize_t active_core_count;

  mcorecreatebase_t HOW_TO_CREATE_BASE[__CORE_TYPE_COUNT];
  mcoredeletebase_t HOW_TO_DESTROY_BASE[__CORE_TYPE_COUNT];

  MerryConsts *_config;
};

// GRAVES has the full authority to terminate the VM
_MERRY_INTERNAL_ MerryGraves GRAVES;

// Entry to Graves(doesn't return)
void Merry_Graves_Run(int argc, char **argv);

// Pre-initialization before initializing Graves
mret_t merry_graves_pre_init();

// Read the input file
mret_t merry_graves_parse_input();

// Initialize Graves
mret_t merry_graves_init();

// Prepare everything now right before running
mret_t merry_graves_ready_everything();
// Registers all of the Base creation and destruction functions
void merry_graves_acquaint_with_cores();

// This is the OVERSEER
void merry_graves_START(mptr_t __);

// Destroy Graves
void merry_graves_destroy();

// Step by step clearing of the Graves before basic cleanup
void merry_graves_cleanup_groups();
// void merry_graves_pre_destruction();

// Utilities
mcore_t merry_graves_obtain_first_valid_c_entry();
MerryGravesGroup *merry_graves_add_group();
MerryGravesCoreRepr *merry_graves_add_core(MerryGravesGroup *grp);
mret_t merry_graves_init_a_core(MerryGravesCoreRepr *repr, mcore_t type,
                                maddress_t addr);
mret_t merry_graves_init_a_core_no_prep(MerryGravesCoreRepr *repr, mcore_t type,
                                        maddress_t addr);
mret_t merry_graves_boot_a_core(MerryGravesCoreRepr *repr);
void merry_graves_give_IDs_to_cores(MerryGravesCoreRepr *repr,
                                    MerryGravesGroup *grp);

/* Request Handlers */
REQ(kill_self);
REQ(create_core);
REQ(create_group);
REQ(get_group_details);
REQ(get_system_details);

#endif
