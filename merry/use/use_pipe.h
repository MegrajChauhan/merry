#ifndef _USE_PIPE_
#define _USE_PIPE_

#include <merry_pipe.h>
#include <use_defs.h>
#include <use_utils.h>

typedef MerryPipe Pipe;

extern result_t open_pipe(Pipe **pipe) _ALIAS_(merry_open_merrypipe);

extern interfaceRet_t pipe_close_read_end(Pipe *pipe)
    _ALIAS_(merry_pipe_close_read_end);

extern interfaceRet_t pipe_close_write_end(Pipe *pipe)
    _ALIAS_(merry_pipe_close_write_end);

extern interfaceRet_t pipe_close_both_ends(Pipe *pipe)
    _ALIAS_(merry_pipe_close_both_ends);

extern interfaceRet_t pipe_reopen(Pipe *pipe) _ALIAS_(merry_merrypipe_reopen);

extern interfaceRet_t destroy_pipe(Pipe *pipe) _ALIAS_(merry_destroy_pipe);

#endif
