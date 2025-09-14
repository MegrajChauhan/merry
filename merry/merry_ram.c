#include "merry_ram.h"

MerryRAM *merry_create_RAM(msize_t number_of_pages, MerryErrorStack *st) {
  merry_assert(number_of_pages != 0);

  MerryRAM *ram = (MerryRAM *)malloc(sizeof(MerryRAM));

  if (!ram) {
    PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
         "Allocating a RAM");
    merry_error_stack_fatality(st);
    return RET_NULL;
  }

  MerryNormalMemoryPage **pages = (MerryNormalMemoryPage **)malloc(
      sizeof(MerryNormalMemoryPage *) * number_of_pages);
  if (!pages) {
    PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
         "Allocating a RAM buffer for pages");
    merry_error_stack_fatality(st);
    free(ram);
    return RET_NULL;
  }

  msize_t i = 0;
  for (; i < number_of_pages; i++) {
    if ((pages[i] = merry_create_normal_memory_page(st)) == RET_NULL) {
      PUSH(st, NULL, "Failed to initialize RAM",
           "Populating the RAM buffer for pages");
      merry_error_stack_fatality(st);
      goto __rid_of_during_error;
    }
  }

  ram->pages = pages;
  ram->page_count = number_of_pages;
  return ram;
__rid_of_during_error: // not the best names :(
  for (msize_t j = 0; j < i; j++) {
    merry_return_normal_memory_page(pages[j], st);
  }
  free(pages);
  free(ram);
  return RET_NULL;
}

MerryRAM *merry_copy_RAM(MerryRAM *source, MerryErrorStack *st) {
  merry_check_ptr(source);

  MerryRAM *destination = (MerryRAM *)malloc(sizeof(MerryRAM));

  if (!destination) {
    PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
         "Copying a RAM");
    merry_error_stack_errno(st);
    merry_error_stack_fatality(st); // because this is fatal
    return RET_NULL;
  }

  destination->page_count = source->page_count;

  if ((destination->pages = (MerryNormalMemoryPage **)malloc(
           sizeof(MerryMemoryPageBase *) * destination->page_count)) ==
      RET_NULL) {
    PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
         "Copying a RAM");
    merry_error_stack_errno(st);
    merry_error_stack_fatality(st); // because this is fatal
    free(destination);
    return RET_NULL;
  }

  // memcpy the pointers or just loop
  for (msize_t i = 0; i < destination->page_count; i++)
    destination->pages[i] = source->pages[i];

  return destination;
}

mret_t merry_RAM_append_page(MerryRAM *ram, MerryNormalMemoryPage *page,
                             MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_check_ptr(page);
  merry_assert(ram->page_count != 0);

  register msize_t temp = ram->page_count + 1;
  MerryNormalMemoryPage **pages =
      (MerryNormalMemoryPage **)malloc(sizeof(MerryNormalMemoryPage *) * temp);
  if (!pages) {
    PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
         "Reallocating the RAM buffer for pages");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }

  pages[ram->page_count] = page;

  MerryNormalMemoryPage **tmp = ram->pages;
  ram->pages = pages;
  free(tmp);

  memcpy(pages, ram->pages, sizeof(MerryNormalMemoryPage *) * ram->page_count);

  ram->page_count++;
  return RET_SUCCESS;
}

mret_t merry_RAM_append_pages(MerryRAM *ram, MerryNormalMemoryPage **pages,
                              msize_t count, MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_check_ptr(pages);
  merry_assert(ram->page_count != 0);
  merry_assert(count != 0);

  register msize_t temp = ram->page_count + count;
  MerryNormalMemoryPage **p =
      (MerryNormalMemoryPage **)malloc(sizeof(MerryNormalMemoryPage *) * temp);
  if (!pages) {
    PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
         "Reallocating the RAM buffer for pages");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }

  for (msize_t i = 0; i < count; i++) {
    pages[ram->page_count + i] = p[i];
  }

  memcpy(p, ram->pages, sizeof(MerryNormalMemoryPage *) * ram->page_count);

  ram->page_count += count;

  MerryNormalMemoryPage **tmp = ram->pages;
  ram->pages = pages;
  free(tmp);

  return RET_SUCCESS;
}

