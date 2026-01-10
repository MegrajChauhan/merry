#ifndef _MERRY_GRAVES_
#define _MERRY_GRAVES_

// The Big Boss
#include <merry_config.h>
#include <merry_consts.h>
#include <merry_core_interface.h>
#include <merry_core_metadata.h>
#include <merry_core_repr.h>
#include <merry_core_state.h>
#include <merry_core_types.h>
#include <merry_graves_defs.h>
#include <merry_graves_groups.h>
#include <merry_graves_input.h>
#include <merry_graves_request_queue.h>
#include <merry_list.h>
#include <merry_logger.h>
#include <merry_protectors.h>
#include <merry_requests.h>
#include <merry_results.h>
#include <merry_threads.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdlib.h>

_MERRY_DECLARE_STATIC_LIST_(Group, MerryGravesGroup *);

typedef struct MerryGraves MerryGraves;

struct MerryGraves {
  MerryGroupList *GRPS; // all of the groups
  msize_t grp_count;
  MerryEntryList *C_ENTRIES;

  // needed fields
  mcond_t graves_cond;
  mmutex_t graves_lock;
  msize_t core_count; // for the unique ID
  atm_msize_t active_core_count;

  MerryConsts *_config;
  MerryGravesRequest *current_req;
};

// Entry to Graves(doesn't return)
void Merry_Graves_Run(int argc, char **argv);

// Pre-initialization before initializing Graves
mresult_t merry_graves_pre_init();

// Read the input file
mresult_t merry_graves_parse_input();

// Initialize Graves
mresult_t merry_graves_init();

// Prepare everything now right before running
mresult_t merry_graves_ready_everything();

// This is the OVERSEER
void merry_graves_START();

// Destroy Graves
void merry_graves_destroy();

// Step by step clearing of the Graves before basic cleanup
void merry_graves_cleanup_groups();

// Utilities
mcore_t merry_graves_obtain_first_valid_c_entry();
mresult_t merry_graves_add_group(MerryGravesGroup **grp);
mresult_t merry_graves_add_core(MerryGravesGroup *grp, MerryCoreRepr **repr);
mresult_t merry_graves_init_a_core(MerryCoreRepr *repr, mcore_t type,
                                   maddress_t addr);
mresult_t merry_graves_boot_a_core(MerryCoreRepr *repr);
void merry_graves_give_IDs_to_cores(MerryCoreRepr *repr, MerryGravesGroup *grp);
void merry_graves_failed_core_booting();

MerryGravesGroup *merry_graves_get_group(mguid_t gid);

_THRET_T_ merry_graves_core_launcher(mptr_t r);

/* Request Handlers */
REQ(create_core);
REQ(create_group);
REQ(get_system_details);

#endif
