#ifndef _USE_MMEM_
#define _USE_MMEM_

#include <use_defs.h>
#include <use_types.h>
#include <use_interface.h>

typedef Interface MappedMemory_ALIAS_();

extern result_t mapped_memory_create(MappedMemory **mem) _ALIAS_(merry_mapped_memory_create);

extern result_t mapped_memory_map(MappedMemory *mem, size_t len) _ALIAS_(merry_mapped_memory_map);

extern result_t mapped_memory_unmap(MappedMemory *mem) _ALIAS_(merry_mapped_memory_unmap);

extern result_t mapped_memory_obtain_ptr(MappedMemory *mem, bptr_t *ptr) _ALIAS_(merry_mapped_memory_obtain_ptr);

extern result_t mapped_memory_destroy(MappedMemory *mem) _ALIAS_(merry_mapped_memory_destroy);

#endif
