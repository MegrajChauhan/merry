#ifndef _MERRY_LOG_SUBSYSTEM_
#define _MERRY_LOG_SUBSYSTEM_

#include <merry_logger.h>
#include <stdarg.h>
#include <stdio.h>

// Definitions for functions that do the displaying
void merry_log__(mloglvl_t lvl, mstr_t msg, ...);

#endif
