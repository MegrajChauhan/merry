#ifndef _USE_PIPE_
#define _USE_PIPE_

#include <use_defs.h>
#include <use_utils.h>
#include <use_interface.h>

typedef Interface Pipe;

result_t open_pipe(Pipe **pipe);

result_t pipe_close_read_end(Pipe *pipe);

result_t pipe_close_write_end(Pipe *pipe);

result_t pipe_close_both_ends(Pipe *pipe);

result_t pipe_reopen(Pipe *pipe);

result_t destroy_pipe(Pipe *pipe);

#endif
