#ifndef _MERRY_GRAVES_DEFS_
#define _MERRY_GRAVES_DEFS_

#include <merry_core_types.h>
#include <merry_error_stack.h>
#include <merry_types.h>

#define _MERRY_MINIMUM_NORMAL_INPUT_FILE_SIZE_ 64
#define _MERRY_NORMAL_INPUT_FILE_ITIT_ENTRY_LEN_ 16
#define _MERRY_INPUT_FILE_HEADER_SIZE_ 40
#define _MERRY_DATA_METADATA_LENGTH_ 24

#define REQ(name)                                                              \
  void req_##name(MerryGravesCoreRepr *repr, MerryGravesGroup *grp)

struct MerryCoreBase;
union MerryRequestArgs;

// Since each core can have its own unique set of fields that
// might require different ways of initialization, this will create
// something that is the default of the core and then later using
// some other tricks, the programs may enable/disable certain features
_MERRY_DEFINE_FUNC_PTR_(mptr_t, mcorecreate_t, struct MerryCoreBase *,
                        maddress_t, MerryErrorStack *);
_MERRY_DEFINE_FUNC_PTR_(void, mcoredeletecore_t, mptr_t);
_MERRY_DEFINE_FUNC_PTR_(_THRET_T_, mcoreexec_t, mptr_t);
_MERRY_DEFINE_FUNC_PTR_(struct MerryCoreBase *, mcorecreatebase_t,
                        MerryErrorStack *);
_MERRY_DEFINE_FUNC_PTR_(void, mcoredeletebase_t, struct MerryCoreBase *);
_MERRY_DEFINE_FUNC_PTR_(union MerryRequestArgs *, mgetreqargs_t, mptr_t);
_MERRY_DEFINE_FUNC_PTR_(void, mcorepredel_t, mptr_t);

// Input file type
typedef enum MerryFileType MerryFileType;
typedef enum mmempg_t mmempg_t;

enum MerryFileType {
  _NORMAL_INPUT_FILE,
};

enum mmempg_t {
  MEMPG_VM_OWNED, // Rogue pages with no parents(The properties remain intact)
  MEMPG_PRIVATE,
  MEMPG_PUBLIC,
  MEMPG_SHARED, // This implies that the particular page was private
                // for some core but they decided to share it with
                // some other core which makes it private.
                // If the owner dies, the page is made MEMPG_VM_OWNED
};

#endif
