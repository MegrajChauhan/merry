#include <regr_core/comp/mem/rbc_ram.h>

RBCMemory *rbc_memory_init(msize_t pg_count) {
  RBCMemory *mem = (RBCMemory *)malloc(sizeof(RBCMemory));
  if (!mem) {
    MFATAL("RBC", "Failed to allocate memory for RBC RAM", NULL);
    return RET_NULL;
  }
  mem->pg_list = merry_RBCMemPage_list_create(pg_count);
  if (!mem->pg_list) {
    MFATAL("RBC", "Failed to allocate memory for RBC RAM PAGE BUFFER", NULL);
    free(mem);
    return RET_NULL;
  }
  mem->max_address = pg_count * _RBC_PAGE_LEN_IN_BYTES_;
  return mem;
}

mret_t rbc_memory_populate(RBCMemory *mem, msize_t pg_num, mbptr_t addr_space) {
  merry_check_ptr(mem);
  merry_check_ptr(mem->pg_list);
  merry_check_ptr(addr_space);
  if (pg_num >= mem->pg_list->cap) {
    MFATAL("RBC",
           "Populating memory page that is beyond what the memory can handle: "
           "LIM=%zu, REQUEST=%zu",
           mem->pg_list->cap, pg_num);
    return RET_FAILURE;
  }

  // Yes! We accessed the buffer directly
  if (mem->pg_list->buf[pg_num].repr.bytes) {
    MERROR("RBC", "Attempt to overwrite populated page: PG_NUM=%zu", pg_num);
    return RET_FAILURE;
  }

  mem->pg_list->buf[pg_num].repr.bytes = addr_space;
  return RET_SUCCESS;
}

void rbc_memory_destroy(RBCMemory *mem) {
  // The pages and the memory buffer is none of RAM's concern
  merry_check_ptr(mem);
  merry_RBCMemPage_list_destroy(mem->pg_list);
  free(mem);
}
