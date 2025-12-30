#include <lib/merry_pipe.h>
#include <use_pipe.h>

_ALWAYS_INLINE_ result_t open_pipe(Pipe **pipe) {
  return merry_open_merrypipe(pipe);
}

_ALWAYS_INLINE_ result_t pipe_close_read_end(Pipe *pipe) {
  return merry_pipe_close_read_end(pipe);
}

_ALWAYS_INLINE_ result_t pipe_close_write_end(Pipe *pipe) {
  return merry_pipe_close_write_end(pipe);
}

_ALWAYS_INLINE_ result_t pipe_close_both_ends(Pipe *pipe) {
  return merry_pipe_close_both_ends(pipe);
}

_ALWAYS_INLINE_ result_t pipe_reopen(Pipe *pipe) {
  return merry_merrypipe_reopen(pipe);
}

_ALWAYS_INLINE_ result_t destroy_pipe(Pipe *pipe) {
  return merry_destroy_pipe(pipe);
}
