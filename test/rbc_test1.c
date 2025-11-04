#include <stdint.h>
#include <stdio.h>

/*
 * Get a number 'n' from User:
 * Sum first n natural numbers and print it
 * PROGRAM IN ASM                                  INSTRUCTIONS
 * entry:
 *   mov64 R0, 20(msg len) 3e 00 00 00 00 00 00 00 00 00 00 00 00 00 00 14
 *   sout msg  bb 00 00 00 00 00 00 00
 *
 *   ; Now we have to take input
 *   uinq R2 ; read the +ve number  ca 00 00 00 00 00 00 02
 *   in R0   ; read the new line    bc 00 00 00 00 00 00 00
 *
 *   xor R10, R10                   aa 00 00 00 00 00 0a 0a
 * sum:
 *    add R10, R2                   0f 00 00 00 00 00 00 a2
 *    dec R2                        3d 00 00 00 00 00 00 02
 *    loop sum ; loop will loop until R2 == 0 86 00 00 00 00 00 00 30
 *
 *    ; displaying the result
 *    mov64 R0, 11(res len) 3e 00 00 00 00 00 00 00 00 00 00 00 00 00 00 0b
 *    sout res bb 00 00 00 00 00 00 00 15
 *    uoutq R10 cb 00 00 00 00 00 00 00 0a
 *
 *    mov64 R0, 10 ; new line 3e 00 00 00 00 00 00 00 00 00 00 00 00 00 00 0a
 *    cout R0 b9 00 00 00 00 00 00 00
 *
 *    hlt ; done 01 00 00 00 00 00 00 00
 * DATA:
 *   msg: "Enter a +ve number: "
 *   res: "The sum is "
 * */

int main() {
  uint64_t instructions[17] = {
      0x3e00000000000000, 0x0000000000000014, 0xbb00000000000000,
      0xca00000000000002, 0xbc00000000000000, 0xaa00000000000a0a,
      0x0f000000000000a2, 0x3d00000000000002, 0x8600000000000030,
      0x3e00000000000000, 0x000000000000000b, 0xbb00000000000015,
      0xcb0000000000000a, 0x3e00000000000000, 0x000000000000000a,
      0xb900000000000000, 0x0100000000000000,
  };
  uint8_t data[31] = {'E', 'n', 't', 'e', 'r', ' ', 'a', ' ', '+', 'v', 'e',
                      ' ', 'n', 'u', 'm', 'b', 'e', 'r', ':', ' ', 'T', 'h',
                      'e', ' ', 's', 'u', 'm', ' ', 'i', 's', ' '};

  uint8_t header[32] = {
      'R', 'I', 'F', 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0x88,
      0,   0,   0,   0, 0, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0, 0,
  };

  const char *metadata_file = "MMF\x0\x0\x0\x0\x1"
                              "\x0\x0\x0\x0\x0\x0\x0\x1" // REGR_CORE
                              "test_results/rbc_test1.rc\0";
  FILE *metadata = fopen("test_results/metadata.mdat", "wb");
  FILE *rbc_prog = fopen("test_results/rbc_test1.rc", "wb");

  fwrite(metadata_file, 1, 42, metadata);
  fwrite(header, 1, 32, rbc_prog);
  fwrite(instructions, 8, 17, rbc_prog);
  fwrite(data, 1, 31, rbc_prog);
  fclose(metadata);
  fclose(rbc_prog);
  return 0;
}
