#ifndef _RBC_INSTRUCTION_HANDLER_
#define _RBC_INSTRUCTION_HANDLER_

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

#endif
