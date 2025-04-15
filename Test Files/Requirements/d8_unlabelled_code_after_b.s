@ Requirement validation test file.
@ Design/coding error #8: Executable code after B instruction with no label.
@ (Isolated code with no way to execute). 
@ Expected: Line 17 is flagged.

.global main

main:
	mov r0, #1
	mov r1, #2
	add r0, r0, r1
	mov r2, r0
	mul r0, r1, r2
	cmp r0, #6
	beq myexit
	b next
	add r1, r1, #1 		@ Not ok. Will never be executed.
	
next:
	sub r1, #1
	b myexit
	
myexit:
	mov r7, #0x01
	svc 0
	
.data