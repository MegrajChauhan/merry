#ifndef _MERRY_MEMORY_
#define _MERRY_MEMORY_

#include <merry_config.h>
#include <merry_consts.h>
#include <merry_dynamic_list.h>
#include <merry_error_stack.h>
#include <merry_platform.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdlib.h>
#include <string.h>

/**
 * This is not a custom allocator for the VM.
 * This will allocate memory pages from the host
 * The pages will be of constant size and we will introduce specific types of
 * these pages too With the ideas i have in my head, i cannot give up speed over
 * the functionality they provide. The complications that we might meet are too
 * much to think about.
 */

_MERRY_INTERNAL_ MerryDynamicList *pg_list; // a list of "deleted" pages

typedef struct MerryMemoryPageBase MerryMemoryPageBase;
typedef MerryMemoryPageBase MerryNormalMemoryPage;

struct MerryMemoryPageBase {
  mbptr_t buf; // The size of the buffer is known to us[a constant]
  mbool_t init;
};

mret_t merry_initialize_memory_interface(MerryErrorStack *st);

MerryNormalMemoryPage *merry_create_normal_memory_page(MerryErrorStack *st);

mret_t merry_initialize_normal_memory_page(MerryNormalMemoryPage *pg,
                                           MerryErrorStack *st);

MerryNormalMemoryPage *
merry_obtain_initialized_normal_memory_page(MerryErrorStack *st);

void merry_return_normal_memory_page(MerryNormalMemoryPage *pg,
                                     MerryErrorStack *st);

void merry_destroy_memory_interface();

#endif
