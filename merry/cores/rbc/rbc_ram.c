#include <regr_core/comp/mem/rbc_ram.h>

_MERRY_DEFINE_STATIC_LIST_(RBCMemPage, RBCMemPage);

RBCMemory *rbc_memory_init() {
  RBCMemory *mem = (RBCMemory *)malloc(sizeof(RBCMemory));
  if (!mem) {
    MFATAL("RBC", "Failed to allocate memory for RBC RAM", NULL);
    return RET_NULL;
  }
  mem->pg_list = NULL;
  mem->max_address = 0;
  return mem;
}

mresult_t rbc_memory_populate(RBCMemory *mem, msize_t space_len,
                              mbptr_t addr_space) {
  merry_check_ptr(mem);
  merry_check_ptr(addr_space);

  msize_t pg_count =
      space_len /
      _RBC_PAGE_LEN_IN_BYTES_; // space_len should be aligned most of the time
                               // until instruction len is extremely small
  if (space_len && (space_len < _RBC_PAGE_LEN_IN_BYTES_))
    pg_count = 1;

  mresult_t res = merry_RBCMemPage_list_create(pg_count, &mem->pg_list);
  if (res != MRES_SUCCESS) {
    MFATAL("RBC", "Failed to allocate memory for RBC RAM PAGE BUFFER", NULL);
    return RET_FAILURE;
  }

  mem->max_address = space_len;

  for (msize_t i = 0; i < pg_count; i++) {
    // Yes! We accessed the buffer directly
    mem->pg_list->buf[i].repr.bytes = addr_space + i * _RBC_PAGE_LEN_IN_BYTES_;
  }
  return RET_SUCCESS;
}

