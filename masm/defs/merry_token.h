#ifndef _MERRY_TOKEN_
#define _MERRY_TOKEN_

#include <merry_token_types.h>
#include <merry_types.h>

typedef struct Token Token;

struct Token {
  token_t type;
  mstr_t begin, end; // the starting of the token and end of the token
  msize_t lnum;
  msize_t col_st;
};

#endif
