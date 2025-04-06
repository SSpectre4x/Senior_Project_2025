@ Requirement validation test file.
@ Design/coding error #7: Update flag instruction used with no corresponding
@ condition code instruction
@ Expected: Line 20 is flagged.

.global main

main:
	mov r0, #1
	mov r1, #2
	add r0, r0, r1
	mov r2, r0
	mul r0, r1, r2
	cmp r0, #6			@ Ok. is used by beq statement.
	add r1, r1, #1
	beq myexit
	b next
	
next:
	subs r1, #1			@ Bad. No condition code instruction uses this.
	b myexit
	
myexit:
	mov r7, #0x01
	svc 0
	
.data