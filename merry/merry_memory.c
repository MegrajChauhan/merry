#include "merry_memory.h"

mptr_t merry_get_anonymous_memory(msize_t size) {
  mptr_t map;
#ifdef _USE_LINUX_
  if ((map = mmap(NULL, size, PROT_READ | PROT_WRITE,
                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
    return RET_NULL;
#endif
  return map;
}

void merry_return_memory(mptr_t mem, msize_t len) {
  merry_check_ptr(mem);
#ifdef _USE_LINUX_
  munmap(mem, len);
#endif
}

mret_t merry_map_file(mptr_t map, MerryInterface *file) {
  merry_check_ptr(map);
  merry_check_ptr(file);
  if (file->interface_t != INTERFACE_TYPE_FILE)
    return RET_FAILURE;
  msize_t len;
  merry_file_size(file, &len); // it shouldn't fail
#ifdef _USE_LINUX_
  if (mmap(map, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED,
           file->file.fd, 0) == MAP_FAILED)
    return RET_FAILURE;
#endif
  return RET_SUCCESS;
}
