@ Requirement validation test file.
@ Coding/logic error #9: User input string is defined as “%d” or “%c”
@ instead of “ %d” or “ %c”. Space between the “ and % which does not
@ read any non-printable characters.
@ Expected: Lines 36 and 42 should be flagged.

.global main

main:
	ldr r0, =strInputPattern
	ldr r1, =userInput
	bl scanf
	ldr r0, =badCharInputPattern
	ldr r1, =userInput
	bl scanf
	ldr r0, =goodCharInputPattern
	ldr r1, =userInput
	bl scanf
	ldr r0, =badDecimalInputPattern
	ldr r1, =userInput
	bl scanf
	ldr r0, =goodDecimalInputPattern
	ldr r1, =userInput
	bl scanf
	b myexit
myexit:
	mov r7, #0x01
	svc 0
	
.data

.balign 4
strInputPattern: .asciz "%[^\n]"

.balign 4
badCharInputPattern: .asciz "%c"	@ Bad. No space before format specifier.

.balign 4
goodCharInputPattern: .asciz " %c"

.balign 4
badDecimalInputPattern: .asciz "%d"	@ Bad. No space before format specifier.

.balign 4
goodDecimalInputPattern: .asciz " %d"

.balign 4
userInput: .word 0

.global printf
.global scanf