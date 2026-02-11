#include <merry_expr_parser.h>

#define ASSOCIATIVITY_LEFT 0
#define ASSOCIATIVITY_RIGHT 1

_MERRY_DEFINE_STACK_(Op, __TmpRepr);
_MERRY_DEFINE_STACK_(Oper, token_t);

_MERRY_INTERNAL_ int associativity(token_t type) {
  switch (type) {
  case TOK_PLUS:
  case TOK_MINUS:
  case TOK_MUL:
    return ASSOCIATIVITY_LEFT;
  }
  return ASSOCIATIVITY_RIGHT; // this will never be executed[I am 100% sure!!!! :) ]
}

_MERRY_INTERNAL_ int precedence(token_t type) {
  switch (type) {
  case TOK_AND:
  case TOK_OR:
    return 0;
  case TOK_GREATER:
  case TOK_SMALLER:
  case TOK_SMALLER_EQUAL:
  case TOK_GREATER_EQUAL:
  case TOK_IS_EQUAL:
  case TOK_NOT_EQUAL:
    return 1;
  case TOK_MINUS:
  case TOK_PLUS:
    return 5;
  case TOK_MUL:
    return 6;
  case TOK_DIV:
    return 7;
  case TOK_MODULO:
    return 8;
  case TOK_BITWISE_AND:
  case TOK_BITWISE_OR:
  case TOK_BITWISE_XOR:
    return 9;
  case TOK_BITWISE_NOT:
    return 10;
  }
  return 0;
}

_MERRY_INTERNAL_ mbool_t evaluate_operator(ExprParser *expr) {
  token_t top;
  token_t resulting_type = TOK_NUM_INT;
  __TmpRepr right, left;
  
  if (merry_Oper_stack_pop(expr->oper_stack, &top) != MRES_SUCCESS) {
    MERR("Expression parsing failed!", NULL);
    return mfalse;
  }
  
  if (merry_Op_stack_pop(expr->op_stack, &right) != MRES_SUCCESS) {
    MERR("Expression parsing failed!", NULL);
    return mfalse;
  }

  if (top != TOK_BITWISE_NOT) {
  if (merry_Op_stack_pop(expr->op_stack, &left) != MRES_SUCCESS) {
    MERR("Expression parsing failed!", NULL);
    return mfalse;
  }
  }
  if (top == TOK_OPEN_PAREN) {
  	MERR("Invalid Expression!", NULL);
  	return mfalse;
  }
  mqword_t res_int;
  double res_dec;
  switch (top) {
  case TOK_PLUS: {
    res_int = right.integer + left.integer;
    res_dec = right.decimal + left.decimal;
    break;
  }
  case TOK_MINUS: {
    res_int = left.integer - right.integer;
    res_dec = left.decimal - right.decimal;
    break;
  }
  case TOK_MUL: {
    res_int = right.integer * left.integer;
    res_dec = right.decimal * left.decimal;
    break;
  }
  case TOK_DIV: {
    if (left.integer == 0 || left.decimal == 0.0) {
      MERR("[LINE: %zu]: Divide by ZERO!", right.lnum);
      return mfalse;
    }
    res_int = left.integer / right.integer;
    res_dec = left.decimal / right.decimal;
    break;
  }
  case TOK_MODULO: {
    if (right.lnum == TOK_NUM_FLOAT) {
    	MERR("[LINE: %zu]: Modulo operator not supported for floats", right.lnum);
    }
    if (left.integer == 0) {
      MERR("[LINE: %zu]: Divide by ZERO!", right.lnum);
      return mfalse;
    }
    res_int = left.integer % right.integer;
    break;
  }
  case TOK_GREATER: {
  	res_int = left.integer > right.integer?1:0;
  	res_dec = left.decimal > right.decimal?1.0:0.0;
  	break;
  }
  case TOK_SMALLER: {
   	res_int = left.integer < right.integer?1:0;
   	res_dec = left.decimal < right.decimal?1.0:0.0;
   	break;
  }
  case TOK_SMALLER_EQUAL: {
   	res_int = left.integer <= right.integer?1:0;
   	res_dec = left.decimal <= right.decimal?1.0:0.0;
   	break;
  }
  case TOK_GREATER_EQUAL: {
   	res_int = left.integer >= right.integer?1:0;
   	res_dec = left.decimal >= right.decimal?1.0:0.0;
   	break;
  }
  case TOK_IS_EQUAL: {
  	res_int = left.integer == right.integer?1:0;
  	res_dec = left.decimal == right.decimal?1.0:0.0;
  	break;
  }
  case TOK_NOT_EQUAL: {
  	res_int = left.integer != right.integer?1:0;
  	res_dec = left.decimal != right.decimal?1.0:0.0;
  	break;
  }
  case TOK_AND: {
  	res_int = left.integer && right.integer?1:0;
  	res_dec = left.decimal && right.decimal?1.0:0.0;
  	break;  	
  }
  case TOK_OR: {
  	res_int = left.integer || right.integer?1:0;
  	res_dec = left.decimal || right.decimal?1.0:0.0;
  	break;
  }
  case TOK_BITWISE_AND: {
    if (right.lnum == TOK_NUM_FLOAT || left.lnum == TOK_NUM_FLOAT) {
    	MERR("[LINE: %zu]: BITWISE AND operator not supported for floats", right.lnum);
    }
  	res_int = left.integer & right.integer;
  	break;
  }
  case TOK_BITWISE_OR: {
    if (right.lnum == TOK_NUM_FLOAT || left.lnum == TOK_NUM_FLOAT) {
    	MERR("[LINE: %zu]: BITWISE OR operator not supported for floats", right.lnum);
    }
  	res_int = left.integer | right.integer;
  	break;
  }
  case TOK_BITWISE_XOR: {
    if (right.lnum == TOK_NUM_FLOAT || left.lnum == TOK_NUM_FLOAT) {
    	MERR("[LINE: %zu]: BITWISE XOR operator not supported for floats", right.lnum);
    }
  	res_int = left.integer ^ right.integer;
  	break;
  }
  case TOK_BITWISE_NOT: {
  	res_int = ~right.integer;
  	break;
  }
  }
  if (right.type == TOK_NUM_INT)
    right.integer = res_int;
  else
    right.decimal = res_dec;
  if (merry_Op_stack_push(expr->op_stack, &right) != MRES_SUCCESS) {
    MERR("Expression parsing failed!", NULL);
    return mfalse;
  }
  return mtrue;
}

