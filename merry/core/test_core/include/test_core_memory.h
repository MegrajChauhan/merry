#ifndef _MERRY_TEST_CORE_MEM_
#define _MERRY_TEST_CORE_MEM_

/*
 * This test core has a 64-bit bus length and can only access memory
 * in 8-bytes and work at 8-byte level
 * */

#include <merry_graves_core_base.h>
#include <merry_graves_request_queue.h>
#include <merry_ram.h>

void merry_test_core_read_64(MerryRAM *ram, MerryCoreBase *base,
                             maddress_t addr, mqptr_t store_in) {
  if (surelyF(merry_RAM_read_qword(ram, addr, store_in, &base->estack) ==
              RET_SUCCESS))
    return;
  if (base->estack.fatality) {
    ERROR(&base->estack);
    // Kill self now
    base->_greq->sys_request = mfalse;
    base->_greq->type = KILL_SELF;
    if (merry_SEND_REQUEST(base->_greq, &base->cond) == RET_FAILURE) {
      base->interrupt = mtrue;
      // base.
    }
  }
}

#endif
