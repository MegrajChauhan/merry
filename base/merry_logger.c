#include <merry_logger.h>

MerryLogger logger;

_MERRY_ALWAYS_INLINE_ void merry_init_logger(log_func_t func,
                                             mloglvl_t log_upto) {
  logger.logger = func;
  logger.log_upto = log_upto;
}
