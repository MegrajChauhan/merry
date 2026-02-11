#include <merry_parser.h>

_MERRY_DEFINE_DYNAMIC_LIST_(Node, Node*);

_MERRY_INTERNAL_ token_t identify_keyword(Parser *p, Token *tok) {
    // using switch-statements instead
    msize_t len = (msize_t)(tok->end - tok->start);
	switch (*tok->begin) {
		case 'd': {
            if (len == 2) {
			  switch (*(tok->begin+1)) {
			   	case 'b':
			   	  return TOK_DB;
			   	case 'w':
			   	  return TOK_DW;
			   	case 'd':
			   	  return TOK_DD;
			   	case 'q':
			   	  return TOK_DQ;
			   	default:
			   	  MERR("[LINE: %zu]: Unknown keyword 'd%c'", tok->lnum, *(tok->begin+1));
			   	  return TOK_ERR;
			  }
            }
		}
	}
	char key[len + 1];
	memcpy(key, tok->begin, len);
	key[len] = 0;
	MERR("[LINE: %zu]: Unknown keyword '%s'", tok->lnum, key);
	return TOK_ERR;
}

// TODO!!!!
_MERRY_INTERNAL_ mbool_t handle_variable_defins(Parser *p, token_t ) {
	mdatatype_t type;
    Token var_name = lexer_next(p->l);
    if (var_name.type != TOK_IDENTIFIER) 
}


Parser *create_parser(Lexer *l, ExprParser *expr) {
	Parser *p = (Parser*)malloc(sizeof(Parser));
	if (!p) {
		MERR("Failed to initialize Parser", NULL);
		return NULL;
	}
	if (merry_dNode_list_create(100, &p->node_list) != MRES_SUCCESS) {
		MERR("Failed to intialize Parser", NULL);
		free(p);
		return NULL;
	}
	p->expr = expr;
	p->l = l;
	return p;
}

mbool_t parse_file(Parser *parser) {
	Token tok = lexer_next(parser->l);
	while (tok != TOK_EOF) {
		switch (tok.type) {
			case TOK_ERR:
			  MERR("Failed to parse the file!", NULL);
			  return mfalse;
			case TOK_IDENTIFIER: {
				// handle the identifier
				// it must be a keyword
			}
		}
	}
}

void destroy_parser(Parser *parser) {
    merry_dNode_list_destroy(parser->node_list);
	free(parser);
}
