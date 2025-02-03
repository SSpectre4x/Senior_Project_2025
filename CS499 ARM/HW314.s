@ File: HW314.s
@ Author: R. Kevin Preston
@ Purpose: Code to check out Homework problem 3.14 in Text 
@
@ Use these commands to assemble, link and run the program
@
@  as -o HW314.o HW314.s
@  gcc -o HW314 HW314.o
@ ./HW314 ;echo $?
@ gdb --args ./HW314
@

.global main
main:

    MOV  r1, #8   @Set-up for postive mask. 

    LDR  r0, =string1
    push {r0, r1}
    BL   printf
    pop  {r0, r1}

    MOV  r1, r1, ASR #31
    push {r0, r1}
    BL   printf
    pop  {r0, r1}

    MOV  r1, #-8   @Set-up for negative mask. 
    push {r0, r1}
    BL   printf
    pop  {r0, r1}

    MOV  r1, r1, ASR #31 
    push {r0, r1}
    BL   printf   
    pop  {r0, r1}

@ Force the exit of this program and return command to OS
Exit:
    MOV  r7, #0X01
    SVC  0

@ Declare the stings

.data       @ Lets the OS know it is OK to write to this area of memory. 

.balign 4

string1: .asciz "r1 contains: %d\n"

.global printf

@end of code and end of file. Leave a blank line after this. 
