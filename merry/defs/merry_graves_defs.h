#ifndef _MERRY_GRAVES_DEFS_
#define _MERRY_GRAVES_DEFS_

#include <merry_core_types.h>
#include <merry_types.h>

#define REQ(name)                                                              \
  void req_##name(MerryGraves *GRAVES, MerryGravesCoreRepr *repr,              \
                  MerryGravesGroup *grp, MerryGravesRequest *req)

struct MerryCoreBase;
union MerryRequestArgs;
struct MerryICRes;

// Since each core can have its own unique set of fields that
// might require different ways of initialization, this will create
// something that is the default of the core and then later using
// some other tricks, the programs may enable/disable certain features
_MERRY_DEFINE_FUNC_PTR_(mptr_t, mcorecreate_t, struct MerryCoreBase *,
                        maddress_t, struct MerryICRes *);
_MERRY_DEFINE_FUNC_PTR_(void, mcoredeletecore_t, mptr_t);
_MERRY_DEFINE_FUNC_PTR_(msize_t, mcoreexec_t, mptr_t);
_MERRY_DEFINE_FUNC_PTR_(struct MerryCoreBase *, mcorecreatebase_t,
                        struct MerryICRes *);
_MERRY_DEFINE_FUNC_PTR_(void, mcoredeletebase_t, struct MerryCoreBase *);
_MERRY_DEFINE_FUNC_PTR_(void, mcorepredel_t, mptr_t);
_MERRY_DEFINE_FUNC_PTR_(mret_t, mcoresetinp_t, mptr_t, mstr_t,
                        struct MerryICRes *);
_MERRY_DEFINE_FUNC_PTR_(mret_t, mcoreprepcore_t, mptr_t, struct MerryICRes *);

#endif
