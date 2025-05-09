@ Requirement validation test file.
@ Calculation #7: Lines of ARM Assembly code with no comments
@ Expected: 9 lines of code without comments.

.global main

main:
	mov r0, #1
	mov r1, #2
	add r0, r0, r1
	mov r2, r0				@ Code line with comment should still count
	mul r0, r1, r2			// Another code line with a comment
	b myexit				/* This should count too. */
myexit:
	mov r7, #0x01
	svc 0
	
.data