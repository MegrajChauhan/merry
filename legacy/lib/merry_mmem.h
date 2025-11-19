#ifndef _MERRY_MMEM_
#define _MERRY_MMEM_

// MMEM: Mapped Memory

#include <merry_interface.h>
#include <merry_memory.h>
#include <merry_operations.h>
#include <stdlib.h>
#include <string.h>

typedef MerryInterface MerryMMem;

/*
 * Since Merry doesn't care about the flags of the map,
 * the program will map it for themselves and we will store it
 * but maybe we will care about the maps too
 * */

mresult_t merry_map_memory(MerryMMem **mem, mptr_t map, msize_t len);

/*minterfaceRet_t merry_sub_map_memory(MerryMMem *map, MerryMMem **res,
                                     mptr_t sub_map, msize_t len);*/

minterfaceRet_t merry_unmap_memory(MerryMMem *map);

#endif