rbcmemOperRes_t rbc_memory_read_byte(RBCMemory *mem, maddress_t addr,
                                     mbptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  *store_in = mem->pg_list->buf[addr / _RBC_PAGE_LEN_IN_BYTES_]
                  .repr.bytes[addr % _RBC_PAGE_LEN_IN_BYTES_];
  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_read_word(RBCMemory *mem, maddress_t addr,
                                     mwptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  *store_in = *(mwptr_t)&mem->pg_list->buf[addr / _RBC_PAGE_LEN_IN_BYTES_]
                   .repr.bytes[addr % _RBC_PAGE_LEN_IN_BYTES_];
  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_read_dword(RBCMemory *mem, maddress_t addr,
                                      mdptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  *store_in = *(mdptr_t)&mem->pg_list->buf[addr / _RBC_PAGE_LEN_IN_BYTES_]
                   .repr.bytes[addr % _RBC_PAGE_LEN_IN_BYTES_];
  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_read_qword(RBCMemory *mem, maddress_t addr,
                                      mqptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  *store_in = *(mqptr_t)&mem->pg_list->buf[addr / _RBC_PAGE_LEN_IN_BYTES_]
                   .repr.bytes[addr % _RBC_PAGE_LEN_IN_BYTES_];
  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_read_byte_atm(RBCMemory *mem, maddress_t addr,
                                         mbptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  *store_in = atomic_load_explicit(
      (_Atomic mbyte_t *)&mem->pg_list->buf[addr / _RBC_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _RBC_PAGE_LEN_IN_BYTES_],
      memory_order_relaxed);
  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_read_word_atm(RBCMemory *mem, maddress_t addr,
                                         mwptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  *store_in = atomic_load_explicit(
      (_Atomic mword_t *)&mem->pg_list->buf[addr / _RBC_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _RBC_PAGE_LEN_IN_BYTES_],
      memory_order_relaxed);
  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_read_dword_atm(RBCMemory *mem, maddress_t addr,
                                          mdptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  *store_in = atomic_load_explicit(
      (_Atomic mdword_t *)&mem->pg_list->buf[addr / _RBC_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _RBC_PAGE_LEN_IN_BYTES_],
      memory_order_relaxed);
  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_read_qword_atm(RBCMemory *mem, maddress_t addr,
                                          mqptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  *store_in = atomic_load_explicit(
      (_Atomic mqword_t *)&mem->pg_list->buf[addr / _RBC_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _RBC_PAGE_LEN_IN_BYTES_],
      memory_order_relaxed);
  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_read_bulk(RBCMemory *mem, maddress_t addr,
                                     msize_t len, mbptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  if (surelyF(!len))
    return RBC_MEM_OPER_SUCCESS;
  register maddress_t end = addr + len;
  if (surelyF(end >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  register msize_t pg = addr / _RBC_PAGE_LEN_IN_BYTES_;
  register msize_t off = addr % _RBC_PAGE_LEN_IN_BYTES_;
  register msize_t diff = _RBC_PAGE_LEN_IN_BYTES_ - off;
  mbptr_t iter = store_in;

  //////  FIX THIS SHIT
  if (diff > len) {
    memcpy(store_in, mem->pg_list->buf[pg].repr.bytes + off, len);
  } else {
    memcpy(store_in, mem->pg_list->buf[pg].repr.bytes + off, diff);
    iter += diff;
    len -= diff;
    pg++;
    off = 0;

    while (len > 0) {
      diff = (len < _RBC_PAGE_LEN_IN_BYTES_) ? len : _RBC_PAGE_LEN_IN_BYTES_;
      memcpy(iter, mem->pg_list->buf[pg].repr.bytes, diff);
      iter += diff;
      len -= diff;
      pg++;
    }
  }

  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_write_byte(RBCMemory *mem, maddress_t addr,
                                      mbyte_t store) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  *(mbptr_t)&mem->pg_list->buf[addr / _RBC_PAGE_LEN_IN_BYTES_]
       .repr.bytes[addr % _RBC_PAGE_LEN_IN_BYTES_] = store;
  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_write_word(RBCMemory *mem, maddress_t addr,
                                      mword_t store) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  *(mqptr_t)&mem->pg_list->buf[addr / _RBC_PAGE_LEN_IN_BYTES_]
       .repr.bytes[addr % _RBC_PAGE_LEN_IN_BYTES_] = store;
  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_write_dword(RBCMemory *mem, maddress_t addr,
                                       mdword_t store) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  *(mdptr_t)&mem->pg_list->buf[addr / _RBC_PAGE_LEN_IN_BYTES_]
       .repr.bytes[addr % _RBC_PAGE_LEN_IN_BYTES_] = store;
  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_write_qword(RBCMemory *mem, maddress_t addr,
                                       mqword_t store) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  *(mqptr_t)&mem->pg_list->buf[addr / _RBC_PAGE_LEN_IN_BYTES_]
       .repr.bytes[addr % _RBC_PAGE_LEN_IN_BYTES_] = store;
  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_write_byte_atm(RBCMemory *mem, maddress_t addr,
                                          mbyte_t store) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  atomic_store_explicit(
      (_Atomic mbyte_t *)&mem->pg_list->buf[addr / _RBC_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _RBC_PAGE_LEN_IN_BYTES_],
      store, memory_order_release);
  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_write_word_atm(RBCMemory *mem, maddress_t addr,
                                          mword_t store) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  atomic_store_explicit(
      (_Atomic mword_t *)&mem->pg_list->buf[addr / _RBC_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _RBC_PAGE_LEN_IN_BYTES_],
      store, memory_order_release);
  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_write_dword_atm(RBCMemory *mem, maddress_t addr,
                                           mdword_t store) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  atomic_store_explicit(
      (_Atomic mdword_t *)&mem->pg_list->buf[addr / _RBC_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _RBC_PAGE_LEN_IN_BYTES_],
      store, memory_order_release);
  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_write_qword_atm(RBCMemory *mem, maddress_t addr,
                                           mqword_t store) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  atomic_store_explicit(
      (_Atomic mqword_t *)&mem->pg_list->buf[addr / _RBC_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _RBC_PAGE_LEN_IN_BYTES_],
      store, memory_order_release);
  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_write_bulk(RBCMemory *mem, maddress_t addr,
                                      msize_t len, mbptr_t store) {
  merry_check_ptr(mem);
  merry_check_ptr(store);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  if (surelyF(!len))
    return RBC_MEM_OPER_SUCCESS;
  register maddress_t end = addr + len;
  if (surelyF(end >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  register msize_t pg = addr / _RBC_PAGE_LEN_IN_BYTES_;
  register msize_t off = addr % _RBC_PAGE_LEN_IN_BYTES_;
  register msize_t diff = _RBC_PAGE_LEN_IN_BYTES_ - off;
  mbptr_t iter = store;
  if (diff > len) {
    memcpy(mem->pg_list->buf[pg].repr.bytes + off, store, len);
  } else {
    memcpy(mem->pg_list->buf[pg].repr.bytes + off, store, diff);
    iter += diff;
    len -= diff;
    pg++;
    off = 0;

    while (len > 0) {
      diff = (len < _RBC_PAGE_LEN_IN_BYTES_) ? len : _RBC_PAGE_LEN_IN_BYTES_;
      memcpy(mem->pg_list->buf[pg].repr.bytes, iter, diff);
      iter += diff;
      len -= diff;
      pg++;
    }
  }

  return RBC_MEM_OPER_SUCCESS;
}

rbcmemOperRes_t rbc_memory_cmpxchg(RBCMemory *mem, maddress_t addr, mbyte_t exp,
                                   mbyte_t des) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return RBC_MEM_OPER_ACCESS_INVALID;
  atomic_compare_exchange_strong(
      (_Atomic mbyte_t *)&mem->pg_list->buf[addr / _RBC_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _RBC_PAGE_LEN_IN_BYTES_],
      &exp, des);
  return RBC_MEM_OPER_SUCCESS;
}

void rbc_memory_destroy(RBCMemory *mem) {
  // The pages and the memory buffer is none of RAM's concern
  merry_check_ptr(mem);
  merry_RBCMemPage_list_destroy(mem->pg_list);
  free(mem);
}
