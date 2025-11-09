#include "merry_memory.h"

mresult_t merry_get_anonymous_memory(mptr_t *map, msize_t size) {
#ifdef _USE_LINUX_
  if ((*map = mmap(NULL, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
    return MRES_SYS_FAILURE;
#endif
  return MRES_SUCCESS;
}

void merry_return_memory(mptr_t mem, msize_t len) {
  merry_check_ptr(mem);
#ifdef _USE_LINUX_
  munmap(mem, len);
#endif
}

mresult_t merry_map_file(mptr_t map, MerryInterface *file) {
  merry_check_ptr(map);
  merry_check_ptr(file);
  if (file->interface_t != INTERFACE_TYPE_FILE)
    return MRES_INVALID_ARGS;
  msize_t len;
  merry_file_size(file, &len); // it shouldn't fail
#ifdef _USE_LINUX_
  if (mmap(map, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED,
           file->file.fd, 0) == MAP_FAILED)
    return MRES_SYS_FAILURE;
#endif
  return MRES_SUCCESS;
}

mresult_t merry_map_file_explicit(mptr_t map, msize_t off, msize_t len,
                                  MerryInterface *file) {
  merry_check_ptr(map);
  merry_check_ptr(file);
  if (file->interface_t != INTERFACE_TYPE_FILE)
    return MRES_INVALID_ARGS;
#ifdef _USE_LINUX_
  if (mmap(map, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED,
           file->file.fd, off) == MAP_FAILED)
    return MRES_SYS_FAILURE;
#endif
  return MRES_SUCCESS;
}
