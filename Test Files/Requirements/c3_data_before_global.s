@ Requirement validation test file.
@ Coding/logic error #3: The .data section is before the .global entry point.
@ The ARM Code can still work but the debugger will not be able to work. 
@ Expected: It is reported that the data section was found before global.

.data								@ Not ok. Appears before .global

.balign 4
testStr: .asciz "hello world!\n"

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