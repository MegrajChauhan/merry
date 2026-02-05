#ifndef _MERRY_CORE_INSTRUCTION_HANDLER_
#define _MERRY_CORE_INSTRUCTION_HANDLER_

#include <float.h>
#include <math.h>
#include <merry_core_defs.h>
#include <merry_core_internals.h>
#include <merry_core_registers.h>
#include <merry_core_sysint.h>
#include <merry_flags_regr.h>
#include <merry_graves_request_queue.h>

struct MerryCore;

#define merry_core_ihdlr(name)                                                 \
  mbool_t merry_core_i##name(struct MerryCore *core)

void merry_core_compare_f32(float a, float b, MerryCoreFFlagsRegr *regr);
void merry_core_compare_f64(double a, double b, MerryCoreFFlagsRegr *regr);

merry_core_ihdlr(sysint);
merry_core_ihdlr(mint);
merry_core_ihdlr(add_imm);
merry_core_ihdlr(add_reg);
merry_core_ihdlr(sub_imm);
merry_core_ihdlr(sub_reg);
merry_core_ihdlr(mul_imm);
merry_core_ihdlr(mul_reg);
merry_core_ihdlr(div_imm);
merry_core_ihdlr(div_reg);
merry_core_ihdlr(mod_imm);
merry_core_ihdlr(mod_reg);
merry_core_ihdlr(fadd);
merry_core_ihdlr(fsub);
merry_core_ihdlr(fmul);
merry_core_ihdlr(fdiv);
merry_core_ihdlr(fadd32);
merry_core_ihdlr(fsub32);
merry_core_ihdlr(fmul32);
merry_core_ihdlr(fdiv32);
merry_core_ihdlr(inc);
merry_core_ihdlr(dec);
merry_core_ihdlr(mov_imm);
merry_core_ihdlr(mov_imm_32);
merry_core_ihdlr(mov_reg);
merry_core_ihdlr(mov_reg8);
merry_core_ihdlr(mov_reg16);
merry_core_ihdlr(mov_reg32);
merry_core_ihdlr(movesx_imm8);
merry_core_ihdlr(movesx_imm16);
merry_core_ihdlr(movesx_imm32);
merry_core_ihdlr(movesx_reg8);
merry_core_ihdlr(movesx_reg16);
merry_core_ihdlr(movesx_reg32);
merry_core_ihdlr(excg8);
merry_core_ihdlr(excg16);
merry_core_ihdlr(excg32);
merry_core_ihdlr(excg);
merry_core_ihdlr(jmp_off);
merry_core_ihdlr(jmp_addr);
merry_core_ihdlr(jmp_reg);
merry_core_ihdlr(jnz);
merry_core_ihdlr(jz);
merry_core_ihdlr(jne);
merry_core_ihdlr(je);
merry_core_ihdlr(jnc);
merry_core_ihdlr(jc);
merry_core_ihdlr(jno);
merry_core_ihdlr(jo);
merry_core_ihdlr(jnn);
merry_core_ihdlr(jn);
merry_core_ihdlr(jng);
merry_core_ihdlr(jg);
merry_core_ihdlr(jns);
merry_core_ihdlr(js);
merry_core_ihdlr(jge);
merry_core_ihdlr(jse);
merry_core_ihdlr(jfz);
merry_core_ihdlr(jfnz);
merry_core_ihdlr(jfn);
merry_core_ihdlr(jfnn);
merry_core_ihdlr(jfuf);
merry_core_ihdlr(jfnuf);
merry_core_ihdlr(jfo);
merry_core_ihdlr(jfno);
merry_core_ihdlr(jfu);
merry_core_ihdlr(jfnu);
merry_core_ihdlr(jfi);
merry_core_ihdlr(jfni);
merry_core_ihdlr(call_reg);
merry_core_ihdlr(call);
merry_core_ihdlr(ret);
merry_core_ihdlr(retnz);
merry_core_ihdlr(retz);
merry_core_ihdlr(retne);
merry_core_ihdlr(rete);
merry_core_ihdlr(retnc);
merry_core_ihdlr(retc);
merry_core_ihdlr(retno);
merry_core_ihdlr(reto);
merry_core_ihdlr(retnn);
merry_core_ihdlr(retn);
merry_core_ihdlr(retng);
merry_core_ihdlr(retg);
merry_core_ihdlr(retns);
merry_core_ihdlr(rets);
merry_core_ihdlr(retge);
merry_core_ihdlr(retse);
merry_core_ihdlr(retfz);
merry_core_ihdlr(retfnz);
merry_core_ihdlr(retfn);
merry_core_ihdlr(retfnn);
merry_core_ihdlr(retfuf);
merry_core_ihdlr(retfnuf);
merry_core_ihdlr(retfo);
merry_core_ihdlr(retfno);
merry_core_ihdlr(retfu);
merry_core_ihdlr(retfnu);
merry_core_ihdlr(retfi);
merry_core_ihdlr(retfni);
merry_core_ihdlr(loop);
merry_core_ihdlr(push_imm64);
merry_core_ihdlr(push_reg);
merry_core_ihdlr(pop64);
merry_core_ihdlr(pusha);
merry_core_ihdlr(popa);
merry_core_ihdlr(loadsq);
merry_core_ihdlr(storesq);
merry_core_ihdlr(loadsq_reg);
merry_core_ihdlr(storesq_reg);
merry_core_ihdlr(and_imm);
merry_core_ihdlr(and_reg);
merry_core_ihdlr(or_imm);
merry_core_ihdlr(or_reg);
merry_core_ihdlr(xor_imm);
merry_core_ihdlr(xor_reg);
merry_core_ihdlr(not);
merry_core_ihdlr(lshift);
merry_core_ihdlr(rshift);
merry_core_ihdlr(lshift_reg);
merry_core_ihdlr(rshift_reg);
merry_core_ihdlr(cmp_imm);
merry_core_ihdlr(cmp_reg);
merry_core_ihdlr(fcmp);
merry_core_ihdlr(fcmp32);
merry_core_ihdlr(bin);
merry_core_ihdlr(bout);
merry_core_ihdlr(loadb);
merry_core_ihdlr(loadw);
merry_core_ihdlr(loadd);
merry_core_ihdlr(loadq);
merry_core_ihdlr(storeb);
merry_core_ihdlr(storew);
merry_core_ihdlr(stored);
merry_core_ihdlr(storeq);
merry_core_ihdlr(loadb_reg);
merry_core_ihdlr(loadw_reg);
merry_core_ihdlr(loadd_reg);
merry_core_ihdlr(loadq_reg);
merry_core_ihdlr(storeb_reg);
merry_core_ihdlr(storew_reg);
merry_core_ihdlr(stored_reg);
merry_core_ihdlr(storeq_reg);
merry_core_ihdlr(atm_loadb);
merry_core_ihdlr(atm_loadw);
merry_core_ihdlr(atm_loadd);
merry_core_ihdlr(atm_loadq);
merry_core_ihdlr(atm_storeb);
merry_core_ihdlr(atm_storew);
merry_core_ihdlr(atm_stored);
merry_core_ihdlr(atm_storeq);
merry_core_ihdlr(atm_loadb_reg);
merry_core_ihdlr(atm_loadw_reg);
merry_core_ihdlr(atm_loadd_reg);
merry_core_ihdlr(atm_loadq_reg);
merry_core_ihdlr(atm_storeb_reg);
merry_core_ihdlr(atm_storew_reg);
merry_core_ihdlr(atm_stored_reg);
merry_core_ihdlr(atm_storeq_reg);
merry_core_ihdlr(lea);
merry_core_ihdlr(cflags);
merry_core_ihdlr(cfflags);
merry_core_ihdlr(reset);
merry_core_ihdlr(cmpxchg);
merry_core_ihdlr(cmpxchg_reg);

merry_core_ihdlr(invalid_inst);

#endif
