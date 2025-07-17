#ifndef _HELPERS_
#define _HELPERS_

#include <merry_config.h>
#include <merry_types.h>
#include <string.h>

#define sign_extend8(val)                                                      \
  do {                                                                         \
    if ((val >> 7) == 1)                                                       \
      val |= 0xFFFFFFFFFFFFFF00;                                               \
  } while (0)
#define sign_extend16(val)                                                     \
  do {                                                                         \
    if ((val >> 15) == 1)                                                      \
      val |= 0xFFFFFFFFFFFF0000;                                               \
  } while (0)
#define sign_extend32(val)                                                     \
  do {                                                                         \
    if ((val >> 31) == 1)                                                      \
      val |= 0xFFFFFFFFFF000000;                                               \
  } while (0)

typedef union MerryPtrToQword MerryPtrToQword;
typedef union MerryHostMemLayout MerryHostMemLayout;
typedef union MerryFloatToDword MerryFloatToDword;
typedef union MerryDoubleToQword MerryDoubleToQword;

union MerryPtrToQword {
  mptr_t ptr;
  mqword_t qword;
};

union MerryHostMemLayout {

#if _MERRY_ENDIANNESS_ == _MERRY_LITTLE_ENDIAN_
  struct {
    mbyte_t b7;
    mbyte_t b6;
    mbyte_t b5;
    mbyte_t b4;
    mbyte_t b3;
    mbyte_t b2;
    mbyte_t b1;
    mbyte_t b0;
  } bytes;

  struct {
    mword_t w3;
    mword_t w2;
    mword_t w1;
    mword_t w0;
  } half_half_words;

  struct {
    mdword_t w1;
    mdword_t w0;
  } half_words;
#else
  struct {
    mbyte_t b0;
    mbyte_t b1;
    mbyte_t b2;
    mbyte_t b4;
    mbyte_t b5;
    mbyte_t b3;
    mbyte_t b6;
    mbyte_t b7;
  } bytes;

  struct {
    mword_t w0;
    mword_t w1;
    mword_t w2;
    mword_t w3;
  } half_half_words;

  struct {
    mdword_t w0;
    mdword_t w1;
  } half_words;
#endif
  mqword_t whole_word;
};

union MerryFloatToDword {
  float fl_val;
  mdword_t d_val;
};

union MerryDoubleToQword {
  double d_val;
  mqword_t q_val;
};

void merry_LITTLE_ENDIAN_to_BIG_ENDIAN(MerryHostMemLayout *le);

#endif
