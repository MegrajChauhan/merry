#ifndef _MERRY_MEMORY_
#define _MERRY_MEMORY_

#include <merry_config.h>
#include <merry_consts.h>
#include <merry_file.h>
#include <merry_interface.h>
#include <merry_logger.h>
#include <merry_operations.h>
#include <merry_platform.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdlib.h>
#include <string.h>

/*
 * This should be the interface to system's memory
 * */

mresult_t merry_get_anonymous_memory(mptr_t *map, msize_t size);

void merry_return_memory(mptr_t mem, msize_t len);

mresult_t merry_map_file(mptr_t map, MerryInterface *file);

mresult_t merry_map_file_explicit(mptr_t map, msize_t off, msize_t len,
                                  MerryInterface *file);

#endif
