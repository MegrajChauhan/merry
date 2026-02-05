#include <merry_context.h>

Context *context_create(mstr_t file_path) {
  if (merry_is_path_a_directory(file_path)) {
    MERR("[%s]: The given file is actually a directory!", file_path);
    return NULL;
  }
  Context *ctx = (Context *)malloc(sizeof(Context));
  if (!ctx) {
    MERR("[%s]: Failed to initialize file context", file_path);
    return NULL;
  }
  MerryFile *file;
  if (merry_open_file(&file, file_path, _MERRY_FOPEN_READ_, 0) !=
      MRES_SUCCESS) {
    MERR("[%s]: Failed to open file.", file_path);
    // we need to interpret the error(TODO: We need such a function in base)
    free(ctx);
    return NULL;
  }
  msize_t len_of_file = 0;
  merry_file_size(file, &len_of_file); // this shouldn't fail
  if (!len_of_file) {
    MERR("[%s]: Couldn't get file size", file_path);
    free(ctx);
    merry_destroy_file(file);
    return NULL;
  }
  ctx->file_stream = (mstr_t)malloc(len_of_file + 1);
  if (!ctx->file_stream) {
    MERR("[%s]: Failed to allocate buffer for file(length=%zu)", file_path,
         len_of_file);
    free(ctx);
    merry_destroy_file(file);
    return NULL;
  }
  if (merry_file_read(file, NULL, ctx->file_stream, len_of_file) !=
      MRES_SUCCESS) {
    MERR("[%s]: Failed to read file", file_path);
    free(ctx);
    merry_destroy_file(file);
    return NULL;
  }
  ctx->file_path = file_path;
  ctx->lexer = lexer_init(ctx->file_stream, len_of_file);
  if (!ctx->lexer) {
    MERR("[%s]: Failed to initialize lexer", file_path);
    free(ctx->file_stream);
    free(ctx);
    merry_destroy_file(file);
    return NULL;
  }
  merry_destroy_file(file);
  return ctx;
}

mbool_t context_process_file(Context *ctx) {
  Token curr = lexer_next(ctx->lexer);
  while (curr.type != TOK_ERR && curr.type != TOK_EOF) {
    printf("TOK_TYPE: %zu\nTOK_LINE: %zu\nTOK_COL: %zu\nTOK_VAL: ", curr.type,
           curr.lnum, curr.col_st);
    mstr_t itr = curr.begin;
    while (itr != curr.end) {
      putchar(*itr);
      itr++;
    }
    putchar(10);
    curr = lexer_next(ctx->lexer);
  }
  return true;
}

void context_destroy(Context *ctx) {
  if (ctx) {
    if (ctx->file_stream)
      free(ctx->file_stream);
    lexer_destroy(ctx->lexer);
    free(ctx);
  }
}
