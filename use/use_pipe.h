#ifndef _USE_PIPE_
#define _USE_PIPE_

#include <use_defs.h>
#include <use_utils.h>
#include <use_interface.h>

typedef Interface Pipe;

extern result_t open_pipe(Pipe **pipe) _ALIAS_(merry_open_merrypipe);

extern result_t pipe_close_read_end(Pipe *pipe)
    _ALIAS_(merry_pipe_close_read_end);

extern result_t pipe_close_write_end(Pipe *pipe)
    _ALIAS_(merry_pipe_close_write_end);

extern result_t pipe_close_both_ends(Pipe *pipe)
    _ALIAS_(merry_pipe_close_both_ends);

extern result_t pipe_reopen(Pipe *pipe) _ALIAS_(merry_merrypipe_reopen);

extern result_t destroy_pipe(Pipe *pipe) _ALIAS_(merry_destroy_pipe);

#endif
