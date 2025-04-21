@ Requirement validation test file.
@ Coding/logic error #6: Making a BL call to a user defined subroutine and
@ not using either the BX LR or MOV PC, LR to make the return.
@ Expected: Line 29 should be flagged.

.global main

main:
	bl		output
	bl		output2
	b		myexit
	
output:
	push 	{r1}
	ldr 	r0, =testStr
	push	{lr}
	bl 		printf
	pop		{lr}			
	pop 	{r1}
	bx 		lr				@ ok

output2:
	push 	{r1}
	ldr 	r0, =testStr
	push	{lr}
	bl 		printf
	pop		{lr}
	pop 	{r1}
	b		main			@ not ok
	
myexit:
	mov		r7, #0x01
	svc		0

.data

.balign 4
testStr: .asciz "Please input a number: \n"

.global printf
.global scanf