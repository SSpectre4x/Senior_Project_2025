@ Requirement validation test file.
@ Calculation #2: McCabe's Cyclomatic Complexity.
@ Expected: 3 paths of execution

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

next:
	subs r1, #1
	bge myexit
	sub r1, r1, #1
	b myexit

myexit:
	mov r7, #0x01
	svc 0
	
.data