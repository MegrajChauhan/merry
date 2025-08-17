#ifndef _MERRY_RAM_
#define _MERRY_RAM_

/**
 * The way we will define things here is:
 * 1) Either a core can have a private RAM that only they can use
 * 2) Or the core will share the same RAM with other cores
 * The tradeoff is that if the core has a private RAM than it is not allowed to
 * access the shared RAM. The shared RAM is managed by graves. We could have
 * methods in place such as shared memory that the core may use to communicate
 * with the shared RAM
 */

#include <merry_error_stack.h>
#include <merry_helpers.h>
#include <merry_memory.h>
#include <merry_protectors.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdatomic.h>
#include <stdlib.h>

/**
 * The addresses are 64-bit and one page has _MERRY_PAGE_LEN_ addresses.
 * To any core or program, the RAM is going to be an array which is just linear
 * with no breaks but the RAM will hide those details
 */

#define merry_deduce_address(addr, page_num, offset)                           \
  do {                                                                         \
    (page_num) = (addr) / _MERRY_PAGE_LEN_;                                    \
    (offset) = (addr) % _MERRY_PAGE_LEN_;                                      \
  } while (0)

#define merry_memory_access_bounds_check(st, ram, page_num)                    \
  do {                                                                         \
    if (surelyF((page_num) >= (ram)->page_count)) {                            \
      PUSH(st, "Page Fault", "Accessing out of bounds memory",                 \
           "Accessing the memory");                                            \
      merry_error_stack_fatality(st);                                          \
      return RET_FAILURE;                                                      \
    }                                                                          \
    if (surelyF(!(ram)->pages[(page_num)]->init)) {                            \
      return RET_FAILURE;                                                      \
    }                                                                          \
  } while (0)

typedef struct MerryRAM MerryRAM;

struct MerryRAM {
  MerryNormalMemoryPage **pages; // all of the pages
  msize_t page_count;            // Number of pages
};

// Builds a RAM that has default flags and properties set(PUBLIC RAM)
MerryRAM *merry_create_RAM(msize_t number_of_pages, MerryErrorStack *st);

MerryRAM *merry_copy_RAM(MerryRAM *source, MerryErrorStack *st);

mret_t merry_RAM_append_page(MerryRAM *ram, MerryNormalMemoryPage *page,
                             MerryErrorStack *st);

mret_t merry_RAM_append_pages(MerryRAM *ram, MerryNormalMemoryPage **pages,
                              msize_t count, MerryErrorStack *st);

mret_t merry_RAM_read_byte(MerryRAM *ram, maddress_t address, mbptr_t store_in,
                           MerryErrorStack *st);

mret_t merry_RAM_read_word(MerryRAM *ram, maddress_t address, mwptr_t store_in,
                           MerryErrorStack *st);

mret_t merry_RAM_read_dword(MerryRAM *ram, maddress_t address, mdptr_t store_in,
                            MerryErrorStack *st);

mret_t merry_RAM_read_qword(MerryRAM *ram, maddress_t address, mqptr_t store_in,
                            MerryErrorStack *st);

mret_t merry_RAM_write_byte(MerryRAM *ram, maddress_t address, mbyte_t value,
                            MerryErrorStack *st);

mret_t merry_RAM_write_word(MerryRAM *ram, maddress_t address, mword_t value,
                            MerryErrorStack *st);

mret_t merry_RAM_write_dword(MerryRAM *ram, maddress_t address, mdword_t value,
                             MerryErrorStack *st);

mret_t merry_RAM_write_qword(MerryRAM *ram, maddress_t address, mqword_t value,
                             MerryErrorStack *st);

mret_t merry_RAM_read_byte_atm(MerryRAM *ram, maddress_t address,
                               mbptr_t store_in, MerryErrorStack *st);

mret_t merry_RAM_read_word_atm(MerryRAM *ram, maddress_t address,
                               mwptr_t store_in, MerryErrorStack *st);

mret_t merry_RAM_read_dword_atm(MerryRAM *ram, maddress_t address,
                                mdptr_t store_in, MerryErrorStack *st);

mret_t merry_RAM_read_qword_atm(MerryRAM *ram, maddress_t address,
                                mqptr_t store_in, MerryErrorStack *st);

mret_t merry_RAM_write_byte_atm(MerryRAM *ram, maddress_t address,
                                mbyte_t value, MerryErrorStack *st);

mret_t merry_RAM_write_word_atm(MerryRAM *ram, maddress_t address,
                                mword_t value, MerryErrorStack *st);

mret_t merry_RAM_write_dword_atm(MerryRAM *ram, maddress_t address,
                                 mdword_t value, MerryErrorStack *st);

mret_t merry_RAM_write_qword_atm(MerryRAM *ram, maddress_t address,
                                 mqword_t value, MerryErrorStack *st);

mbptr_t merry_RAM_bulk_read(MerryRAM *ram, maddress_t address, msize_t length,
                            MerryErrorStack *st);

mret_t merry_RAM_bulk_write(MerryRAM *ram, maddress_t address, msize_t length,
                            mbptr_t to_write, MerryErrorStack *st);

mret_t merry_RAM_cmpxchg(MerryRAM *ram, maddress_t address, mbyte_t expected,
                         mbyte_t desired, MerryErrorStack *st);

void merry_destroy_RAM(MerryRAM *ram, MerryErrorStack *st);

#endif
