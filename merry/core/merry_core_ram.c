#include <merry_core_ram.h>

_MERRY_DEFINE_STATIC_LIST_(MerryCoreRAMPage, MerryCoreRAMPage);

MerryCoreRAM *merry_memory_init() {
  MerryCoreRAM *mem = (MerryCoreRAM *)malloc(sizeof(MerryCoreRAM));
  if (!mem) {
    MERR("Failed to allocate memory for Core RAM", NULL);
    return RET_NULL;
  }
  mem->pg_list = NULL;
  mem->max_address = 0;
  return mem;
}

mresult_t merry_memory_populate(MerryCoreRAM *mem, msize_t space_len,
                              mbptr_t addr_space) {
  merry_check_ptr(mem);
  merry_check_ptr(addr_space);

  msize_t pg_count =
      space_len /
      _MERRY_PAGE_LEN_IN_BYTES_; // space_len should be aligned most of the time
                                 // until instruction len is extremely small
  if (space_len && (space_len < _MERRY_PAGE_LEN_IN_BYTES_))
    pg_count = 1;

  mresult_t res = merry_MerryCoreRAMPage_list_create(pg_count, &mem->pg_list);
  if (res != MRES_SUCCESS) {
    MERR("Failed to allocate memory for RAM PAGE BUFFER", NULL);
    return res;
  }

  mem->max_address = space_len;

  for (msize_t i = 0; i < pg_count; i++) {
    // Yes! We accessed the buffer directly
    mem->pg_list->buf[i].repr.bytes = addr_space + i * _MERRY_PAGE_LEN_IN_BYTES_;
  }
  return MRES_SUCCESS;
}

mresult_t merry_memory_read_byte(MerryCoreRAM *mem, maddress_t addr,
                                     mbptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  *store_in = mem->pg_list->buf[addr / _MERRY_PAGE_LEN_IN_BYTES_]
                  .repr.bytes[addr % _MERRY_PAGE_LEN_IN_BYTES_];
  return MRES_SUCCESS;
}

mresult_t merry_memory_read_word(MerryCoreRAM *mem, maddress_t addr,
                                     mwptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  *store_in = *(mwptr_t)&mem->pg_list->buf[addr / _MERRY_PAGE_LEN_IN_BYTES_]
                   .repr.bytes[addr % _MERRY_PAGE_LEN_IN_BYTES_];
  return MRES_SUCCESS;
}

mresult_t merry_memory_read_dword(MerryCoreRAM *mem, maddress_t addr,
                                      mdptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  *store_in = *(mdptr_t)&mem->pg_list->buf[addr / _MERRY_PAGE_LEN_IN_BYTES_]
                   .repr.bytes[addr % _MERRY_PAGE_LEN_IN_BYTES_];
  return MRES_SUCCESS;
}

mresult_t merry_memory_read_qword(MerryCoreRAM *mem, maddress_t addr,
                                      mqptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  *store_in = *(mqptr_t)&mem->pg_list->buf[addr / _MERRY_PAGE_LEN_IN_BYTES_]
                   .repr.bytes[addr % _MERRY_PAGE_LEN_IN_BYTES_];
  return MRES_SUCCESS;
}

mresult_t merry_memory_read_byte_atm(MerryCoreRAM *mem, maddress_t addr,
                                         mbptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  *store_in = atomic_load_explicit(
      (_Atomic mbyte_t *)&mem->pg_list->buf[addr / _MERRY_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _MERRY_PAGE_LEN_IN_BYTES_],
      memory_order_relaxed);
  return MRES_SUCCESS;
}

mresult_t merry_memory_read_word_atm(MerryCoreRAM *mem, maddress_t addr,
                                         mwptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  *store_in = atomic_load_explicit(
      (_Atomic mword_t *)&mem->pg_list->buf[addr / _MERRY_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _MERRY_PAGE_LEN_IN_BYTES_],
      memory_order_relaxed);
  return MRES_SUCCESS;
}

mresult_t merry_memory_read_dword_atm(MerryCoreRAM *mem, maddress_t addr,
                                          mdptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  *store_in = atomic_load_explicit(
      (_Atomic mdword_t *)&mem->pg_list->buf[addr / _MERRY_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _MERRY_PAGE_LEN_IN_BYTES_],
      memory_order_relaxed);
  return MRES_SUCCESS;
}

mresult_t merry_memory_read_qword_atm(MerryCoreRAM *mem, maddress_t addr,
                                          mqptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  *store_in = atomic_load_explicit(
      (_Atomic mqword_t *)&mem->pg_list->buf[addr / _MERRY_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _MERRY_PAGE_LEN_IN_BYTES_],
      memory_order_relaxed);
  return MRES_SUCCESS;
}

mresult_t merry_memory_read_bulk(MerryCoreRAM *mem, maddress_t addr,
                                     msize_t len, mbptr_t store_in) {
  merry_check_ptr(mem);
  merry_check_ptr(store_in);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  if (surelyF(!len))
    return MRES_SUCCESS;
  register maddress_t end = addr + len;
  if (surelyF(end >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  register msize_t pg = addr / _MERRY_PAGE_LEN_IN_BYTES_;
  register msize_t off = addr % _MERRY_PAGE_LEN_IN_BYTES_;
  register msize_t diff = _MERRY_PAGE_LEN_IN_BYTES_ - off;
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
      diff = (len < _MERRY_PAGE_LEN_IN_BYTES_) ? len : _MERRY_PAGE_LEN_IN_BYTES_;
      memcpy(iter, mem->pg_list->buf[pg].repr.bytes, diff);
      iter += diff;
      len -= diff;
      pg++;
    }
  }

  return MRES_SUCCESS;
}

