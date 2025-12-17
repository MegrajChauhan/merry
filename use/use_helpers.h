#ifndef _USE_HELPERS_
#define _USE_HELPERS_

#include <use_types.h>
#include <use_config.h>
#include <use_defs.h>

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

#define bit_group(name, len) unsigned name : len

typedef union PtrToQword PtrToQword;
typedef union HostMemLayout HostMemLayout;
typedef union FloatToDword FloatToDword;
typedef union DoubleToQword DoubleToQword;

union PtrToQword {
  ptr_t ptr;
  qword_t qword;
};

union HostMemLayout {

#if _MENDIANNESS_ == _MLITTLE_ENDIAN_
  struct {
    byte_t b7;
    byte_t b6;
    byte_t b5;
    byte_t b4;
    byte_t b3;
    byte_t b2;
    byte_t b1;
    byte_t b0;
  } bytes;

  struct {
    word_t w3;
    word_t w2;
    word_t w1;
    word_t w0;
  } half_half_words;

  struct {
    dword_t w1;
    dword_t w0;
  } half_words;
#else
  struct {
    byte_t b0;
    byte_t b1;
    byte_t b2;
    byte_t b4;
    byte_t b5;
    byte_t b3;
    byte_t b6;
    byte_t b7;
  } bytes;

  struct {
    word_t w0;
    word_t w1;
    word_t w2;
    word_t w3;
  } half_half_words;

  struct {
    dword_t w0;
    dword_t w1;
  } half_words;
#endif
  qword_t whole_word;
};

union FloatToDword {
  float fl_val;
  dword_t d_val;
};

union DoubleToQword {
  double d_val;
  qword_t q_val;
};

extern result_t open_pipe(dataline_t *rline, dataline_t *wline) _ALIAS_(merry_open_pipe);

extern bool_t is_path_a_directory(str_t path) _ALIAS_(merry_is_path_a_directory);

#endif
