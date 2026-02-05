#include <merry_context.h>
#include <merry_log_sys.h>
#include <merry_logger.h>

int main(int argc, char **argv) {
  merry_init_logger(mlog, MERRY_LOG_ERR);
  Context *ctx = context_create(argv[1]);
  if (!ctx) {
    printf("Abnormal Termination\n");
    return 0;
  }
  context_process_file(ctx);
  context_destroy(ctx);
  return 0;
}
