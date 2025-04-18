@ Requirement validation test file.
@ Additional optional output #2: Calls to subroutines (BL) by line number
@ Expected: 4 calls to subroutines.

.equ READERROR, 0

.global main

main:
	ldr		r0, =testStr
	bl		printf					@ subroutine call

get_input:
	ldr		r0, =intInputPattern
	ldr 	r1, =intInput
	bl		scanf					@ subroutine call
	cmp		r0, #READERROR
	beq		readerror
	ldr		r1, =intInput
	ldr		r1, [r1]
	bl		do_thing				@ subroutine call
	b		myexit

do_thing:
	mov		r1, #1
	bx		lr
	
myexit:
	mov		r7, #0x01
	svc		0

readerror:
	ldr		r0, =strInputPattern
	ldr		r1, =strInputError
	bl		scanf					@ subroutine call
	b		main

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