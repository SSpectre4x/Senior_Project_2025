@ File: HW312.s
@ Author: R. Kevin Preston
@ Purpose: Provide enough assembly to allow students to complete an assignment. 
@
@ Use these commands to assemble, link and run the program
@
@  as -o HW312.o HW312.s
@  gcc -o HW312 HW312.o
@ ./HW312 ;echo $?
@ gdb --args ./HW312
@

.global main
main:

    LDR  r1, =ThreeFs
    LDR  r1, [r1]
    MOV  r2, #4
    MOV  r3, r1, LSL r2
    MOV  r4, r3
    MOV  r3, r1, LSR r2    
    MOV  r5, r3
    MVN  r3, r1, LSL r2
    MOV  r6, r3
    MVN  r3, r1, LSR r2
    MOV  r7, r3

@ Force the exit of this program and return command to OS
Exit:
    MOV  r7, #0X01
    SVC  0

@ Declare the stings

.data       @ Lets the OS know it is OK to write to this area of memory. 

.balign 4

ThreeFs: .word   4095 @ FFF in hex 

.global printf

@end of code and end of file. Leave a blank line after this. 
