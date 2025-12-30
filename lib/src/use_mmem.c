#include <lib/merry_mapped_memory.h>
#include <use_mmem.h>

_ALWAYS_INLINE_ result_t mapped_memory_create(MappedMemory **mem) {
  return merry_mapped_memory_create(mem);
}

_ALWAYS_INLINE_ result_t mapped_memory_map(MappedMemory *mem, size_t len) {
  return merry_mapped_memory_map(mem, len);
}

_ALWAYS_INLINE_ result_t mapped_memory_unmap(MappedMemory *mem) {
  return merry_mapped_memory_unmap(mem);
}

_ALWAYS_INLINE_ result_t mapped_memory_obtain_ptr(MappedMemory *mem,
                                                  bptr_t *ptr) {
  return merry_mapped_memory_obtain_ptr(mem, ptr);
}

_ALWAYS_INLINE_ result_t mapped_memory_destroy(MappedMemory *mem) {
  return merry_mapped_memory_destroy(mem);
}