mresult_t merry_memory_write_byte(MerryCoreRAM *mem, maddress_t addr,
                                      mbyte_t store) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  *(mbptr_t)&mem->pg_list->buf[addr / _MERRY_PAGE_LEN_IN_BYTES_]
       .repr.bytes[addr % _MERRY_PAGE_LEN_IN_BYTES_] = store;
  return MRES_SUCCESS;
}

mresult_t merry_memory_write_word(MerryCoreRAM *mem, maddress_t addr,
                                      mword_t store) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  *(mqptr_t)&mem->pg_list->buf[addr / _MERRY_PAGE_LEN_IN_BYTES_]
       .repr.bytes[addr % _MERRY_PAGE_LEN_IN_BYTES_] = store;
  return MRES_SUCCESS;
}

mresult_t merry_memory_write_dword(MerryCoreRAM *mem, maddress_t addr,
                                       mdword_t store) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  *(mdptr_t)&mem->pg_list->buf[addr / _MERRY_PAGE_LEN_IN_BYTES_]
       .repr.bytes[addr % _MERRY_PAGE_LEN_IN_BYTES_] = store;
  return MRES_SUCCESS;
}

mresult_t merry_memory_write_qword(MerryCoreRAM *mem, maddress_t addr,
                                       mqword_t store) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  *(mqptr_t)&mem->pg_list->buf[addr / _MERRY_PAGE_LEN_IN_BYTES_]
       .repr.bytes[addr % _MERRY_PAGE_LEN_IN_BYTES_] = store;
  return MRES_SUCCESS;
}

mresult_t merry_memory_write_byte_atm(MerryCoreRAM *mem, maddress_t addr,
                                          mbyte_t store) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  atomic_store_explicit(
      (_Atomic mbyte_t *)&mem->pg_list->buf[addr / _MERRY_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _MERRY_PAGE_LEN_IN_BYTES_],
      store, memory_order_release);
  return MRES_SUCCESS;
}

mresult_t merry_memory_write_word_atm(MerryCoreRAM *mem, maddress_t addr,
                                          mword_t store) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  atomic_store_explicit(
      (_Atomic mword_t *)&mem->pg_list->buf[addr / _MERRY_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _MERRY_PAGE_LEN_IN_BYTES_],
      store, memory_order_release);
  return MRES_SUCCESS;
}

mresult_t merry_memory_write_dword_atm(MerryCoreRAM *mem, maddress_t addr,
                                           mdword_t store) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  atomic_store_explicit(
      (_Atomic mdword_t *)&mem->pg_list->buf[addr / _MERRY_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _MERRY_PAGE_LEN_IN_BYTES_],
      store, memory_order_release);
  return MRES_SUCCESS;
}

mresult_t merry_memory_write_qword_atm(MerryCoreRAM *mem, maddress_t addr,
                                           mqword_t store) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  atomic_store_explicit(
      (_Atomic mqword_t *)&mem->pg_list->buf[addr / _MERRY_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _MERRY_PAGE_LEN_IN_BYTES_],
      store, memory_order_release);
  return MRES_SUCCESS;
}

mresult_t merry_memory_write_bulk(MerryCoreRAM *mem, maddress_t addr,
                                      msize_t len, mbptr_t store) {
  merry_check_ptr(mem);
  merry_check_ptr(store);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  if (surelyF(!len))
    return MRES_SUCCESS;
  register maddress_t end = addr + len;
  if (surelyF(end >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  register msize_t pg = addr / _MERRY_PAGE_LEN_IN_BYTES_;
  register msize_t off = addr % _MERRY_PAGE_LEN_IN_BYTES_;
  register msize_t diff = _MERRY_PAGE_LEN_IN_BYTES_ - off;
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
      diff = (len < _MERRY_PAGE_LEN_IN_BYTES_) ? len : _MERRY_PAGE_LEN_IN_BYTES_;
      memcpy(mem->pg_list->buf[pg].repr.bytes, iter, diff);
      iter += diff;
      len -= diff;
      pg++;
    }
  }

  return MRES_SUCCESS;
}

mresult_t merry_memory_cmpxchg(MerryCoreRAM *mem, maddress_t addr, mbyte_t exp,
                                   mbyte_t des) {
  merry_check_ptr(mem);
  if (surelyF(addr >= mem->max_address))
    return MRES_CORE_RAM_OUT_OF_BOUNDS_ACCESS;
  atomic_compare_exchange_strong(
      (_Atomic mbyte_t *)&mem->pg_list->buf[addr / _MERRY_PAGE_LEN_IN_BYTES_]
          .repr.bytes[addr % _MERRY_PAGE_LEN_IN_BYTES_],
      &exp, des);
  return MRES_SUCCESS;
}

void merry_memory_destroy(MerryCoreRAM *mem) {
  // The pages and the memory buffer is none of RAM's concern
  merry_check_ptr(mem);
  merry_MerryCoreRAMPage_list_destroy(mem->pg_list);
  free(mem);
}
