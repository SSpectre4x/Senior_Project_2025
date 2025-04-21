@ Requirement validation test file.
@ Design/coding error #1: Source register not set prior to reference.
@ Must take in account registers that get set/changes from calls to
@ printf and scanf. 
@ Expected: Lines 10, & 16 are flagged.

.global main

main:
	add r0, r1, #1		@ Not ok, r1 has not yet been set.
	mov r1, #1
	add r2, r1, #1		@ Ok, r1 has now been set.
	
	ldr r0, =testStr
	bl printf
	mov r1, r0			@ Not ok, r0 was just wiped out by printf.
	mov r0, #1			@ Ok. r0 can be a destination, but not a source.
	add r3, r0, #1 		@ r0 now has a valid value so this is fine.
	b myexit
myexit:
	mov r7, #0x01
	svc 0

.data

.balign 4
testStr: .asciz "hello world!\n"

.global printf
.global scanf