#ifndef _MERRY_GRAVES_
#define _MERRY_GRAVES_

// The Big Boss
#include <merry_config.h>
#include <merry_consts.h>
#include <merry_core_types.h>
#include <merry_default_consts.h>
#include <merry_dynamic_list.h>
#include <merry_error_stack.h>
#include <merry_graves_core_base.h>
#include <merry_graves_core_repr.h>
#include <merry_graves_defs.h>
#include <merry_graves_groups.h>
#include <merry_graves_input.h>
#include <merry_graves_memory_base.h>
#include <merry_graves_request_queue.h>
#include <merry_memory.h>
#include <merry_protectors.h>
#include <merry_requests.h>
#include <merry_threads.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdlib.h>

typedef struct MerryGraves MerryGraves;

struct MerryGraves {
  MerryDynamicList *GRPS; // all of the groups
  msize_t overall_core_count;
  msize_t overall_active_core_count;
  msize_t initial_data_mem_page_count;
  MerryGravesInput *input;

  // needed fields
  mcond_t graves_cond;
  mmutex_t graves_lock;
  msize_t return_value;
  msize_t core_count; // for the unique ID
  msize_t active_core_count;

  mcorecreatebase_t HOW_TO_CREATE_BASE[__CORE_TYPE_COUNT];
  mcoredeletebase_t HOW_TO_DESTROY_BASE[__CORE_TYPE_COUNT];

  MerryConsts *_config;

  atomic_bool interrupt_broadcast;
  atomic_size_t request_broadcast;
  atomic_size_t broadcast_result; // Once the broadcast is made and
                                  // each core start completing the
                                  // request then they will update
                                  // this counter and Graves will
                                  // wait until it reaches 0
};

// GRAVES has the full authority to terminate the VM
_MERRY_INTERNAL_ MerryGraves GRAVES;

// Entry to Graves(doesn't return)
_MERRY_NO_RETURN_ void Merry_Graves_Run(int argc, char **argv);

// Pre-initialization before initializing Graves
mret_t merry_graves_pre_init(MerryErrorStack *st);

// Read the input file
mret_t merry_graves_parse_input(MerryErrorStack *st);

// Initialize Graves
mret_t merry_graves_init(MerryErrorStack *st);

// Prepare everything now right before running
mret_t merry_graves_ready_everything(MerryErrorStack *st);
// Registers all of the Base creation and destruction functions
void merry_graves_acquaint_with_cores();

// This is the OVERSEER
void merry_graves_START();

// Destroy Graves
void merry_graves_destroy(MerryErrorStack *st);

// Step by step clearing of the Graves before basic cleanup
void merry_graves_cleanup_groups();
void merry_graves_terminate_all_cores();
void merry_graves_cleanup_request_queue();
void merry_graves_temporary_wake_up_request_queue_cores();

// Utilities

MerryGravesGroup *merry_graves_add_group(MerryErrorStack *st);
MerryGravesCoreRepr *merry_graves_add_core(MerryGravesGroup *grp,
                                           MerryErrorStack *st);
mret_t merry_graves_init_a_core(MerryGravesCoreRepr *repr, mcore_t type,
                                maddress_t addr, MerryErrorStack *st);
mret_t merry_graves_boot_a_core(MerryGravesCoreRepr *repr, MerryErrorStack *st);
void merry_graves_give_IDs_to_cores(MerryGravesCoreRepr *repr,
                                    MerryGravesGroup *grp);
void merry_graves_wait_out_global_broadcast();

/* Request Handlers */
REQ(kill_self);
REQ(create_core);
REQ(create_group);

#endif
