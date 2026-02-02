#include <merry_lexer.h>

_MERRY_INTERNAL_ void lexer_update(Lexer *l) {
	if (l->len <= l->curr)
	    return; // nothing to do
	 if((l->curr < l->len) && l->stream[l->curr] == '\n') {
	   l->curr++;
	   l->lnum++;
	   l->col = 1;	
	}else{
		l->curr++;
		l->col++;
	}
	return;
}

// Why aren't we making extra checks?
// Well, we will make sure that incorrect arguments are never passed
Lexer* lexer_init(mstr_t stream, msize_t len) {
	Lexer *l = (Lexer *)malloc(sizeof(Lexer));
	if (!l) {
		MERR("Failed to initialize LEXER(Memory allocation failure)", NULL);
		return NULL;
	}
	l->stream = stream;
	l->len = len;
	l->curr = 0;
	l->lnum = 1;
	l->col = 1;
	return l;
}

Token lexer_next(Lexer *l) {
	Token res;
	res.lnum = l->lnum;
    while ((l->curr < l->len) && isspace(l->stream[l->curr]))
        lexer_update(l);
	if (l->len == l->curr) {
		res.type = TOK_EOF;
		return res;
	}
    // it is something
    res.col_st = l->col;
    res.begin = &l->stream[l->curr];
    if (*res.begin == '+')
      res.type = TOK_PLUS;
    else
    return res;
}

Token lexer_peek(Lexer *l) {
	
}

void lexer_destroy(Lexer *l) {
	free(l); // we don't know we received stream
}
