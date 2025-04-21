@ Requirement validation test file.
@ Calculation #1: Halsteads primitives.
@ Expected: 5 unique operators, 10 unique operands, 9 total operators, 18 total operands
@ Unique operators: mov, add, mul, b, svc (used 9 times total)
@ Unique operands: r0, #1, r1, #2, r2, r3, myexit, r7, #0x01, 0 (used 18 times total)

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
