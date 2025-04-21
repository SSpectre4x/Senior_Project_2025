@ Requirement validation test file.
@ Coding/logic error #2: Missing the .data section. STR will not work.
@ Expected: Missing data section is reported.

.global main

main:
	mov r0, #1
	mov r1, #2
	add r0, r0, r1
	mov r2, r0
	mul r0, r1, r2
	b myexit
myexit:
	mov r7, #0x01
	svc 0
	
@ No data section in sight.