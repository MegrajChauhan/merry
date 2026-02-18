/*
 * Program that can print things
 * 
 * proc print_byte_stream ; void print_byte_stream(char *stream, size_t len);
 * 
 * ; R0 = address to the stream, R1 = length of stream. Doesn't gurantee register states
 * print_byte_stream:
 *   mov_reg R2, R1   ; save the length in R2 so that loop instruction can use it
 *   loadb_reg R1, R0 ; load the byte
 *   inc R0           ; the next byte
 *   bout R1          ; output the byte
 *   dec R2           ; next byte
 *   loop print_byte_stream
 *   ret
 * 
 * db message: "Hello World!", 10
 * dq len: 13
 * 
 * proc main
 * 
 * main:
 *   mov_imm R0, message ; store the address of message
 *   loadq_imm R1, len   ; load the length of message
 *   call print_byte_stream
 *   hlt                 ; stop the program
 */

 /*
 The program in decimal:

 data:
 'H' 'e' 'l' 'l' 'o' ' ' 'w' 'o' 'r' 'l' 'd' '!' 10
 00 00 00 00 00 00 00 13
 starting address 00H and ending address 0dH

 code(decimal)(added first jmp instruction to jump to main):
 40 00 00 00 00 00 00 52
 25 00 00 00 00 00 00 21
136 00 00 00 00 00 00 10
 21 00 00 00 00 00 00 00
127 00 00 00 00 00 00 01
 22 00 00 00 00 00 00 02
101 00 00 00 00 00 00 08
 72 00 00 00 00 00 00 00
 23 00 00 00 00 00 00 00
134 01 00 00 00 00 00 13
 71 00 00 00 00 00 00 08
 00 00 00 00 00 00 00 00
 */
#include <stdio.h>

size_t program[] = {
 40 00 00 00 00 00 00 52
 25 00 00 00 00 00 00 21
136 00 00 00 00 00 00 10
 21 00 00 00 00 00 00 00
127 00 00 00 00 00 00 01
 22 00 00 00 00 00 00 02
101 00 00 00 00 00 00 08
 72 00 00 00 00 00 00 00
 23 00 00 00 00 00 00 00
134 01 00 00 00 00 00 13
 71 00 00 00 00 00 00 08
 00 00 00 00 00 00 00 00
	
};

int main() {
	
}
 
