#ifndef _MERRY_RAM_
#define _MERRY_RAM_

#include <merry_config.h>
#include <merry_helpers.h>
#include <merry_list.h>
#include <merry_logger.h>
#include <merry_results.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <merry_core_defs.h>
#include <stdatomic.h>
#include <stdlib.h>

/*
 * It is compulsory that the addresses are page-aligned
 * If some data spans mutliple pages than the program will get
 * incomplete data
 * */
 
typedef struct MerryCoreRAM MerryCoreRAM;
typedef struct MerryCoreRAMPage MerryCoreRAMPage;

struct MerryCoreRAMPage {
	union {
		mbptr_t bytes;
		mwptr_t words;
		mdptr_t dwords;
		mqptr_t qwords;
	} repr;
};

_MERRY_DECLARE_STATIC_LIST_(CoreRAMPage, MerryCoreRAMPage);

struct MerryCoreRAM {
  MerryCoreRAMPageList *pg_list;
  maddress_t max_address;
};

// Initialize a memory(Just Initialization. Pages not populated)
MerryCoreRAM *merry_core_memory_init();

mresult_t merry_core_memory_populate(MerryCoreRAM *mem, msize_t space_len,
                              mbptr_t addr_space);

// Reads
mresult_t merry_core_memory_read_byte(MerryCoreRAM *mem, maddress_t addr,
                                     mbptr_t store_in);
mresult_t merry_core_memory_read_word(MerryCoreRAM *mem, maddress_t addr,
                                     mwptr_t store_in);
mresult_t merry_core_memory_read_dword(MerryCoreRAM *mem, maddress_t addr,
                                      mdptr_t store_in);
mresult_t merry_core_memory_read_qword(MerryCoreRAM *mem, maddress_t addr,
                                      mqptr_t store_in);
mresult_t merry_core_memory_read_byte_atm(MerryCoreRAM *mem, maddress_t addr,
                                         mbptr_t store_in);
mresult_t merry_core_memory_read_word_atm(MerryCoreRAM *mem, maddress_t addr,
                                         mwptr_t store_in);
mresult_t merry_core_memory_read_dword_atm(MerryCoreRAM *mem, maddress_t addr,
                                          mdptr_t store_in);
mresult_t merry_core_memory_read_qword_atm(MerryCoreRAM *mem, maddress_t addr,
                                          mqptr_t store_in);
mresult_t merry_core_memory_read_bulk(MerryCoreRAM *mem, maddress_t addr,
                                     msize_t len, mbptr_t store_in);

// Writes
mresult_t merry_core_memory_write_byte(MerryCoreRAM *mem, maddress_t addr,
                                      mbyte_t store);
mresult_t merry_core_memory_write_word(MerryCoreRAM *mem, maddress_t addr,
                                      mword_t store);
mresult_t merry_core_memory_write_dword(MerryCoreRAM *mem, maddress_t addr,
                                       mdword_t store);
mresult_t merry_core_memory_write_qword(MerryCoreRAM *mem, maddress_t addr,
                                       mqword_t store);
mresult_t merry_core_memory_write_byte_atm(MerryCoreRAM *mem, maddress_t addr,
                                          mbyte_t store);
mresult_t merry_core_memory_write_word_atm(MerryCoreRAM *mem, maddress_t addr,
                                          mword_t store);
mresult_t merry_core_memory_write_dword_atm(MerryCoreRAM *mem, maddress_t addr,
                                           mdword_t store);
mresult_t merry_core_memory_write_qword_atm(MerryCoreRAM *mem, maddress_t addr,
                                           mqword_t store);
mresult_t merry_core_memory_write_bulk(MerryCoreRAM *mem, maddress_t addr,
                                      msize_t len, mbptr_t store);

mresult_t merry_core_memory_cmpxchg(MerryCoreRAM *mem, maddress_t addr, mbyte_t exp,
                                   mbyte_t des);

void merry_core_memory_destroy(MerryCoreRAM *mem);

#endif
