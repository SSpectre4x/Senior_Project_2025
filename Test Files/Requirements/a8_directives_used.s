@ Requirement validation test file.
@ Calculation #8: Number of Assembly directives used
@ Expected: 5 directives

.equ BLUE, 42

.text
.global main

main:
	mov r0, #1
	mov r1, #2
	add r0, r0, r1
	mov r2, r0
	mul r0, r1, r2
	cmp r0, #6
	b myexit
myexit:
	mov r7, #0x01
	svc 0
	
.data

.balign 4
testStr: .asciz "hello world!/n"