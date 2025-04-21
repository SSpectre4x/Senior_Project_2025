@ Requirement validation test file.
@ Coding/logic error #8: Branching outside of the bounds of a user defined subroutine. 
@ Expected: Line 24 should be flagged.

.global main

main:
	ldr		r0, =testStr
	bl		printf
	mov		r1, #0
	bl		output_loop
	ldr		r0, =testStr
	ldr		r1, [r0]
	bx		r1
	
output_loop:
	add		r1, #1
	ldr 	r0, =testStr
	push	{lr}
	bl 		printf
	pop		{lr}
	cmp r1, #10
	ble		output_loop			@ Fine.  We are branching back inside the subroutine.
	beq		myexit				@ Not ok. We branch outside of the subroutine without returning.
	bx 		lr
	
myexit:
	mov		r7, #0x01
	svc		0

.data

.balign 4
testStr: .asciz "The loop has run %d times."

.global printf
.global scanf