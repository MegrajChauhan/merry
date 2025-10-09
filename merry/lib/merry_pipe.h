#ifndef _MERRY_PIPE_
#define _MERRY_PIPE_

#include <merry_config.h>
#include <merry_interface.h>
#include <merry_logger.h>
#include <merry_platform.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdlib.h>

typedef struct MerryInterface MerryPipe;

MerryPipe *merry_open_merrypipe();

minterfaceRet_t merry_pipe_close_read_end(MerryPipe *pipe);

minterfaceRet_t merry_pipe_close_write_end(MerryPipe *pipe);

minterfaceRet_t merry_pipe_close_both_ends(MerryPipe *pipe);

minterfaceRet_t merry_merrypipe_reopen(MerryPipe *pipe);

minterfaceRet_t merry_destroy_pipe(MerryPipe *pipe);

#endif
