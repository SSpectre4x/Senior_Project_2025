@ Requirement validation test file.
@ Coding/logic error #8: Branching outside of the bounds of a user defined subroutine. 
@ Expected: Line 14 should be flagged.

.global main

main:
	ldr		r0, =testStr
	bl		printf

	bl		output
	ldr		r0, =testStr	@ Shenanigans begin here.
	ldr		r1, [r0]
	bx		r1				@ Bad. r1 holds a location outside any user subroutine.
	
output:
	ldr 	r0, =testStr
	push	{lr}
	bl 		printf
	pop		{lr}
	bx 		lr
	
myexit:
	mov		r7, #0x01
	svc		0

.data

.balign 4
testStr: .asciz "Please input a number: \n"

.global printf
.global scanf