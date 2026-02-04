#ifndef _MERRY_TOKEN_TYPES_
#define _MERRY_TOKEN_TYPES_

typedef enum token_t token_t;

enum token_t {
    TOK_EOF,
    TOK_ERR,
	TOK_NUM_INT,
	TOK_NUM_FLOAT,
	TOK_NUM_BINARY,
	TOK_NUM_OCTAL,
	TOK_NUM_HEX,
	TOK_FLOAT,
	TOK_PLUS,
	TOK_MINUS,
	TOK_MUL,
	TOK_DIV,
};

#endif
