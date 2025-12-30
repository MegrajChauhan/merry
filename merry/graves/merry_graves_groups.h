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

#include <merry_core_repr.h>
#include <merry_list.h>
#include <merry_results.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdlib.h>

_MERRY_DECLARE_STATIC_LIST_(CoreRepr, MerryCoreRepr *);

typedef struct MerryGravesGroup MerryGravesGroup;

struct MerryGravesGroup {
  MerryCoreReprList *all_cores;
  msize_t core_count;
  msize_t group_id;

  // Flags for the group that define the Group's properties
  //....
};

mresult_t merry_graves_group_create(MerryGravesGroup **grp, msize_t gid);

mresult_t merry_graves_group_add_core(MerryGravesGroup *grp,
                                      MerryCoreRepr *core);

mresult_t merry_graves_group_find_core(MerryGravesGroup *grp,
                                       MerryCoreRepr **res, msize_t uid,
                                       msize_t id);

MerryCoreRepr *merry_graves_group_find_dead_core(MerryGravesGroup *grp);

mresult_t merry_graves_group_get_core(MerryGravesGroup *grp,
                                      MerryCoreRepr **res, msize_t id);

void merry_graves_group_destroy(MerryGravesGroup *grp);

#endif
