#ifndef _MERRY_LOG_SYS_
#define _MERRY_LOG_SYS_

#include <merry_logger.h>
#include <stdarg.h>
#include <stdio.h>

void mlog(mloglvl_t lvl, mstr_t msg, ...);

#endif
