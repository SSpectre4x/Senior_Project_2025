@ Requirement validation test file.
@ Additional optional output #3: SVC instructions by line number
@ Expected: SVC at lines 21 & 27.

.equ TEST, 0

.global main

main:
	ldr		r0, =testStr
	bl		printf

get_input:
	mov		r1, #TEST
	cmp		r1, #0
	bge		firstExit
	blt		secondExit

firstExit:
	mov		r7, #0x01
	svc 	0				@ <-- svc here

secondExit:
	ldr 	r0, =exitStr
	bl 		printf
	mov 	r7, #0x01
	svc 	0				@ <-- svc here

.data