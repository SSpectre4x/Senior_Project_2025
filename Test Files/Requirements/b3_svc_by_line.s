@ Requirement validation test file.
@ Additional optional output #3: SVC instructions by line number
@ Expected: SVC at lines 20 & 24.

.equ TEST, 0

.global main

main:
	b get_input
	
get_input:
	mov		r1, #TEST
	cmp		r1, #0
	bge		firstExit
	blt		secondExit

firstExit:
	mov		r7, #0x01
	svc 	0				@ <-- svc here

secondExit:
	mov 	r7, #0x01
	svc 	0				@ <-- svc here

.data