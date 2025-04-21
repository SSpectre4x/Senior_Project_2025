@ Requirement validation test file.
@ Coding/logic error #5: Making a BL call to a user defined subroutine then
@ making another BL call in the call routine without saving the LR first. 
@ Expected: Line 24 should be flagged.

.global main

main:
	ldr		r0, =testStr
	bl		printf
	bl		output
	bl		output2
	b		myexit

output:
	ldr 	r0, =testStr
	push	{lr}
	bl 		printf			@ lr was pushed, no error.
	pop		{lr}			@ push without pop will catch if this is missed.
	bx 		lr

output2:
	ldr 	r0, =testStr
	bl 		printf			@ lr was not saved beforehand, bx won't return correctly.
	bx 		lr
	
myexit:
	mov		r7, #0x01
	svc		0

.data

.balign 4
testStr: .asciz "Please input a number: \n"

.global printf
.global scanf