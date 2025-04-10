@ Requirement validation test file.
@ Calculation #1: Halsteads primitives.
@ Expected: 5 unique operators, 9 unique operands, 9 total operators, 17 total operands

.global main

main:
	mov r0, #1
	mov r1, #2
	add r0, r0, r1
	mov r2, r0
	mul r0, r1, r2
	mov r3, r0
	b myexit

myexit:
	mov r7, #0x01
	svc 0

.data
