#ifndef _MERRY_MAPPED_MEMORY_
#define _MERRY_MAPPED_MEMORY_

#include <merry_interface.h>
#include <merry_operations.h>
#include <merry_mapped_memory_definitions.h>
#include <stdlib.h>

typedef MerryInterface MerryMappedMemory;

mresult_t merry_mapped_memory_create(MerryMappedMemory **mem);

mresult_t merry_mapped_memory_map(MerryMappedMemory *mem, msize_t len, msize_t flags);

mresult_t merry_mapped_memory_unmap(MerryMappedMemory *mem);

mresult_t merry_mapped_memory_destroy(MerryMappedMemory *mem);

#endif
