#ifndef _MERRY_EXPR_PARSER_
#define _MERRY_EXPR_PARSER_

#include <merry_lexer.h>
#include <merry_stack.h>
#include <stdlib.h>

typedef struct ExprParser ExprParser;
typedef struct __TmpRepr __TmpRepr;

_MERRY_DECLARE_STACK_(Oper, token_t); // operator stack
_MERRY_DECLARE_STACK_(Op, __TmpRepr);

struct ExprParser {
  MerryOperStack *oper_stack;
  MerryOpStack *op_stack;
  token_t type; // type of result
  union {
    mqword_t integer;
    double decimal;
  };
};

struct __TmpRepr {
  token_t type;
  msize_t lnum;
  union {
    mqword_t integer;
    double decimal;
  };
};

ExprParser *create_expr_parser();

mbool_t parse_expr(ExprParser *expr, Lexer *l);

void destroy_expr_parser(ExprParser *expr);

#endif
