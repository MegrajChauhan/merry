#ifndef _MERRY_TWC_
#define _MERRY_TWC_

// Two-way channel
#include <merry_config.h>
#include <merry_owc.h>
#include <merry_platform.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdlib.h>

typedef struct MerryTWC MerryTWC;

struct MerryTWC {
  MerryOWC *speaking_channel;
  MerryOWC *listening_channel;
};

// Form a TWC channel between two parties
mret_t merry_twc_init(MerryTWC *party_1, MerryTWC *party_2,
                      MerryErrorStack *st);

void merry_twc_config(MerryTWC *twc);

mret_t merry_twc_send(MerryTWC *twc, mbptr_t data, msize_t len,
                      MerryErrorStack *err_st);
mret_t merry_twc_receive(MerryTWC *twc, mbptr_t buf, msize_t n,
                         MerryErrorStack *err_st);

void merry_twc_close_channel(MerryTWC *twc);

#endif
