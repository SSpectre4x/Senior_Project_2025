@ Requirement validation test file.
@ #1: Use of unexpected instructions: SWI, LDM or LTM. 
@ Expected: Lines 10, 14, 17 are flagged.

.global main

main:
	mov r0, #1
	mov r1, #2
	swi	0						@ unexpected instr
	add r0, r0, r1
	mov r2, r0
	ldr r4, =testList
	ldm r4, {r0, r1, r2, r3}	@ unexpected instr
	mul r0, r1, r2
	cmp r0, #6
	ltm							@ unexpected instr (not sure this is even real)
	b myexit
myexit:
	mov r7, #0x01
	svc 0
	
.data

.balign 4
testList: .word 0 1 2 3 4