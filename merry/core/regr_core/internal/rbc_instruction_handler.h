#ifndef _RBC_INSTRUCTION_HANDLER_
#define _RBC_INSTRUCTION_HANDLER_

#include <merry_flags_regr.h>
#include <merry_graves_request_queue.h>
#include <regr_core/internal/rbc.h>

#define rbc_ihdlr(name)                                                        \
  void rbc_i##name(RBCCoreBase *base, _Atomic mbool_t *kcore)
#define rbc_ihdlrX(name)                                                       \
  void rbc_i##name(RBCCoreBase *base, _Atomic mbool_t *kcore,                  \
                   mqword_t instruction)

rbc_ihdlr(sysint);
rbc_ihdlr(mint);
rbc_ihdlrX(add);
rbc_ihdlrX(sub);
rbc_ihdlrX(mul);
rbc_ihdlrX(div);

rbc_ihdlrX(add_imm);
rbc_ihdlrX(add_reg);
rbc_ihdlrX(sub_imm);
rbc_ihdlrX(sub_reg);
rbc_ihdlrX(mul_imm);
rbc_ihdlrX(mul_reg);
rbc_ihdlrX(div_imm);
rbc_ihdlrX(div_reg);
rbc_ihdlrX(mod_imm);
rbc_ihdlrX(mod_reg);

rbc_ihdlrX(fadd);
rbc_ihdlrX(fsub);
rbc_ihdlrX(fmul);
rbc_ihdlrX(fdiv);

rbc_ihdlrX(fadd32);
rbc_ihdlrX(fsub32);
rbc_ihdlrX(fmul32);
rbc_ihdlrX(fdiv32);

rbc_ihdlrX(add_memb);
rbc_ihdlrX(add_memw);
rbc_ihdlrX(add_memd);
rbc_ihdlrX(add_memq);

rbc_ihdlrX(sub_memb);
rbc_ihdlrX(sub_memw);
rbc_ihdlrX(sub_memd);
rbc_ihdlrX(sub_memq);

rbc_ihdlrX(mul_memb);
rbc_ihdlrX(mul_memw);
rbc_ihdlrX(mul_memd);
rbc_ihdlrX(mul_memq);

rbc_ihdlrX(div_memb);
rbc_ihdlrX(div_memw);
rbc_ihdlrX(div_memd);
rbc_ihdlrX(div_memq);

rbc_ihdlrX(mod_memb);
rbc_ihdlrX(mod_memw);
rbc_ihdlrX(mod_memd);
rbc_ihdlrX(mod_memq);

rbc_ihdlrX(fadd64_mem);
rbc_ihdlrX(fsub64_mem);
rbc_ihdlrX(fmul64_mem);
rbc_ihdlrX(fdiv64_mem);

rbc_ihdlrX(fadd32_mem);
rbc_ihdlrX(fsub32_mem);
rbc_ihdlrX(fmul32_mem);
rbc_ihdlrX(fdiv32_mem);

rbc_ihdlrX(mov_imm);
rbc_ihdlrX(movf32);

// mov_reg, mov_reg8, mov_reg16, mov_reg32 don't need a dedicated function

rbc_ihdlrX(movesx_imm8);
rbc_ihdlrX(movesx_imm16);
rbc_ihdlrX(movesx_imm32);
rbc_ihdlrX(movesx_reg8);
rbc_ihdlrX(movesx_reg16);
rbc_ihdlrX(movesx_reg32);

rbc_ihdlrX(excg);
rbc_ihdlrX(excg8);
rbc_ihdlrX(excg16);
rbc_ihdlrX(excg32);

// mov8, mov16, mov32 also won't need dedicated functions
// conditional moves also won't require dedicated functions

rbc_ihdlrX(call);
rbc_ihdlrX(ret);
rbc_ihdlrX(call_reg);

rbc_ihdlrX(push_immb);
rbc_ihdlrX(push_immw);
rbc_ihdlrX(push_immd);
rbc_ihdlrX(push_immq);
rbc_ihdlrX(push_reg);
rbc_ihdlrX(popb);
rbc_ihdlrX(popw);
rbc_ihdlrX(popd);
rbc_ihdlrX(popq);
rbc_ihdlrX(pusha);
rbc_ihdlrX(popa);

rbc_ihdlrX(push_memb);
rbc_ihdlrX(push_memw);
rbc_ihdlrX(push_memd);
rbc_ihdlrX(push_memq);
rbc_ihdlrX(pop_memb);
rbc_ihdlrX(pop_memw);
rbc_ihdlrX(pop_memd);
rbc_ihdlrX(pop_memq);

rbc_ihdlrX(loadsb);
rbc_ihdlrX(loadsw);
rbc_ihdlrX(loadsd);
rbc_ihdlrX(loadsq);

rbc_ihdlrX(storesb);
rbc_ihdlrX(storesw);
rbc_ihdlrX(storesd);
rbc_ihdlrX(storesq);

rbc_ihdlrX(and_imm);
rbc_ihdlrX(or_imm);
rbc_ihdlrX(xor_imm);

rbc_ihdlrX(cmp_imm);
rbc_ihdlrX(cmp_imm_memb);
rbc_ihdlrX(cmp_imm_memw);
rbc_ihdlrX(cmp_imm_memd);
rbc_ihdlrX(cmp_imm_memq);

rbc_ihdlrX(sin);
rbc_ihdlrX(sout);

rbc_ihdlrX(loadb);
rbc_ihdlrX(storeb);
rbc_ihdlrX(loadw);
rbc_ihdlrX(storew);
rbc_ihdlrX(loadd);
rbc_ihdlrX(stored);
rbc_ihdlrX(loadq);
rbc_ihdlrX(storeq);

rbc_ihdlrX(loadb_reg);
rbc_ihdlrX(storeb_reg);
rbc_ihdlrX(loadw_reg);
rbc_ihdlrX(storew_reg);
rbc_ihdlrX(loadd_reg);
rbc_ihdlrX(stored_reg);
rbc_ihdlrX(loadq_reg);
rbc_ihdlrX(storeq_reg);

rbc_ihdlrX(atm_loadb);
rbc_ihdlrX(atm_loadw);
rbc_ihdlrX(atm_loadd);
rbc_ihdlrX(atm_loadq);

rbc_ihdlrX(atm_storeb);
rbc_ihdlrX(atm_storew);
rbc_ihdlrX(atm_stored);
rbc_ihdlrX(atm_storeq);

rbc_ihdlrX(atm_loadb_reg);
rbc_ihdlrX(atm_loadw_reg);
rbc_ihdlrX(atm_loadd_reg);
rbc_ihdlrX(atm_loadq_reg);

rbc_ihdlrX(atm_storeb_reg);
rbc_ihdlrX(atm_storew_reg);
rbc_ihdlrX(atm_stored_reg);
rbc_ihdlrX(atm_storeq_reg);

rbc_ihdlrX(cmpxchg);
rbc_ihdlrX(cmpxchg_reg);

#endif
