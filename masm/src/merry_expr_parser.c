#include <merry_expr_parser.h>

#define ASSOCIATIVITY_LEFT 0
#define ASSOCIATIVITY_RIGHT 1

_MERRY_DEFINE_STACK_(Temp, __AstTmpRepr);

_MERRY_INTERNAL_ int associativity(token_t type) {
  switch (type) {
  case TOK_PLUS:
  case TOK_MINUS:
    return ASSOCIATIVITY_LEFT;
  }
  return 2; // this will never be executed[I am 100% sure!!!! :) ]
}

_MERRY_INTERNAL_ int precedence(token_t type) {
  switch (type) {
  case TOK_ERR:
    return 0;
  case TOK_MINUS:
    return 1;
  case TOK_PLUS:
    return 2;
  }
  return 0;
}

_MERRY_INTERNAL_ mbool_t evaluate_operator(ExprParser *expr) {
  token_t top;
  __TmpRepr right, left;
  if (merry_Op_stack_pop(expr->op_stack, &right) != MRES_SUCCESS) {
    MERR("Expression parsing failed!", NULL);
    return mfalse;
  }
  if (merry_Op_stack_pop(expr->op_stack, &left) != MRES_SUCCESS) {
    MERR("Expression parsing failed!", NULL);
    return mfalse;
  }
  if (merry_Oper_stack_pop(expr->oper_stack, &top) != MRES_SUCCESS) {
    MERR("Expression parsing failed!", NULL);
    return mfalse;
  }
  if (right.type != left.type) {
    MERR("[LINE: %zu]: Mismatch of value types for expression", right.lnum);
    return mfalse;
  }
  mqword_t res_int;
  double res_dec;
  switch (top) {
        case TOK_PLUS;
    	   res_int = right.integer + left.integer;
    	   res_dec = right.decimal + left.decimal;
    	   break;
    	case TOK_MINUS;
    	   res_int = right.integer - left.integer;
    	   res_dec = right.decimal - left.decimal;
    	   break;
    	case TOK_MUL;
    	   res_int = right.integer * left.integer;
    	   res_dec = right.decimal * left.decimal;
    	   break;
    	case TOK_DIV;
    	   if (left.integer == 0 || left.decimal == 0.0) {
    	   	MERR("[LINE: %zu]: Divide by ZERO!", right.lnum);
    	   	return mfalse;
    	   }
    	   res_int = right.integer / left.integer;
    	   res_dec = right.decimal / left.decimal;
    	   break;
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

_MERRY_INTERNAL_ mbool_t push_operand(ExprParser *expr, Token *tok, mbool_t unary) {
	// if unary, then do '-operand', else just push the operand after conversion
    // TODO: Finish this shit!
}

_MERRY_INTERNAL_ mbool_t start_parsing_expr(ExprParser *expr, Lexer *l) {
	Token tok = lexer_next(l);
	token_t top = TOK_ERR;
	while (tok.type != TOK_EOF) {
		switch (tok.type) {
			case TOK_ERR:
          MERR("[LINE: %zu]: Expression parsing error", tok.lnum);
          return mfalse;
        case TOK_NUM_INT:
        case TOK_NUM_FLOAT:
          if (!push_operand(expr, &tok, mfalse))
            return mfalse;
        default: {
          if ((top == TOK_ERR) && tok.type == TOK_MINUS) {
            // probably a unary '-'
            tok = lexer_next(l);
            if (tok.type == TOK_ERR) {
              MERR("[LINE: %zu]: Expected a number after unary '-'", tok.lnum);
              return mfalse;
            }
            // PUSH a unary
            if (!push_operand(expr, &tok, mtrue))
              return mfalse;
          } else if (precedence(top) < precedence(tok.type)) {
            if (merry_Oper_stack_push(expr->oper_stack, tok.type) !=
                MRES_SUCCESS) {
              MERR("[LINE: %zu]: Expression parsing failed", tok.lnum);
              return mfalse;
            }
            top = tok.type;
          } else if (precedence(top) > precedence(tok.type)) {
            // evaluate the top operator
            if (!evaluate_operator(expr))
              return mfalse;
            if (merry_Oper_stack_push(expr->oper_stack, tok.type) !=
                MRES_SUCCESS) {
              MERR("[LINE: %zu]: Expression parsing failed", tok.lnum);
              return mfalse;
            }
            top = tok.type;
          } else {
            if (associativity(top) == ASSOCIATIVITY_LEFT) {
              if (!evaluate_operator(expr))
                return mfalse;
            }
            if (merry_Oper_stack_push(expr->oper_stack, tok.type) !=
                MRES_SUCCESS) {
              MERR("[LINE: %zu]: Expression parsing failed", tok.lnum);
              return mfalse;
            }
            top = tok.type;
          }
        }
        }
        tok = lexer_next(l);
        if (tok.type > TOK_NON_EXPRESSION) {
          // TODO: The expression has ended. Do something!!!!
          break;
        }
}
return mtrue;
}

mbool_t parse_expr(ExprParser *expr, Lexer *l) {
  if (merry_Oper_stack_init(&expr->oper_stack, 32) != MRES_SUCSESS) {
    MERR("Failed to initialize EXPRESSION PARSER", NULL);
    return mfalse;
  }
  if (merry_Op_stack_init(&expr->op_stack, 64) != MRES_SUCSESS) {
    MERR("Failed to initialize EXPRESSION PARSER", NULL);
    merry_Oper_stack_destroy(&ast->oper_stack);
    return mfalse;
  }

  return mtrue;
}

void ast_destroy(Ast ast);
