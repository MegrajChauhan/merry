#ifndef _MERRY_MAPPED_MEMORY_
#define _MERRY_MAPPED_MEMORY_

#include <merry_interface.h>
#include <merry_results.h>
// #include <merry_mapped_memory_definitions.h>
#include <stdlib.h>

typedef MerryInterface MerryMappedMemory;

mresult_t merry_mapped_memory_create(MerryMappedMemory **mem);

mresult_t merry_mapped_memory_map(MerryMappedMemory *mem, msize_t len);

mresult_t merry_mapped_memory_unmap(MerryMappedMemory *mem);

mresult_t merry_mapped_memory_obtain_ptr(MerryMappedMemory *mem, mbptr_t *ptr);

mresult_t merry_mapped_memory_destroy(MerryMappedMemory *mem);

#endif
