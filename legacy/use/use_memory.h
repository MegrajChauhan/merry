#ifndef _USE_MEMORY_
#define _USE_MEMORY_

#include <merry_memory.h>
#include <use_interface_base.h>
#include <use_types.h>
#include <use_utils.h>

extern result_t get_anonymous_memory(ptr_t *map, msize_t size)
    _ALIAS_(merry_get_anonymous_memory);

extern void return_memory(ptr_t mem, msize_t len) _ALIAS_(merry_return_memory);

extern result_t map_file(ptr_t map, Interface *file) _ALIAS_(merry_map_file);

extern result_t map_file_explicit(ptr_t map, msize_t off, msize_t len,
                                  Interface *file)
    _ALIAS_(merry_map_file_explicit);

#endif
