@ Requirement validation test file.
@ Design/coding error #3: Pop without a corresponding Push.
@ Expected: Line 22 is flagged.

.equ READERROR, 0

.global main

main:
prompt:
	ldr		r0, =testStr
	bl		printf

get_input:
	ldr		r0, =intInputPattern
	ldr 	r1, =intInput
	bl		scanf
	cmp		r0, #READERROR
	beq		readerror
	ldr		r1, =intInput
	ldr		r1, [r1]
	pop		{r1}					@ Not ok. No matching push precedes.
	bl		output_result
	b		myexit

output_result:
	push 	{r1, lr}				@ Matches pop.
	ldr 	r0, =testStr2
	bl 		printf
	pop 	{r1, lr}				@ OK.
	bx 		lr
	
myexit:
	mov		r7, #0x01
	svc		0

readerror:
	ldr		r0, =strInputPattern
	ldr		r1, =strInputError
	bl		scanf
	b		prompt

.data

.balign 4
testStr: .asciz "Please input a number: \n"

.balign 4
testStr2: .asciz "The number you input was: %d \n"

.balign 4
intInputPattern: .asciz " %c"

.balign 4
intInput: .word 0

.balign 4
strInputPattern: .asciz "%[^\n]"

.balign 4
strInputError: .skip 100*4

.global printf
.global scanf