_MERRY_INTERNAL_ __TmpRepr convert_operand(Token *tok) {
  __TmpRepr res;
  msize_t len = (msize_t)((mbptr_t)tok->end - (mbptr_t)tok->begin) + 1;
  char num[len];
  memcpy(num, tok->begin, len - 1);
  num[len-1] = '\0';
  switch (tok->type) {
  case TOK_NUM_INT: {
    res.integer = strtoull(num, NULL, 10);
    res.type = TOK_NUM_INT;
    break;
  }
  case TOK_NUM_HEX: {
    res.integer = strtoull(num, NULL, 16);
    res.type = TOK_NUM_INT;
    break;
  }
  case TOK_NUM_OCTAL: {
    res.integer = strtoull(num, NULL, 2);
    res.type = TOK_NUM_INT;
    break;
  }
  case TOK_NUM_BINARY: {
    res.integer = strtoull(num, NULL, 2);
    res.type = TOK_NUM_INT;
    break;
  }
  case TOK_NUM_FLOAT: {
    res.decimal = strtod(num, NULL);
    res.type = TOK_NUM_FLOAT;
    break;
  }
  }
  res.lnum = tok->lnum;
  return res;
}

_MERRY_INTERNAL_ mbool_t push_operand(ExprParser *expr, Token *tok,
                                      mbool_t unary) {
  __TmpRepr conv = convert_operand(tok);
  if (unary) {
    if (conv.type == TOK_NUM_INT)
      conv.integer = -conv.integer;
    else
      conv.decimal = -conv.decimal;
  }
  if (merry_Op_stack_push(expr->op_stack, &conv) != MRES_SUCCESS) {
    MERR("Expression parsing failed!", NULL);
    return mfalse;
  }
  return mtrue;
}

