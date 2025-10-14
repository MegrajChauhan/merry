#include <stdio.h>

int main() {
  const char *metadata_file =
      "MMF\x0\x0\x0\x0\x1"
      "\x0\x0\x0\x0\x0\x0\x0\x0" // first entry, TEST CORE
      "test_results/tc_test_6.tst\0";
  const char *tc_test_program = "\x5\x1";

  FILE *metadata = fopen("test_results/metadata_6.mdat", "wb");
  FILE *tc_t_prog = fopen("test_results/tc_test_6.tst", "wb");

  fwrite(metadata_file, 1, 43, metadata);
  fwrite(tc_test_program, 1, 2, tc_t_prog);
  fclose(metadata);
  fclose(tc_t_prog);
  return 0;
}