mret_t merry_RAM_read_byte(MerryRAM *ram, maddress_t address, mbptr_t store_in,
                           MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_check_ptr(store_in);
  merry_assert(ram->page_count != 0);

  register msize_t page_num;
  register msize_t page_off;

  merry_deduce_address(address, page_num, page_off);
  merry_memory_access_bounds_check(st, ram, page_num);

  *store_in = ram->pages[page_num]->buf[page_off];
  return RET_SUCCESS;
}

mret_t merry_RAM_read_word(MerryRAM *ram, maddress_t address, mwptr_t store_in,
                           MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_check_ptr(store_in);
  merry_assert(ram->page_count != 0);

  register msize_t page_num;
  register msize_t page_off;
  register MerryHostMemLayout layout;

  merry_deduce_address(address, page_num, page_off);
  merry_memory_access_bounds_check(st, ram, page_num);

  switch (page_off) {
  case _MERRY_PAGE_LEN_ - 1: {
    merry_memory_access_bounds_check(st, ram, page_num + 1);
    layout.bytes.b0 = ram->pages[page_num]->buf[page_off];
    layout.bytes.b1 = ram->pages[page_num + 1]->buf[0];
    break;
  }
  default:
    layout.half_half_words.w0 =
        *(mwptr_t)(ram->pages[page_num]->buf + page_off);
    break;
  }

  *store_in = layout.half_half_words.w0;
  return RET_SUCCESS;
}

mret_t merry_RAM_read_dword(MerryRAM *ram, maddress_t address, mdptr_t store_in,
                            MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_check_ptr(store_in);
  merry_assert(ram->page_count != 0);

  register msize_t page_num;
  register msize_t page_off;
  register MerryHostMemLayout layout;

  merry_deduce_address(address, page_num, page_off);
  merry_memory_access_bounds_check(st, ram, page_num);

  switch (page_off) {

  case _MERRY_PAGE_LEN_ - 3:
  case _MERRY_PAGE_LEN_ - 2:
  case _MERRY_PAGE_LEN_ - 1: {
    merry_memory_access_bounds_check(st, ram, page_num + 1);
    for (msize_t i = page_off; i < _MERRY_PAGE_LEN_; i++) {
      layout.half_words.w0 |= ram->pages[page_num]->buf[i];
      layout.half_words.w0 <<= 8;
    }
    page_num++;
    msize_t i = 0;
    for (; i < 3 - (_MERRY_PAGE_LEN_ - page_off); i++) {
      layout.half_words.w0 |= ram->pages[page_num]->buf[i];
      layout.half_words.w0 <<= 8;
    }
    layout.half_words.w0 |= ram->pages[page_num]->buf[i];
    break;
  }
  default:
    layout.half_words.w0 = *(mdptr_t)(ram->pages[page_num]->buf + page_off);
    break;
  }

  *store_in = layout.half_words.w0;
  return RET_SUCCESS;
}

mret_t merry_RAM_read_qword(MerryRAM *ram, maddress_t address, mqptr_t store_in,
                            MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_check_ptr(store_in);
  merry_assert(ram->page_count != 0);

  register msize_t page_num;
  register msize_t page_off;
  register MerryHostMemLayout layout;

  merry_deduce_address(address, page_num, page_off);
  merry_memory_access_bounds_check(st, ram, page_num);

  switch (page_off) {

  case _MERRY_PAGE_LEN_ - 7:
  case _MERRY_PAGE_LEN_ - 6:
  case _MERRY_PAGE_LEN_ - 5:
  case _MERRY_PAGE_LEN_ - 4:
  case _MERRY_PAGE_LEN_ - 3:
  case _MERRY_PAGE_LEN_ - 2:
  case _MERRY_PAGE_LEN_ - 1: {
    merry_memory_access_bounds_check(st, ram, page_num + 1);
    for (msize_t i = page_off; i < _MERRY_PAGE_LEN_; i++) {
      layout.whole_word |= ram->pages[page_num]->buf[i];
      layout.whole_word <<= 8;
    }
    page_num++;
    msize_t i = 0;
    for (; i < 7 - (_MERRY_PAGE_LEN_ - page_off); i++) {
      layout.whole_word |= ram->pages[page_num]->buf[i];
      layout.whole_word <<= 8;
    }
    layout.whole_word |= ram->pages[page_num]->buf[i];
    break;
  }
  default:
    layout.whole_word = *(mqptr_t)(ram->pages[page_num]->buf + page_off);
    break;
  }
  *store_in = layout.whole_word;
  return RET_SUCCESS;
}

