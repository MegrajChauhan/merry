#ifndef _MERRY_CONTEXT_
#define _MERRY_CONTEXT_

#include <merry_utils.h>
#include <merry_types.h>
#include <merry_logger.h>
#include <merry_lexer.h>
#include <merry_file.h>
#include <merry_helpers.h>
#include <stdlib.h>

typedef struct Context Context;

struct Context {
	mstr_t file_path;
	mstr_t file_stream;
	Lexer *lexer;
};

Context* context_create(mstr_t file_path);

mbool_t context_process_file(Context *ctx);

void context_destroy(Context *ctx);

#endif
