@ Requirement validation test file.
@ Coding/logic error #7: Missing correct exit from main. No SVC
@ instruction prior to the .data section. 
@ Expected: Lack of SVC instruction before .data should be flagged.

.global main

main:
	ldr		r0, =testStr
	bl		printf

.data

	svc		0	@ Only found after data.

.balign 4
testStr: .asciz "Please input a number: \n"

.global printf
.global scanf