mret_t merry_RAM_write_byte(MerryRAM *ram, maddress_t address, mbyte_t value,
                            MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_assert(ram->page_count != 0);

  register msize_t page_num;
  register msize_t page_off;

  merry_deduce_address(address, page_num, page_off);
  merry_memory_access_bounds_check(st, ram, page_num);

  ram->pages[page_num]->buf[page_off] = value;
  return RET_SUCCESS;
}

mret_t merry_RAM_write_word(MerryRAM *ram, maddress_t address, mword_t value,
                            MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_assert(ram->page_count != 0);

  register msize_t page_num;
  register msize_t page_off;

  merry_deduce_address(address, page_num, page_off);
  merry_memory_access_bounds_check(st, ram, page_num);

  switch (page_off) {
  case _MERRY_PAGE_LEN_ - 1: {
    register MerryHostMemLayout layout;
    layout.half_half_words.w0 = value;
    merry_memory_access_bounds_check(st, ram, page_num + 1);
    ram->pages[page_num]->buf[page_off] = layout.bytes.b0;
    ram->pages[page_num + 1]->buf[0] = layout.bytes.b1;
    break;
  }
  default:
    *(mwptr_t)(ram->pages[page_num]->buf + page_off) = value;
    break;
  }

  return RET_SUCCESS;
}

mret_t merry_RAM_write_dword(MerryRAM *ram, maddress_t address, mdword_t value,
                             MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_assert(ram->page_count != 0);

  register msize_t page_num;
  register msize_t page_off;

  merry_deduce_address(address, page_num, page_off);
  merry_memory_access_bounds_check(st, ram, page_num);

  switch (page_off) {

  case _MERRY_PAGE_LEN_ - 3:
  case _MERRY_PAGE_LEN_ - 2:
  case _MERRY_PAGE_LEN_ - 1: {
    register MerryHostMemLayout layout;
    layout.half_words.w0 = value;
    merry_memory_access_bounds_check(st, ram, page_num + 1);
    for (msize_t i = page_off; i < _MERRY_PAGE_LEN_; i++) {
      ram->pages[page_num]->buf[i] = layout.half_words.w0 & 255;
      layout.half_words.w0 >>= 8;
    }
    page_num++;
    msize_t i = 0;
    for (; i < 4 - (_MERRY_PAGE_LEN_ - page_off); i++) {
      ram->pages[page_num]->buf[i] = layout.half_words.w0 & 255;
      layout.half_words.w0 >>= 8;
    }
    break;
  }
  default:
    *(mdptr_t)(ram->pages[page_num]->buf + page_off) = value;
    break;
  }

  return RET_SUCCESS;
}

mret_t merry_RAM_write_qword(MerryRAM *ram, maddress_t address, mqword_t value,
                             MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_assert(ram->page_count != 0);

  register msize_t page_num;
  register msize_t page_off;

  merry_deduce_address(address, page_num, page_off);
  merry_memory_access_bounds_check(st, ram, page_num);

  switch (page_off) {

  case _MERRY_PAGE_LEN_ - 7:
  case _MERRY_PAGE_LEN_ - 6:
  case _MERRY_PAGE_LEN_ - 5:
  case _MERRY_PAGE_LEN_ - 4:
  case _MERRY_PAGE_LEN_ - 3:
  case _MERRY_PAGE_LEN_ - 2:
  case _MERRY_PAGE_LEN_ - 1: {
    register MerryHostMemLayout layout;
    layout.whole_word = value;
    merry_memory_access_bounds_check(st, ram, page_num + 1);

    for (msize_t i = page_off; i < _MERRY_PAGE_LEN_; i++) {
      ram->pages[page_num]->buf[i] = layout.whole_word & 255;
      layout.whole_word >>= 8;
    }
    page_num++;
    msize_t i = 0;
    for (; i < 8 - (_MERRY_PAGE_LEN_ - page_off); i++) {
      ram->pages[page_num]->buf[i] = layout.whole_word & 255;
      layout.whole_word >>= 8;
    }
    break;
  }
  default:
    *(mqptr_t)(ram->pages[page_num]->buf + page_off) = value;
    break;
  }

  return RET_SUCCESS;
}

