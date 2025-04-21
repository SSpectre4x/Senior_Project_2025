@ Requirement validation test file.
@ Calculation #4: Number of blank lines
@ Expected: 5 blank lines

.global main

main:
	mov r0, #1
	mov r1, #2
	add r0, r0, r1
	mov r2, r0
	mul r0, r1, r2
	b myexit
							@ next blank line has tab character
	
myexit:
	mov r7, #0x01
	svc 0
							@ next blank line has spaces
    
.data
