@ Requirement validation test file.
@ Coding/logic error #4: Making a call to printf or scanf then trying to
@ use the registers that get wiped out (r0-r3 & r12/ip).
@ Expected: Lines 11, 12 (printf), 18, & 19 (scanf) should be flagged.

.global main

main:
	ldr r0, =testStr
	bl printf
	mov r1, r0			@ Not ok, r0 was just wiped out by printf.
	add r2, r3, #1 		@ Also not ok. Incrementing a garbage value.
	mov r0, #1			@ Ok. r0 can be a destination, but not a source.
	add r0, r0, #1 		@ r0 now has a valid value so this is fine.
	ldr r0, =intInputPattern
	ldr r1, =userInput
	bl scanf
	mov r1, r0			@ Not ok, r0 was just wiped out by scanf.
	add r2, r12, #1 		@ Also not ok. Incrementing a garbage value.
	mov r0, #1			@ This is fine. r0 can be a destination, but not a source.
	add r0, r0, #1 		@ r0 now has a valid value so this is fine.
	b myexit
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