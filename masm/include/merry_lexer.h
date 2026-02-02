#ifndef _MERRY_LEXER_
#define _MERRY_LEXER_

#include <merry_token.h>
#include <merry_utils.h>
#include <merry_types.h>
#include <merry_logger.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct Lexer Lexer;

struct Lexer {
	mstr_t stream;
	msize_t len;
	msize_t curr;
	msize_t lnum;
	msize_t col;
};

Lexer* lexer_init(mstr_t stream, msize_t len);

Token lexer_next(Lexer *l);

Token lexer_peek(Lexer *l);

void lexer_destroy(Lexer *l);

#endif
