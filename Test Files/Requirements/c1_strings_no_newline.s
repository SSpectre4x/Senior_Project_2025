@ Requirement validation test file.
@ Coding/logic error #1: Strings declared in the .data section do not end with \n
@ Expected: String testStr2 at line 23 should be flagged.

.global main

main:
	ldr r0, =testStr
	bl printf
	ldr r1, =testStr2
	bl printf
	b myexit
myexit:
	mov r7, #0x01
	svc 0
	
.data

.balign 4
testStr: .asciz "hello world!"			@ No \n at end of string, should report

.balign 4
testStr2: .asciz "hello again world!\n"	@ Shouldn't be flagged

.global printf
.global scanf