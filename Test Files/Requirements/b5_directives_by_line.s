@ Requirement validation test file.
@ Additional optional output #5: Assembler directives by line number.
@ Expected: see comments.

.equ BLUE, 42		@ Directive #1

.text				@ Directive #2
.global main		@ Directive #3

main:
	mov r0, #1
	mov r1, #2
	add r0, r0, r1
	mov r2, r0
	mul r0, r1, r2
	b myexit
myexit:
	mov r7, #0x01
	svc 0
	
.data				@ Directive #4

.balign 4			@ Directive #5
testStr: .asciz "hello world!\n"