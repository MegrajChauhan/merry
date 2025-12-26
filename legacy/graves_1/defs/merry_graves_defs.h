#ifndef _MERRY_GRAVES_DEFS_
#define _MERRY_GRAVES_DEFS_

#include <merry_core_types.h>
#include <merry_operations.h>
#include <merry_types.h>

#define REQ(name)                                                              \
  void req_##name(MerryGraves *GRAVES,\
                  MerryGravesGroup *grp, MerryGravesRequest *req)

struct MerryCoreBase;
union MerryRequestArgs;
struct MerryICRes;

// Since each core can have its own unique set of fields that
// might require different ways of initialization, this will create
// something that is the default of the core and then later using
// some other tricks, the programs may enable/disable certain features
_MERRY_DEFINE_FUNC_PTR_(mresult_t, mcorecreate_t, struct MerryCoreBase *,
                        maddress_t, mptr_t, mptr_t *);
_MERRY_DEFINE_FUNC_PTR_(void, mcoredeletecore_t, mptr_t);
_MERRY_DEFINE_FUNC_PTR_(msize_t, mcoreexec_t, mptr_t);
_MERRY_DEFINE_FUNC_PTR_(mresult_t, mcorecreatebase_t,
                        struct MerryCoreBase **);
_MERRY_DEFINE_FUNC_PTR_(void, mcoredeletebase_t, struct MerryCoreBase *);
_MERRY_DEFINE_FUNC_PTR_(void, mcorepredel_t, mptr_t);
_MERRY_DEFINE_FUNC_PTR_(mresult_t, mcoresetinp_t, mptr_t, mstr_t);
_MERRY_DEFINE_FUNC_PTR_(mresult_t, mcoreprepcore_t, mptr_t);
_MERRY_DEFINE_FUNC_PTR_(mresult_t, mcoreconfig_t, mstr_t, mcstr_t, mbool_t *);


#endif
