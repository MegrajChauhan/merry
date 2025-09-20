#ifndef _MERRY_GRAVES_GROUP_
#define _MERRY_GRAVES_GROUP_

/*
 * The vcores are divided into groups:
 * - Group A:
 *     - vcore 1
 *     - vcore 2
 *     - vcore N
 * - Group N:
 *     - vcore 1
 *     - vcore 2
 *     - vcore N
 *
 * Although nothing has been planned for Groups, we may introduce some
 * form of features to utilize this structure. The main reason for
 * groups is for priviledge management such that some vcores may not
 * interfere with other vcores with a similar goal. So, a group may
 * contain vcores that share a common goal.
 * */

// Graves actually works with groups

#include <merry_dynamic_list.h>
#include <merry_error_stack.h>
#include <merry_graves_core_base.h>
#include <merry_graves_core_repr.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdatomic.h>
#include <stdlib.h>

#define merry_graves_group_register_new_core(grp) ((grp)->active_core_count++)
#define merry_graves_group_register_dead_core(grp) ((grp)->active_core_count--)
#define merry_graves_group_index_for(grp, repr)                                \
  merry_dynamic_list_index_of((grp)->all_cores, (repr))
#define merry_graves_group_dead(grp) ((grp)->active_core_count == 0)

typedef struct MerryGravesGroup MerryGravesGroup;

struct MerryGravesGroup {
  MerryDynamicList *all_cores;
  msize_t core_count;
  msize_t active_core_count;
  msize_t group_id;
};

MerryGravesGroup *merry_graves_group_create(msize_t gid, MerryErrorStack *st);

MerryGravesCoreRepr *merry_graves_group_add_core(MerryGravesGroup *grp,
                                                 MerryErrorStack *st);

void merry_graves_group_last_add_failed(MerryGravesGroup *grp);

MerryGravesCoreRepr *merry_graves_group_find_core(MerryGravesGroup *grp,
                                                  msize_t uid, msize_t id,
                                                  mbool_t *is_dead);

MerryGravesCoreRepr *merry_graves_group_find_dead_core(MerryGravesGroup *grp);

MerryGravesCoreRepr *merry_graves_group_get_core(MerryGravesGroup *grp,
                                                 msize_t id);

void merry_graves_group_destroy(MerryGravesGroup *grp);

#endif