mbptr_t merry_RAM_bulk_read(MerryRAM *ram, maddress_t address, msize_t length,
                            MerryErrorStack *st) {
  merry_check_ptr(ram);
  if (length == 0) {
    PUSH(st, "Invalid Bulk Length", "Bulk length cannot be 0",
         "Reading in Bulk");
    merry_error_stack_fatality(st);
    return RET_NULL;
  }

  mbptr_t buf = (mbptr_t)malloc(length);
  mbptr_t iter = buf;

  if (!buf) {
    PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
         "Allocating Bulk buffer for Reading");
    merry_error_stack_fatality(st);
    return RET_NULL;
  }

  msize_t pg_num;
  msize_t off;

  merry_deduce_address(address, pg_num, off);
  while (length > 0) {
    merry_memory_access_bounds_check(st, ram, pg_num);

    if (length > _MERRY_PAGE_LEN_) {
      memcpy(iter, (char *)ram->pages[pg_num]->buf + off, _MERRY_PAGE_LEN_);
      length -= _MERRY_PAGE_LEN_;
      iter += _MERRY_PAGE_LEN_;
    } else {
      memcpy(iter, (char *)ram->pages[pg_num]->buf + off, length);
      iter += length;
      length -= length;
    }
    pg_num++;
    off = 0;
  }
  return buf;
}

mret_t merry_RAM_bulk_write(MerryRAM *ram, maddress_t address, msize_t length,
                            mbptr_t to_write, MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(to_write);

  if (length == 0) {
    PUSH(st, "Invalid Bulk Length", "Bulk length cannot be 0",
         "Writing in Bulk");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }

  mbptr_t iter = to_write;

  msize_t pg_num;
  msize_t off;

  merry_deduce_address(address, pg_num, off);

  while (length > 0) {
    merry_memory_access_bounds_check(st, ram, pg_num);

    if (length > _MERRY_PAGE_LEN_) {
      memcpy((char *)ram->pages[pg_num]->buf + off, iter, _MERRY_PAGE_LEN_);
      length -= _MERRY_PAGE_LEN_;
      iter += _MERRY_PAGE_LEN_;
    } else {
      memcpy((char *)ram->pages[pg_num]->buf + off, iter, length);
      iter += length;
      length -= length;
    }
    pg_num++;
    off = 0;
  }

  return RET_SUCCESS;
}

mret_t merry_RAM_read_byte_atm(MerryRAM *ram, maddress_t address,
                               mbptr_t store_in, MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_check_ptr(store_in);
  merry_assert(ram->page_count != 0);

  register msize_t page_num;
  register msize_t page_off;

  merry_deduce_address(address, page_num, page_off);
  merry_memory_access_bounds_check(st, ram, page_num);

  *store_in = atomic_load((atomic_char *)&ram->pages[page_num]->buf[page_off]);
  return RET_SUCCESS;
}

mret_t merry_RAM_read_word_atm(MerryRAM *ram, maddress_t address,
                               mwptr_t store_in, MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_check_ptr(store_in);
  merry_assert(ram->page_count != 0);

  register msize_t page_num;
  register msize_t page_off;
  register MerryHostMemLayout layout;

  merry_deduce_address(address, page_num, page_off);
  merry_memory_access_bounds_check(st, ram, page_num);

  switch (page_off) {
  case _MERRY_PAGE_LEN_ - 1: {
    merry_memory_access_bounds_check(st, ram, page_num + 1);
    layout.bytes.b0 =
        atomic_load((atomic_char *)&ram->pages[page_num]->buf[page_off]);
    layout.bytes.b1 =
        atomic_load((atomic_char *)&ram->pages[page_num + 1]->buf[0]);
    break;
  }
  default:
    layout.half_half_words.w0 =
        atomic_load((atomic_char16_t *)(ram->pages[page_num]->buf + page_off));
    break;
  }

  *store_in = layout.half_half_words.w0;
  return RET_SUCCESS;
}

