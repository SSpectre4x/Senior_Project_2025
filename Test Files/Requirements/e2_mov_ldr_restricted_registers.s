@ Requirement validation test file.
@ #2: MOV or LDR into restricted registers : SP (R13), LR (R14) and PC (R15). 
@ Expected: Lines 15, 16, 17, & 18 are flagged.

.global main

main:
	mov r0, #1
	mov r1, #2
	add r0, r0, r1
	mov r2, r0
	ldr r4, =testList
	mul r0, r1, r2
	cmp r0, #6
	mov pc, #68			@ restricted register
	ldr pc, =testList	@ restricted register
	mov lr, #68			@ restricted register
	mov sp, #68			@ restricted register
	b myexit
myexit:
	mov r7, #0x01
	svc 0
	
.data

.balign 4
testList: .word 0 1 2 3 4