_MERRY_INTERNAL_ mbool_t start_parsing_expr(ExprParser *expr, Lexer *l) {
  Token tok = lexer_next(l);
  msize_t line = tok.lnum;
  token_t top = TOK_ERR;
  while (tok.type != TOK_EOF) {
    switch (tok.type) {
    case TOK_ERR:
      MERR("[LINE: %zu]: Expression parsing error", tok.lnum);
      return mfalse;
    case TOK_NUM_INT:
    case TOK_NUM_FLOAT:
    case TOK_NUM_BINARY:
    case TOK_NUM_OCTAL:
    case TOK_NUM_HEX:
      if (!push_operand(expr, &tok, mfalse)) {
        MERR("[LINE: %zu]: While parsing expression", line);
        return mfalse;
      }
      break;
    case TOK_OPEN_PAREN:
      if (merry_Oper_stack_push(expr->oper_stack, &tok.type) !=
                  MRES_SUCCESS) {
        MERR("[LINE: %zu]: Expression parsing failed", tok.lnum);
        return mfalse;
      }
      top = TOK_OPEN_PAREN;
      break;
    case TOK_CLOSE_PAREN: {
        if (merry_is_stack_empty(expr->oper_stack)) {
        	MERR("[LINE: %zu]: Invalid Expression provided!", line);
        	return mfalse;
        }
        merry_Oper_stack_top(expr->oper_stack, &top);
    	while (!merry_is_stack_empty(expr->oper_stack) && top != TOK_OPEN_PAREN) {
    	  if (!evaluate_operator(expr)) {
    	    MERR("[LINE: %zu]: While parsing expression", line);
    	    return mfalse;
    	  }
    	  merry_Oper_stack_top(expr->oper_stack, &top);	
    	}
    	if (top != TOK_OPEN_PAREN) {
        	MERR("[LINE: %zu]: Invalid Expression provided!", line);
        	return mfalse;
        }
        merry_Oper_stack_pop(expr->oper_stack, &top);
        if (merry_is_stack_empty(expr->oper_stack))
          top = TOK_TMP;
        else
          merry_Oper_stack_top(expr->oper_stack, &top);
        break;
    }
    default: {
      if ((top == TOK_ERR || top == TOK_OPEN_PAREN) && tok.type == TOK_MINUS) {
        // probably a unary '-'
        tok = lexer_next(l);
        if (tok.type == TOK_ERR || tok.type == TOK_EOF) {
          MERR("[LINE: %zu]: Expected a number after unary '-'", tok.lnum);
          return mfalse;
        }
        // PUSH a unary
        if (!push_operand(expr, &tok, mtrue)) {
          MERR("[LINE: %zu]: While parsing expression", line);
          return mfalse;
        }
      } else if ((top == TOK_OPEN_PAREN) || (precedence(top) < precedence(tok.type))) {
        if (merry_Oper_stack_push(expr->oper_stack, &tok.type) !=
            MRES_SUCCESS) {
          MERR("[LINE: %zu]: Expression parsing failed", tok.lnum);
          return mfalse;
        }
        top = tok.type;
      } else if (precedence(top) > precedence(tok.type)) {
        // evaluate the top operator
        while (precedence(top) > precedence(tok.type)) {
          if (!evaluate_operator(expr)) {
            MERR("[LINE: %zu]: While parsing expression", line);
            return mfalse;
          }
          if (merry_Oper_stack_top(expr->oper_stack, &top) != MRES_SUCCESS)
             break;
        }
        if (merry_Oper_stack_push(expr->oper_stack, &tok.type) !=
            MRES_SUCCESS) {
          MERR("[LINE: %zu]: Expression parsing failed", tok.lnum);
          return mfalse;
        }
        top = tok.type;
      } else {
        if (associativity(top) == ASSOCIATIVITY_LEFT) {
          if (!evaluate_operator(expr)) {
            MERR("[LINE: %zu]: While parsing expression", line);
            return mfalse;
          }
        }
        if (merry_Oper_stack_push(expr->oper_stack, &tok.type) !=
            MRES_SUCCESS) {
          MERR("[LINE: %zu]: Expression parsing failed", tok.lnum);
          return mfalse;
        }
        top = tok.type;
      }
    }
    }
    tok = lexer_next(l);
    if (tok.type > TOK_NON_EXPRESSION || tok.type == TOK_ERR ||
       tok.type == TOK_EOF) {
       while (!merry_is_stack_empty(expr->oper_stack)) {
       	  if (!evaluate_operator(expr)) {
             MERR("[LINE: %zu]: Failed to parse expression", line);
       	     return mfalse;
       	  }
       }
       break;
    }
  }
  if (merry_is_stack_empty(expr->op_stack)) {
    MERR("[LINE: %zu]: Invalid Expression!", line);
    return mfalse;
  }
  __TmpRepr result;
  merry_Op_stack_pop(expr->op_stack, &result);
  if (!merry_is_stack_empty(expr->op_stack)) {
    MERR("[LINE: %zu]: Invalid Expression!", line);
    return mfalse;
  }
  expr->type = result.type;
  expr->integer = result.integer;
  return mtrue;
}

ExprParser *create_expr_parser() {
  ExprParser *expr = (ExprParser *)malloc(sizeof(ExprParser));
  if (!expr) {
    MERR("Failed to initialize EXPRESSION PARSER", NULL);
    return NULL;
  }
  if (merry_Oper_stack_init(&expr->oper_stack, 32) != MRES_SUCCESS) {
    MERR("Failed to initialize EXPRESSION PARSER", NULL);
    free(expr);
    return NULL;
  }
  if (merry_Op_stack_init(&expr->op_stack, 64) != MRES_SUCCESS) {
    MERR("Failed to initialize EXPRESSION PARSER", NULL);
    free(expr);
    merry_Oper_stack_destroy(expr->oper_stack);
    return NULL;
  }
  return expr;
}

mbool_t parse_expr(ExprParser *expr, Lexer *l) {
  merry_Oper_stack_clear(expr->oper_stack);
  merry_Op_stack_clear(expr->op_stack);
  return start_parsing_expr(expr, l);
}

void destroy_expr_parser(ExprParser *expr) {
  merry_Op_stack_destroy(expr->op_stack);
  merry_Oper_stack_destroy(expr->oper_stack);
  free(expr);
}
