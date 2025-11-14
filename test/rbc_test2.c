#include <stdint.h>
#include <stdio.h>

/*
 * n factorial and print it
 * PROGRAM IN ASM                                  INSTRUCTIONS
 * entry:
 *   mov64 R0, 20(msg len) 3e 00 00 00 00 00 00 00 00 00 00 00 00 00 00 14
 *   sout msg  bb 00 00 00 00 00 00 00
 *
 *   ; Now we have to take input
 *   uinq R2 ; read the +ve number  ca 00 00 00 00 00 00 02
 *   in R0   ; read the new line    bc 00 00 00 00 00 00 00
 *
 *   mov_reg R10, R2                41 00 00 00 00 00 0a 02
 * factorial:
 *    dec R2                        3d 00 00 00 00 00 00 02
 *    mul R10, R2                   13 00 00 00 00 00 00 a2
 *    cmp R2, 1                     b0 00 00 00 00 00 00 02 00 00 00 00 00 00 00 01
 *    jne factorial ; loop will loop until R2 == 0 66 00 00 00 00 00 00 30
 *
 *    ; displaying the result
 *    mov64 R0, 17(res len) 3e 00 00 00 00 00 00 00 00 00 00 00 00 00 00 11
 *    sout res bb 00 00 00 00 00 00 00 15
 *    uoutq R10 cb 00 00 00 00 00 00 00 0a
 *
 *    mov64 R0, 10 ; new line 3e 00 00 00 00 00 00 00 00 00 00 00 00 00 00 0a
 *    cout R0 b9 00 00 00 00 00 00 00
 *
 *    hlt ; done 01 00 00 00 00 00 00 00
 * DATA:
 *   msg: "Enter a +ve number: "
 *   res: "The factorial is "
 * */

int main() {
  uint64_t instructions[19] = {
      0x3e00000000000000, 0x0000000000000014,
      0xbb00000000000000,
      0xca00000000000002,
      0x0000000000000000,
      0x4100000000000a02,
      0x3d00000000000002,
      0x13000000000000a2,
      0xb000000000000002, 0x0000000000000001,
      0x6600000000000030,
      0x3e00000000000000, 0x0000000000000011,
      0xbb00000000000015,
      0xcb0000000000000a,
      0x3e00000000000000, 0x000000000000000a,
      0xb900000000000000,
      0x0100000000000000,
  };
  uint8_t data[37] = {'E', 'n', 't', 'e', 'r', ' ', 'a', ' ', '+', 'v', 'e',
                      ' ', 'n', 'u', 'm', 'b', 'e', 'r', ':', ' ', 'T', 'h',
                      'e', ' ', 'f', 'a', 'c', 't','o','r','i','a','l',' ', 'i', 's', ' '};

  uint8_t header[32] = {
      'R', 'I', 'F', 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 152,
      0, 0,   0,   0, 0, 0, 0, 37,
      0, 0, 0, 0, 0, 0, 0, 0,
  };

  const char *metadata_file = "MMF\x0\x0\x0\x0\x1"
                              "\x0\x0\x0\x0\x0\x0\x0\x1" // REGR_CORE
                              "test_results/rbc_test2.rc\0";
  FILE *metadata = fopen("test_results/rbc_test.mdat", "wb");
  FILE *rbc_prog = fopen("test_results/rbc_test2.rc", "wb");

  fwrite(metadata_file, 1, 42, metadata);
  fwrite(header, 1, 32, rbc_prog);
  fwrite(instructions, 8, 19, rbc_prog);
  fwrite(data, 1, 37, rbc_prog);
  fclose(metadata);
  fclose(rbc_prog);
  return 0;
}
