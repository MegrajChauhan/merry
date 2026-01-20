#ifndef _MERRY_CORE_INSTRUCTION_HANDLER_
#define _MERRY_CORE_INSTRUCTION_HANDLER_

#include <float.h>
#include <math.h>
#include <merry_flags_regr.h>
#include <merry_graves_request_queue.h>
#include <merry_core.h>

#define merry_core_ihdlr(name)                                                        \
  mbool_t merry_core_i##name(MerryCore *core)

void merry_core_compare_f32(float a, float b, MerryCoreFFlagsRegr *regr);
void merry_core_compare_f64(double a, double b, MerryCoreFFlagsRegr *regr);

merry_core_ihdlr(sysint);
merry_core_ihdlr(mint);
merry_core_ihdlrX(add);
merry_core_ihdlrX(sub);
merry_core_ihdlrX(mul);
merry_core_ihdlrX(div);

merry_core_ihdlrX(add_imm);
merry_core_ihdlrX(add_reg);
merry_core_ihdlrX(sub_imm);
merry_core_ihdlrX(sub_reg);
merry_core_ihdlrX(mul_imm);
merry_core_ihdlrX(mul_reg);
merry_core_ihdlrX(div_imm);
merry_core_ihdlrX(div_reg);
merry_core_ihdlrX(mod_imm);
merry_core_ihdlrX(mod_reg);

merry_core_ihdlrX(fadd);
merry_core_ihdlrX(fsub);
merry_core_ihdlrX(fmul);
merry_core_ihdlrX(fdiv);

merry_core_ihdlrX(fadd32);
merry_core_ihdlrX(fsub32);
merry_core_ihdlrX(fmul32);
merry_core_ihdlrX(fdiv32);

merry_core_ihdlrX(add_memb);
merry_core_ihdlrX(add_memw);
merry_core_ihdlrX(add_memd);
merry_core_ihdlrX(add_memq);

merry_core_ihdlrX(sub_memb);
merry_core_ihdlrX(sub_memw);
merry_core_ihdlrX(sub_memd);
merry_core_ihdlrX(sub_memq);

merry_core_ihdlrX(mul_memb);
merry_core_ihdlrX(mul_memw);
merry_core_ihdlrX(mul_memd);
merry_core_ihdlrX(mul_memq);

merry_core_ihdlrX(div_memb);
merry_core_ihdlrX(div_memw);
merry_core_ihdlrX(div_memd);
merry_core_ihdlrX(div_memq);

merry_core_ihdlrX(mod_memb);
merry_core_ihdlrX(mod_memw);
merry_core_ihdlrX(mod_memd);
merry_core_ihdlrX(mod_memq);

merry_core_ihdlrX(fadd64_mem);
merry_core_ihdlrX(fsub64_mem);
merry_core_ihdlrX(fmul64_mem);
merry_core_ihdlrX(fdiv64_mem);

merry_core_ihdlrX(fadd32_mem);
merry_core_ihdlrX(fsub32_mem);
merry_core_ihdlrX(fmul32_mem);
merry_core_ihdlrX(fdiv32_mem);

merry_core_ihdlrX(mov_imm);
merry_core_ihdlrX(movf32);

// mov_reg, mov_reg8, mov_reg16, mov_reg32 don't need a dedicated function

merry_core_ihdlrX(movesx_imm8);
merry_core_ihdlrX(movesx_imm16);
merry_core_ihdlrX(movesx_imm32);
merry_core_ihdlrX(movesx_reg8);
merry_core_ihdlrX(movesx_reg16);
merry_core_ihdlrX(movesx_reg32);

merry_core_ihdlrX(excg);
merry_core_ihdlrX(excg8);
merry_core_ihdlrX(excg16);
merry_core_ihdlrX(excg32);

// mov8, mov16, mov32 also won't need dedicated functions
// conditional moves also won't require dedicated functions

merry_core_ihdlrX(call);
merry_core_ihdlrX(ret);
merry_core_ihdlrX(call_reg);

merry_core_ihdlrX(push_immq);
merry_core_ihdlrX(push_reg);
merry_core_ihdlrX(popq);
merry_core_ihdlrX(pusha);
merry_core_ihdlrX(popa);

merry_core_ihdlrX(push_memb);
merry_core_ihdlrX(push_memw);
merry_core_ihdlrX(push_memd);
merry_core_ihdlrX(push_memq);
merry_core_ihdlrX(pop_memb);
merry_core_ihdlrX(pop_memw);
merry_core_ihdlrX(pop_memd);
merry_core_ihdlrX(pop_memq);

merry_core_ihdlrX(loadsq);

merry_core_ihdlrX(storesq);

merry_core_ihdlrX(and_imm);
merry_core_ihdlrX(or_imm);
merry_core_ihdlrX(xor_imm);

merry_core_ihdlrX(cmp_imm);
merry_core_ihdlrX(cmp_imm_memb);
merry_core_ihdlrX(cmp_imm_memw);
merry_core_ihdlrX(cmp_imm_memd);
merry_core_ihdlrX(cmp_imm_memq);

merry_core_ihdlrX(sin);
merry_core_ihdlrX(sout);

merry_core_ihdlrX(loadb);
merry_core_ihdlrX(storeb);
merry_core_ihdlrX(loadw);
merry_core_ihdlrX(storew);
merry_core_ihdlrX(loadd);
merry_core_ihdlrX(stored);
merry_core_ihdlrX(loadq);
merry_core_ihdlrX(storeq);

merry_core_ihdlrX(loadb_reg);
merry_core_ihdlrX(storeb_reg);
merry_core_ihdlrX(loadw_reg);
merry_core_ihdlrX(storew_reg);
merry_core_ihdlrX(loadd_reg);
merry_core_ihdlrX(stored_reg);
merry_core_ihdlrX(loadq_reg);
merry_core_ihdlrX(storeq_reg);

merry_core_ihdlrX(atm_loadb);
merry_core_ihdlrX(atm_loadw);
merry_core_ihdlrX(atm_loadd);
merry_core_ihdlrX(atm_loadq);

merry_core_ihdlrX(atm_storeb);
merry_core_ihdlrX(atm_storew);
merry_core_ihdlrX(atm_stored);
merry_core_ihdlrX(atm_storeq);

merry_core_ihdlrX(atm_loadb_reg);
merry_core_ihdlrX(atm_loadw_reg);
merry_core_ihdlrX(atm_loadd_reg);
merry_core_ihdlrX(atm_loadq_reg);

merry_core_ihdlrX(atm_storeb_reg);
merry_core_ihdlrX(atm_storew_reg);
merry_core_ihdlrX(atm_stored_reg);
merry_core_ihdlrX(atm_storeq_reg);

merry_core_ihdlrX(cmpxchg);
merry_core_ihdlrX(cmpxchg_reg);

#endif
