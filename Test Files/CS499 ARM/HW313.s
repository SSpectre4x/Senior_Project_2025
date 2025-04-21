@ File: HW313.s
@ Author: R. Kevin Preston
@ Purpose: Provide enough assembly to allow students to complete an assignment. 
@
@ Use these commands to assemble, link and run the program
@
@  as -o HW313.o HW313.s
@  gcc -o HW313 HW313.o
@ ./HW313 ;echo $?
@ gdb --args ./HW313
@

.global main
main:

    LDR  r1, =TwoFs
    LDR  r1, [r1]

    ADD  r0, r1, r1, LSL #2 
    MOV  r2, r0

    ADD  r0, r1, r1, LSL #4
    MOV  r3, r0

    ADD  r0, r1, r1, ROR #4
    MOV  r4, r0

   

@ Force the exit of this program and return command to OS
Exit:
    MOV  r7, #0X01
    SVC  0

@ Declare the stings

.data       @ Lets the OS know it is OK to write to this area of memory. 

.balign 4

TwoFs: .word   0xFF @in hex 

.global printf

@end of code and end of file. Leave a blank line after this. 
