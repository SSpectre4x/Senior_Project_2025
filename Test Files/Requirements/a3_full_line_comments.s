@ Requirement validation test file.
@ Calculation #3: Number of full line comments
@ Expected: 8 full line comments

.global main

main:
	@ Another full line comment but there's whitespace before it.

// Also a full line comment.

/* Another valid style of comment. */

/* How about this one?
   This should count as two full lines since it's a block comment. */

	mov r0, #1		@ Not a full-line comment
	mov r1, #2		/* Also not a valid full line comment */
	add r0, r0, r1	// Again, not a full line comment.
	mov r2, r0
	mul r0, r1, r2
	b myexit

myexit: 
	mov r7, #0x01 
	svc 0
	
.data