#include <merry_log_sys.h>

_MERRY_INTERNAL_ mcstr_t lvl_str[] = {
    "INFO", "WARN", "ERROR", "DEBUG", "MSG",
};

void mlog(mloglvl_t lvl, mstr_t msg, ...) {
  fprintf(stdout, "[%s]: ", lvl_str[lvl]);

  va_list args;
  va_start(args, msg);
  vfprintf(stdout, msg, args);
  va_end(args);

  fprintf(stdout, "\n");
}
