@ File: HW322.s
@ Author: R. Kevin Preston
@ Purpose: Code to check out Homework problem 3.22 in Text 
@
@ Use these commands to assemble, link and run the program
@
@  as -o HW322.o HW322.s
@  gcc -o HW322 HW322.o
@ ./HW322 ;echo $?
@ gdb --args ./HW322
@

@ Write the ARM code for the following:
@
@ int s =0;
@ for (i=0; i< 10; i++) {s = s + i*i;}
@
@ Sum of the squares from 0 to 9.
@   r5: used for s
@   r6: used for i
@   r7: hold i*i calculation
@
.global main
main:

      LDR  r0, =string1
      BL   printf
@
@ Implement this the way the C code is written. 
@ -increment the counter.
@ -do the square
@ -add to s
@
      MOV  r6, #0
      MOV  r5, #0
Loop:
      MUL  r7, r6, r6  @Calculate i*i
      ADD  r5, r5, r7  @ s= s+ i*i
      ADD  r6, r6, #1  @ i++
      CMP  r6, #10     @ i < 10?
      BNE  Loop

      LDR  r0, =string2
      MOV  r1, r5
      BL   printf

@
@ Change the loop and use some differnt instructions to make is more effficent. 
@  - decrement the counter
@  - Use the multiply and accumulate instruction.
@
      LDR  r0, =string3
      BL   printf

      MOV  r6, #9  @Count down this time.
      MOV  r5, #0
Loop2:
      MLA  r5, r6, r6, r5 @ s = s + i*i
      SUBS r6, r6, #1     @ i = i - 1
      BNE  Loop2

      LDR  r0, =string2
      MOV  r1, r5
      BL   printf

@ Force the exit of this program and return command to OS
Exit:
    MOV  r7, #0X01
    SVC  0

@ Declare the stings

.data       @ Lets the OS know it is OK to write to this area of memory. 

.balign 4

string1: .asciz "This program calculates the sum of squares from 0 to 9.\n"
string2: .asciz "The result is: %d \n\n"
string3: .asciz "Change the loop around. \n\n"

.global printf

@end of code and end of file. Leave a blank line after this. 
