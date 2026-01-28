#ifndef _MERRY_LOGGER_
#define _MERRY_LOGGER_

#include <merry_config.h>
#include <merry_defs.h>
#include <merry_helpers.h>
#include <stdio.h>

typedef enum mloglvl_t mloglvl_t;
typedef struct MerryLogger MerryLogger;

enum mloglvl_t {
	MERRY_LOG_NOTE,
	MERRY_LOG_WARN,
	MERRY_LOG_ERR,
	MERRY_LOG_DBG,
	MERRY_LOG_LPOINTS, // log points(extra information here and there)
};

_MERRY_DEFINE_FUNC_PTR_(void, log_func_t,mloglvl_t,  mstr_t, ...);

struct MerryLogger {
	mloglvl_t log_upto;
	log_func_t logger;
};

_MERRY_EXTERNAL_ MerryLogger logger;

void merry_init_logger(log_func_t func, mloglvl_t log_upto);

#define MLOG(lvl, msg, ...) do {if (logger.log_upto >= lvl)logger.logger(lvl, msg, __VA_ARGS__); } while(0)
#define MERR(msg, ...) MLOG(MERRY_LOG_ERR, msg, __VA_ARGS__)
#define MNOTE(msg, ...) MLOG(MERRY_LOG_NOTE, msg, __VA_ARGS__)
#define MWARN(msg, ...) MLOG(MERRY_LOG_WARN, msg, __VA_ARGS__)
#define MDBG(msg, ...) MLOG(MERRY_LOG_DBG, msg, __VA_ARGS__)
#define MLPOINTS(msg, ...) MLOG(MERRY_LOG_LPOINTS, msg, __VA_ARGS__)

#endif
