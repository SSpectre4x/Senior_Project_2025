@ Requirement validation test file.
@ Additional optional output #1: Registers used by line number
@ Expected: see comments.

.global main

main:
	mov r0, #1				@ r0
	mov r1, #2				@ r1
	add r0, r0, r1			@ r0, r1
	mov r2, r0				@ r2, r0
	mul r0, r1, r2			@ r0, r1, r2
	mov r3, r0				@ r3, r0
	b myexit
myexit:
	mov r7, #0x01			@ r7
	svc 0

.data