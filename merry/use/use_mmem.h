#ifndef _USE_MMEM_
#define _USE_MMEM_

#include <merry_mmem.h>
#include <use_defs.h>
#include <use_types.h>
#include <use_utils.h>

typedef MerryMMem MMem;

extern result_t map_memory(MMem **mem, ptr_t map, msize_t len)
    _ALIAS_(merry_map_file);

extern interfaceRet_t unmap_memory(MMem *map) _ALIAS_(merry_unmap_memory);

#endif
