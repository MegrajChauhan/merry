#ifndef _USE_MMEM_
#define _USE_MMEM_

#include <use_defs.h>
#include <use_types.h>
#include <use_interface.h>

typedef Interface MappedMemory;

result_t mapped_memory_create(MappedMemory **mem);

result_t mapped_memory_map(MappedMemory *mem, size_t len);

result_t mapped_memory_unmap(MappedMemory *mem);

result_t mapped_memory_obtain_ptr(MappedMemory *mem, bptr_t *ptr);

result_t mapped_memory_destroy(MappedMemory *mem);

#endif
