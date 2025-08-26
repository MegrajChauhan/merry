#ifndef _MERRY_GRAVES_MEMORY_BASE_
#define _MERRY_GRAVES_MEMORY_BASE_

/*
 * The original plan was to implement Switchable RAM where there would
 * be multiple RAMs with various properties but now we will replace
 * that with page level control. Graves will still be the powerhouse
 * and so it can protect access and not only that but with page level
 * manipulation, we achieve finer control
 * */

#include <merry_graves_defs.h>
#include <merry_memory.h>
#include <merry_types.h>

typedef struct MerryGravesMemPg MerryGravesMemPg;

struct MerryGravesMemPg {
  MerryGravesMemPg *original; // The original page in case of shared pages

  // Identification data
  mmempg_t type;
  mid_t owner_id;
  muid_t owner_uid;
  mguid_t owner_guid;
  /*
   // Flags
   mbool_t shareable;    // If the page is MEMPG_PRIVATE, can it be shared?
   mbool_t sharing_lim;  // Is there a limit to how many cores can share
                         // this page? If shareable is not set to mtrue
                         // then this field means nothing
   mbool_t lazy_loading; // If set, this page is not allocated unless
                         // a page fault occurs for this specific page
                         // If lazy_loading is set to mtrue but the page
                         // is shared while the page hasn't been alloted
                         // then it will force an allocation
   mbool_t inherit_on_owner_death;      // If the owner dies then what is the
                                        // the fate of the page?
                                        // mtrue = VM owns the
   page(MEMPG_PUBLIC)
                                        // mfalse = VM destroys the page
                                        // The owner may transfer ownership
                                        // before its death as well.
   mbool_t notify_on_attempted_changes; // If this page was shared to
                                        // some core but it decided to
                                        // attempt some changes so the
                                        // owner is now notified
   mbool_t notify_on_disowning;         // If the core that shared this page
                                        // decides that it no longer needs this
                                        // page then the owner is notified of
                                        // the changes
   mbool_t shared_can_share;            // if this page was shared to a core
   than
                                        // can that core share it?
   mbool_t notify_on_share;             // If the shared page is shared further,
                                        // notify the owner

   // Properties
   msize_t sharing_limit_hard; // the sharing limit
 */
};

#endif