mret_t merry_RAM_read_dword_atm(MerryRAM *ram, maddress_t address,
                                mdptr_t store_in, MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_check_ptr(store_in);
  merry_assert(ram->page_count != 0);

  register msize_t page_num;
  register msize_t page_off;
  register MerryHostMemLayout layout;

  merry_deduce_address(address, page_num, page_off);
  merry_memory_access_bounds_check(st, ram, page_num);

  switch (page_off) {

  case _MERRY_PAGE_LEN_ - 3:
  case _MERRY_PAGE_LEN_ - 2:
  case _MERRY_PAGE_LEN_ - 1: {
    merry_memory_access_bounds_check(st, ram, page_num + 1);
    for (msize_t i = page_off; i < _MERRY_PAGE_LEN_; i++) {
      layout.half_words.w0 |=
          atomic_load((atomic_char *)&ram->pages[page_num]->buf[i]);
      layout.half_words.w0 <<= 8;
    }
    page_num++;
    msize_t i = 0;
    for (; i < 3 - (_MERRY_PAGE_LEN_ - page_off); i++) {
      layout.half_words.w0 |=
          atomic_load((atomic_char *)&ram->pages[page_num]->buf[i]);
      layout.half_words.w0 <<= 8;
    }
    layout.half_words.w0 |=
        atomic_load((atomic_char *)&ram->pages[page_num]->buf[i]);
    break;
  }
  default:
    layout.half_words.w0 =
        atomic_load((atomic_int *)(ram->pages[page_num]->buf + page_off));
    break;
  }

  *store_in = layout.half_words.w0;
  return RET_SUCCESS;
}

mret_t merry_RAM_read_qword_atm(MerryRAM *ram, maddress_t address,
                                mqptr_t store_in, MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_check_ptr(store_in);
  merry_assert(ram->page_count != 0);

  register msize_t page_num;
  register msize_t page_off;
  register MerryHostMemLayout layout;

  merry_deduce_address(address, page_num, page_off);
  merry_memory_access_bounds_check(st, ram, page_num);

  switch (page_off) {

  case _MERRY_PAGE_LEN_ - 7:
  case _MERRY_PAGE_LEN_ - 6:
  case _MERRY_PAGE_LEN_ - 5:
  case _MERRY_PAGE_LEN_ - 4:
  case _MERRY_PAGE_LEN_ - 3:
  case _MERRY_PAGE_LEN_ - 2:
  case _MERRY_PAGE_LEN_ - 1: {
    merry_memory_access_bounds_check(st, ram, page_num + 1);
    for (msize_t i = page_off; i < _MERRY_PAGE_LEN_; i++) {
      layout.whole_word |=
          atomic_load((atomic_char *)&ram->pages[page_num]->buf[i]);
      layout.whole_word <<= 8;
    }
    page_num++;
    msize_t i = 0;
    for (; i < 7 - (_MERRY_PAGE_LEN_ - page_off); i++) {
      layout.whole_word |=
          atomic_load((atomic_char *)&ram->pages[page_num]->buf[i]);
      layout.whole_word <<= 8;
    }
    layout.whole_word |=
        atomic_load((atomic_char *)&ram->pages[page_num]->buf[i]);
    break;
  }
  default:
    layout.whole_word =
        atomic_load((atomic_long *)(ram->pages[page_num]->buf + page_off));
    break;
  }

  *store_in = layout.whole_word;
  return RET_SUCCESS;
}

mret_t merry_RAM_write_byte_atm(MerryRAM *ram, maddress_t address,
                                mbyte_t value, MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_assert(ram->page_count != 0);

  register msize_t page_num;
  register msize_t page_off;

  merry_deduce_address(address, page_num, page_off);
  merry_memory_access_bounds_check(st, ram, page_num);

  atomic_store((atomic_char *)&ram->pages[page_num]->buf[page_off], value);
  return RET_SUCCESS;
}

mret_t merry_RAM_write_word_atm(MerryRAM *ram, maddress_t address,
                                mword_t value, MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_assert(ram->page_count != 0);

  register msize_t page_num;
  register msize_t page_off;

  merry_deduce_address(address, page_num, page_off);
  merry_memory_access_bounds_check(st, ram, page_num);

  switch (page_off) {
  case _MERRY_PAGE_LEN_ - 1: {
    register MerryHostMemLayout layout;
    layout.half_half_words.w0 = value;

    merry_memory_access_bounds_check(st, ram, page_num + 1);
    atomic_store((atomic_char *)&ram->pages[page_num]->buf[page_off],
                 layout.bytes.b0);
    atomic_store((atomic_char *)&ram->pages[page_num + 1]->buf[0],
                 layout.bytes.b1);
    break;
  }
  default:
    atomic_store((atomic_short *)(ram->pages[page_num]->buf + page_off), value);
    break;
  }

  return RET_SUCCESS;
}

