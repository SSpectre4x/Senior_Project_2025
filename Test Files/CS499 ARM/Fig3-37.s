@ Filename: Fig3-37.s
@ Author: Kevin Preston
@ Purpose: 
@ 
@  as -o Fig3-37.o Fig3-37.s 
@  gcc -o Fig3-37 Fig3-37.o
@  ./Fig3-37 ;echo $?

.equ Len, 8
.global main
main: 
    LDR r0, =A-4
    LDR r1, =B-4
    LDR r2, =C-4

    MOV r5, #Len
Loop:
    LDR  r3, [r0, #4]!    
    LDR  r4, [r1, #4]!
    ADD  r3, r3, r4
    STR  r3, [r2, #4]!
    SUBS r5, r5, #1
    BNE  Loop

@Exit the code and return to OS
Exit:
    MOV  r7, #0x01
    SVC  0
@ Declare the data

.data
.balign 4   @ Force a word boundry.
A: .word  1,2,3,4,5,6,7,8
B: .word  2,5,4,6,7,2,4,1
C: .word  0,0,0,0,0,0,0,0


@ end of file.
