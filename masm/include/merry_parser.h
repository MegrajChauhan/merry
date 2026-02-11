#ifndef _MERRY_PARSER_
#define _MERRY_PARSER_

#include <merry_node_types.h>
#include <merry_logger.h>
#include <merry_types.h>
#include <merry_lexer.h>
#include <merry_expr_parser.h>
#include <merry_list.h>
#include <stdlib.h>

_MERRY_DECLARE_DYNAMIC_LIST_(Node, Node*);

typedef struct Parser Parser;

struct Parser {
	MerryDNodeList *node_list;
	ExprParser *expr;
	Lexer *l;
};

Parser *create_parser(Lexer *l, ExprParser *expr);

mbool_t parse_file(Parser *parser); // populates the node_list, obviously

void destroy_parser(Parser *parser);

#endif
