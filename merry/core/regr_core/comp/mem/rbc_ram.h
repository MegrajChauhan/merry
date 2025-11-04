#ifndef _RBC_RAM_
#define _RBC_RAM_

#include <merry_config.h>
#include <merry_helpers.h>
#include <merry_list.h>
#include <merry_logger.h>
#include <merry_memory.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <regr_core/def/consts/rbc_mem_defs.h>
#include <regr_core/def/declr/rbc_mem_declr.h>
#include <stdatomic.h>
#include <stdlib.h>

/*
 * It is compulsory that the addresses are page-aligned
 * If some data spans mutliple pages than the program will get
 * incomplete data
 * */

_MERRY_DECLARE_STATIC_LIST_(RBCMemPage, RBCMemPage);

typedef struct RBCMemory RBCMemory;

struct RBCMemory {
  MerryRBCMemPageList *pg_list;
  maddress_t max_address;
};

// Initialize a memory(Just Initialization. Pages not populated)
RBCMemory *rbc_memory_init();

mret_t rbc_memory_populate(RBCMemory *mem, msize_t space_len,
                           mbptr_t addr_space);

// Reads
rbcmemOperRes_t rbc_memory_read_byte(RBCMemory *mem, maddress_t addr,
                                     mbptr_t store_in);
rbcmemOperRes_t rbc_memory_read_word(RBCMemory *mem, maddress_t addr,
                                     mwptr_t store_in);
rbcmemOperRes_t rbc_memory_read_dword(RBCMemory *mem, maddress_t addr,
                                      mdptr_t store_in);
rbcmemOperRes_t rbc_memory_read_qword(RBCMemory *mem, maddress_t addr,
                                      mqptr_t store_in);
rbcmemOperRes_t rbc_memory_read_byte_atm(RBCMemory *mem, maddress_t addr,
                                         mbptr_t store_in);
rbcmemOperRes_t rbc_memory_read_word_atm(RBCMemory *mem, maddress_t addr,
                                         mwptr_t store_in);
rbcmemOperRes_t rbc_memory_read_dword_atm(RBCMemory *mem, maddress_t addr,
                                          mdptr_t store_in);
rbcmemOperRes_t rbc_memory_read_qword_atm(RBCMemory *mem, maddress_t addr,
                                          mqptr_t store_in);
rbcmemOperRes_t rbc_memory_read_bulk(RBCMemory *mem, maddress_t addr,
                                     msize_t len, mbptr_t store_in);

// Writes
rbcmemOperRes_t rbc_memory_write_byte(RBCMemory *mem, maddress_t addr,
                                      mbyte_t store);
rbcmemOperRes_t rbc_memory_write_word(RBCMemory *mem, maddress_t addr,
                                      mword_t store);
rbcmemOperRes_t rbc_memory_write_dword(RBCMemory *mem, maddress_t addr,
                                       mdword_t store);
rbcmemOperRes_t rbc_memory_write_qword(RBCMemory *mem, maddress_t addr,
                                       mqword_t store);
rbcmemOperRes_t rbc_memory_write_byte_atm(RBCMemory *mem, maddress_t addr,
                                          mbyte_t store);
rbcmemOperRes_t rbc_memory_write_word_atm(RBCMemory *mem, maddress_t addr,
                                          mword_t store);
rbcmemOperRes_t rbc_memory_write_dword_atm(RBCMemory *mem, maddress_t addr,
                                           mdword_t store);
rbcmemOperRes_t rbc_memory_write_qword_atm(RBCMemory *mem, maddress_t addr,
                                           mqword_t store);
rbcmemOperRes_t rbc_memory_write_bulk(RBCMemory *mem, maddress_t addr,
                                      msize_t len, mbptr_t store);

rbcmemOperRes_t rbc_memory_cmpxchg(RBCMemory *mem, maddress_t addr, mbyte_t exp,
                                   mbyte_t des);

void rbc_memory_destroy(RBCMemory *mem);

#endif
