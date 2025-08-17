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
  MerryGravesInput *input;

  // needed fields
  mcond_t graves_cond;
  mmutex_t graves_lock;
  msize_t return_value;

  mcorecreatebase_t HOW_TO_CREATE_BASE[__CORE_TYPE_COUNT];
  mcoredeletebase_t HOW_TO_DESTROY_BASE[__CORE_TYPE_COUNT];
};

// GRAVES has the full authority to terminate the VM
_MERRY_INTERNAL_ MerryGraves GRAVES;

mret_t merry_graves_user_input_needs_attention(int argc, char **argv,
                                               MerryErrorStack *st);

// ... if we have config files, parsing them should happen here

mret_t merry_graves_pre_initialize(MerryErrorStack *st);

mret_t merry_graves_understand_user_input(MerryErrorStack *st);

void merry_graves_acquaint_with_cores();

MerryGravesGroup *merry_graves_find_dead_group(msize_t *ind);

mret_t merry_graves_add_group(MerryErrorStack *st);

MerryGravesCoreRepr *
merry_graves_find_a_dead_core_repr_grp_specific(MerryGravesGroup *grp,
                                                MerryErrorStack *st);

MerryGravesCoreRepr *merry_graves_find_a_dead_core_repr(MerryErrorStack *st);

mret_t merry_graves_initialize_core_repr(MerryGravesCoreRepr *repr,
                                         mcore_t type, MerryErrorStack *st);

mret_t merry_graves_add_core_repr(MerryGravesGroup *parent_grp,
                                  MerryErrorStack *st);

mret_t merry_graves_launch_core(MerryGravesCoreRepr *repr, MerryErrorStack *st);

mret_t merry_graves_initialize(MerryErrorStack *st, int argc, char **argv);

#endif
