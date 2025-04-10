@ Requirement validation test file.
@ Design/coding error #1: Source register not set prior to reference.
@ Must take in account registers that get set/changes from calls to
@ printf and scanf. 
@ Expected: Lines 10, 16, & 17 are flagged.

.global main

main:
	add r0, r1, #1		@ Not ok, r1 has not yet been set.
	mov r1, #1
	add r0, r1, #1		@ Ok, r1 has now been set.
	
	ldr r0, =testStr
	bl printf
	mov r1, r0			@ Not ok, r0 was just wiped out by printf.
	add r0, r0, #1 		@ Also not ok. Incrementing a garbage value.
	mov r0, #1			@ Ok. r0 can be a destination, but not a source.
	add r0, r0, #1 		@ r0 now has a valid value so this is fine.

myexit:
	mov r7, #0x01
	svc 0

.data

.balign 4
testStr: .asciz "hello world!\n"

.balign 4
intInputPattern: .asciz " %c"

.balign 4
userInput: .word 0

.global printf
.global scanf