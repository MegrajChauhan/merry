#ifndef _MERRY_ERROR_STACK_
#define _MERRY_ERROR_STACK_

#include <merry_default_consts.h>
#include <merry_types.h>
#include <merry_utils.h>

#define merry_error_stack_fatality(st) ((st)->fatality = mtrue)
#define merry_error_stack_init(st, oid, ouid, ogid)                            \
  do {                                                                         \
    (st)->owner_id = (oid);                                                    \
    (st)->owner_uid = (ouid);                                                  \
    (st)->owner_gid = (ogid);                                                  \
    (st)->fatality = mfalse;                                                   \
    (st)->SP = 0;                                                              \
  } while (0)

typedef struct MerryErrorStack MerryErrorStack;
typedef struct MerryErrorStackEntry MerryErrorStackEntry;

struct MerryErrorStackEntry {
  mstr_t cause, message, context;
};

struct MerryErrorStack {
  MerryErrorStackEntry entries[MERRY_ERROR_STACK_DEPTH];
  msize_t SP; // Stack Pointer
  mid_t owner_id;
  muid_t owner_uid;
  mguid_t owner_gid;
  mbool_t fatality; // Was a fatal error encountered?
};

void PUSH(MerryErrorStack *st, mstr_t cause, mstr_t msg, mstr_t cont);

void ERROR(MerryErrorStack *st);

#endif
