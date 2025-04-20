@ Requirement validation test file.
@ Calculation #5: Lines of ARM Assembly code (Total)
@ Expected: 13 lines of code total.

.global main

main:
	mov r0, #1
	mov r1, #2
	add r0, r0, r1
	mov r2, r0		@ Code line with comment should still count
	mul r0, r1, r2
	cmp r0, #6
	b myexit
@ next blank line has tab character
	
myexit:
	mov r7, #0x01
	svc 0
@ next blank line has spaces
    
.data
