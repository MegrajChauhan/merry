#include <merry_lexer.h>

_MERRY_INTERNAL_ void lexer_update(Lexer *l) {
  if (l->len <= l->curr)
    return; // nothing to do
  if ((l->curr < l->len) && l->stream[l->curr] == '\n') {
    l->curr++;
    l->lnum++;
    l->col = 1;
  } else {
    l->curr++;
    l->col++;
  }
  return;
}

_MERRY_INTERNAL_ void lexer_handle_decimal(Lexer *l, Token *t) {
  size_t dot_count = 0;
  while (l->curr < l->len &&
         (misnum(l->stream[l->curr]) || l->stream[l->curr] == '.')) {
    if (l->stream[l->curr] == '.')
      dot_count++;
    if (dot_count > 1) {
      MERR("[LINE:%zu]: Floating point number cannot have multiple floating "
           "points",
           l->lnum);
      t->type = TOK_ERR;
      return;
    }
    lexer_update(l);
  }
  if (dot_count > 0)
    t->type = TOK_NUM_FLOAT;
  else
    t->type = TOK_NUM_INT;
  return;
}

_MERRY_INTERNAL_ void lexer_handle_binary(Lexer *l, Token *t) {
  lexer_update(l); // skip the 'b'
  lexer_update(l); // the actual number
  msize_t len = 0;
  while (l->curr < l->len &&
         (l->stream[l->curr] == '0' || l->stream[l->curr] == '1')) {
    lexer_update(l);
    len++;
  }
  if (len == 0) {
    MERR("[LINE:%zu]: Expected an actual bit sequence after prefix '0b'",
         l->lnum);
    t->type = TOK_ERR;
    return;
  }
  t->type = TOK_NUM_BINARY;
  return;
}

_MERRY_INTERNAL_ void lexer_handle_hex(Lexer *l, Token *t) {
  lexer_update(l);
  lexer_update(l);
  msize_t len = 0;
  while (l->curr < l->len &&
         (misnum(l->stream[l->curr]) ||
          ((l->stream[l->curr] >= 'A' && l->stream[l->curr] <= 'F') ||
           (l->stream[l->curr] >= 'a' && l->stream[l->curr] <= 'f')))) {
    if ((l->stream[l->curr] >= 'a' && l->stream[l->curr] <= 'f'))
      l->stream[l->curr] = l->stream[l->curr] - 32;
    lexer_update(l);
    len++;
  }
  if (len == 0) {
    MERR("[LINE:%zu]: Expected an actual hex sequence after prefix '0x'",
         l->lnum);
    t->type = TOK_ERR;
    return;
  }
  t->type = TOK_NUM_HEX;
  return;
}

_MERRY_INTERNAL_ void lexer_handle_octal(Lexer *l, Token *t) {
  lexer_update(l);
  lexer_update(l);
  msize_t len = 0;
  while (l->curr < l->len &&
         (misnum(l->stream[l->curr]) &&
          ((l->stream[l->curr] >= '0' && l->stream[l->curr] <= '7')))) {
    lexer_update(l);
    len++;
  }
  if (len == 0) {
    MERR("[LINE:%zu]: Expected an actual octal sequence after prefix '0o'",
         l->lnum);
    t->type = TOK_ERR;
    return;
  }
  t->type = TOK_NUM_OCTAL;
  return;
}

// Why aren't we making extra checks?
// Well, we will make sure that incorrect arguments are never passed
Lexer *lexer_init(mstr_t stream, msize_t len) {
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
  while ((l->curr < l->len) && misspace(l->stream[l->curr]))
    lexer_update(l);
  while ((l->curr < l->len) && l->stream[l->curr] == ';') {
    while ((l->curr < l->len) && l->stream[l->curr] != '\n')
      lexer_update(l);
    while ((l->curr < l->len) && misspace(l->stream[l->curr]))
      lexer_update(l);
  }
  res.lnum = l->lnum;
  if (l->len <= l->curr) {
    res.type = TOK_EOF;
    return res;
  }
  // it is something
  res.col_st = l->col;
  res.begin = &l->stream[l->curr];
  if (l->stream[l->curr] == '+') {
    res.type = TOK_PLUS;
    lexer_update(l);
  } else if (l->stream[l->curr] == '-') {
    res.type = TOK_MINUS;
    lexer_update(l);
  } else if (l->stream[l->curr] == '*') {
    res.type = TOK_MUL;
    lexer_update(l);
  } else if (l->stream[l->curr] == '/') {
    res.type = TOK_DIV;
    lexer_update(l);
  } else if (l->stream[l->curr] == '(') {
    res.type = TOK_OPEN_PAREN;
    lexer_update(l);
  } else if (l->stream[l->curr] == ')') {
    res.type = TOK_CLOSE_PAREN;
    lexer_update(l);
  } else if (l->stream[l->curr] == '%') {
    res.type = TOK_MODULO;
    lexer_update(l);
  } else if (misnum(l->stream[l->curr])) {
    msize_t peek = l->curr + 1;
    if (l->stream[l->curr] == '0' && l->len > peek && l->stream[peek] == 'x')
      lexer_handle_hex(l, &res);
    else if (l->stream[l->curr] == '0' && l->len > peek &&
             l->stream[peek] == 'b')
      lexer_handle_binary(l, &res);
    else if (l->stream[l->curr] == '0' && l->len > peek &&
             l->stream[peek] == 'o')
      lexer_handle_octal(l, &res);
    else if (l->stream[l->curr] == '0' && l->len > l->curr &&
             misalpha(l->stream[peek])) {
      MERR("[LINE:%zu]: Unknown prefix used '0%c'", l->lnum, l->stream[peek]);
      res.type = TOK_ERR;
    } else {
      lexer_handle_decimal(l, &res);
    }
  } else {
    MERR("[LINE:%zu]: Cannot build a token from this '%c'", l->lnum,
         l->stream[l->curr]);
    res.type = TOK_ERR;
  }
  res.end = &l->stream[l->curr];
  return res;
}

Token lexer_peek(Lexer *l) {
  msize_t curr = l->curr;
  msize_t lnum = l->lnum;
  msize_t col = l->col;
  Token res = lexer_next(l);
  l->curr = curr;
  l->lnum = l->lnum;
  l->col = col;
  return res;
}

void lexer_destroy(Lexer *l) {
  free(l); // we don't know we received stream
}
