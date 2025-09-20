#ifndef _MERRY_PIPE_
#define _MERRY_PIPE_

#include <merry_config.h>
#include <merry_error_stack.h>
#include <merry_platform.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdlib.h>

typedef struct MerryPipe MerryPipe;

struct MerryPipe {
  union {
    mfd_t pfd[2]; // the file descriptors
    struct {
      mdataline_t _read_fd;
      mdataline_t _write_fd;
    };
  };
  mbool_t _in_use;
  mbool_t _rclosed;
  mbool_t _wclosed;
};

MerryPipe *merry_open_merrypipe(MerryErrorStack *st);

void merry_pipe_close_read_end(MerryPipe *pipe);

void merry_pipe_close_write_end(MerryPipe *pipe);

void merry_pipe_close_both_ends(MerryPipe *pipe);

mret_t merry_merrypipe_reopen(MerryPipe *pipe, MerryErrorStack *st);

void merry_destroy_pipe(MerryPipe *pipe);

#endif
