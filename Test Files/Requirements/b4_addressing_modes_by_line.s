@ Requirement validation test file.
@ Additional optional output #4: All the different addressing modes in
@ the code and the lines they occur in. The output should be organized
@ by addressing mode with the corresponding line numbers where the
@ addressing mode is used. 
@ Expected: see comments.

.global main

main:
	ldr r0, =testInt			@ PC relative
	ldr r0, [r0]				@ Register indirect
	ldr r0, =testList			@ PC relative
	mov r1, #1					@ Literal
	ldr r2, [r0, r1 LSL #2]		@ Register indirect w/ offset
	ldr r2, [r0], r1 LSL #2		@ Autoindexing post-indexed
	ldr r2, [r0, r1 LSL #2]!	@ Autoindexing pre-indexed
	b myexit
	
myexit:
	mov r7, #0x01				@ Literal
	svc 0

.data

.balign 4
testInt: .word 0

.balign 4
testList: .word 1 2 3 4 5