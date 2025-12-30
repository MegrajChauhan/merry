#ifndef _MERRY_CORE_INTERFACE_
#define _MERRY_CORE_INTERFACE_

#include <merry_core_metadata.h>
#include <merry_core_state.h>
#include <merry_core_types.h>
#include <merry_requests.h>
#include <merry_results.h>
#include <merry_types.h>

#define merry_get_id(iden) ((iden)->id)
#define merry_get_uid(iden) ((iden)->uid)
#define merry_get_guid(iden) ((iden)->gid)

_MERRY_DEFINE_FUNC_PTR_(mresult_t, mcorecreate_t, MerryCoreState *,
                        MerryCoreIdentity, maddress_t, mptr_t *);
_MERRY_DEFINE_FUNC_PTR_(void, mcoredeletecore_t, mptr_t);
_MERRY_DEFINE_FUNC_PTR_(msize_t, mcoreexec_t, mptr_t);
_MERRY_DEFINE_FUNC_PTR_(void, mcorepredel_t, mptr_t);
_MERRY_DEFINE_FUNC_PTR_(mresult_t, mcoresetinp_t, mptr_t, mstr_t);
_MERRY_DEFINE_FUNC_PTR_(mresult_t, mcoreprepcore_t, mptr_t);
_MERRY_DEFINE_FUNC_PTR_(mresult_t, mcoreconfig_t, mstr_t, mcstr_t, mbool_t *);
_MERRY_DEFINE_FUNC_PTR_(void, mcorehelp_t, void);

typedef struct MerryCoreInterface MerryCoreInterface;

struct MerryCoreInterface {
  mcorecreate_t ccreate;
  mcoredeletecore_t cdel;
  mcoreexec_t cexec;
  mcorepredel_t cpredel;
  mcoresetinp_t csetinp;
  mcoreprepcore_t cprepc;
  mcoreconfig_t cconf;
  mcorehelp_t chelp;
  mbool_t set;
};

void merry_register_core(mcore_t type, mcorecreate_t ccreate,
                         mcoredeletecore_t cdel, mcoreexec_t cexec,
                         mcorepredel_t cpredel, mcoresetinp_t csetinp,
                         mcoreprepcore_t cprepc, mcoreconfig_t cconf,
                         mcorehelp_t chelp);

mresult_t merry_init_request(MerryGravesRequest **req, mcond_t *cond,
                             mbool_t async);

mresult_t merry_set_request_CREATE_CORE(MerryGravesRequest *req,
                                        mcore_t new_core_type,
                                        maddress_t st_addr, mguid_t gid);
mresult_t merry_set_request_CREATE_GROUP(MerryGravesRequest *req);

mresult_t merry_get_request_result_CREATE_CORE(MerryGravesRequest *req,
                                               msize_t *id, msize_t *uid);
mresult_t merry_get_request_result_CREATE_GROUP(MerryGravesRequest *req,
                                                msize_t *gid);

#endif
