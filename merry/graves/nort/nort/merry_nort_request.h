#ifndef _MERRY_NORT_REQUEST_
#define _MERRY_NORT_REQUEST_

#include <merry_error_stack.h>
#include <merry_protectors.h>
#include <stdlib.h>

typedef struct MerryNortRequest MerryNortRequest;
typedef enum mnreqstate_t mnreqstate_t;

enum mnreqstate_t {
  REQ_NOT_SUBMITTED,
  REQ_PENDING,
  REQ_COMPLETED,
};

struct MerryNortRequest {
  msize_t result;
  mnreqstate_t state;
  mbool_t blocking;
  mcond_t *owner_cond;
};

MerryNortRequest *merry_new_nort_request(mbool_t blocking, mcond_t *ocond,
                                         MerryErrorStack *st);

void merry_destory_nort_request(MerryNortRequest *req);

#endif