mret_t merry_RAM_write_dword_atm(MerryRAM *ram, maddress_t address,
                                 mdword_t value, MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_assert(ram->page_count != 0);

  register msize_t page_num;
  register msize_t page_off;

  merry_deduce_address(address, page_num, page_off);

  merry_memory_access_bounds_check(st, ram, page_num);

  switch (page_off) {

  case _MERRY_PAGE_LEN_ - 3:
  case _MERRY_PAGE_LEN_ - 2:
  case _MERRY_PAGE_LEN_ - 1: {
    register MerryHostMemLayout layout;
    layout.half_words.w0 = value;

    merry_memory_access_bounds_check(st, ram, page_num + 1);
    for (msize_t i = page_off; i < _MERRY_PAGE_LEN_; i++) {
      atomic_store((atomic_char *)&ram->pages[page_num]->buf[i],
                   layout.half_words.w0 & 255);
      layout.half_words.w0 >>= 8;
    }
    page_num++;
    msize_t i = 0;
    for (; i < 4 - (_MERRY_PAGE_LEN_ - page_off); i++) {
      atomic_store((atomic_char *)&ram->pages[page_num]->buf[i],
                   layout.half_words.w0 & 255);
      layout.half_words.w0 >>= 8;
    }
    break;
  }
  default:
    atomic_store((atomic_int *)(ram->pages[page_num]->buf + page_off), value);
    break;
  }

  return RET_SUCCESS;
}

mret_t merry_RAM_write_qword_atm(MerryRAM *ram, maddress_t address,
                                 mqword_t value, MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_assert(ram->page_count != 0);

  register msize_t page_num;
  register msize_t page_off;

  merry_deduce_address(address, page_num, page_off);

  merry_memory_access_bounds_check(st, ram, page_num);

  switch (page_off) {

  case _MERRY_PAGE_LEN_ - 7:
  case _MERRY_PAGE_LEN_ - 6:
  case _MERRY_PAGE_LEN_ - 5:
  case _MERRY_PAGE_LEN_ - 4:
  case _MERRY_PAGE_LEN_ - 3:
  case _MERRY_PAGE_LEN_ - 2:
  case _MERRY_PAGE_LEN_ - 1: {
    register MerryHostMemLayout layout;
    layout.whole_word = value;
    merry_memory_access_bounds_check(st, ram, page_num + 1);
    // Would this work?
    // atomic_store((atomic_uint_fast64_t*)&ram->pages[page_num]->buf[i],
    // layout.whole_word);
    for (msize_t i = page_off; i < _MERRY_PAGE_LEN_; i++) {
      atomic_store((atomic_char *)&ram->pages[page_num]->buf[i],
                   layout.whole_word & 255);
      layout.whole_word >>= 8;
    }
    page_num++;
    msize_t i = 0;
    for (; i < 8 - (_MERRY_PAGE_LEN_ - page_off); i++) {
      atomic_store((atomic_char *)&ram->pages[page_num]->buf[i],
                   layout.whole_word & 255);
      layout.whole_word >>= 8;
    }
    break;
  }
  default:
    atomic_store((atomic_long *)(ram->pages[page_num]->buf + page_off), value);
    break;
  }

  return RET_SUCCESS;
}

mret_t merry_RAM_cmpxchg(MerryRAM *ram, maddress_t address, mbyte_t expected,
                         mbyte_t desired, MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);
  merry_assert(ram->page_count != 0);

  register msize_t page_num;
  register msize_t page_off;

  merry_deduce_address(address, page_num, page_off);
  merry_memory_access_bounds_check(st, ram, page_num);

  atomic_compare_exchange_strong(
      (atomic_char *)&ram->pages[page_num]->buf[page_off], (char *)&expected,
      desired);
  return RET_SUCCESS;
}

void merry_destroy_RAM(MerryRAM *ram, MerryErrorStack *st) {
  merry_check_ptr(ram);
  merry_check_ptr(ram->pages);

  for (msize_t i = 0; i < ram->page_count; i++) {
    merry_check_ptr(ram->pages[i]);
    merry_return_normal_memory_page(ram->pages[i], st);
  }

  free(ram->pages);
  free(ram);
}
