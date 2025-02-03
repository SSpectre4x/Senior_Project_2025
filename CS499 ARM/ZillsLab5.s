@ Filename: ZillsLab5.s
@ Author:   Joshua Zills
@ Email:    jkz0001@uah.edu
@ CS309-01  Fall2023
@ Use these commands to assemble, link, run and debug this program:
@    as -o ZillsLab5.o ZillsLab5.s
@    gcc -o ZillsLab5 ZillsLab5.o
@    ./ZillsLab5 ;echo $?
@    gdb --args ./ZillsLab5 

.equ READERROR, 0 @Used to check for scanf read error. 

.global main @ Have to use main because of C library uses. 


main:

	@ Initialize
	ldr r9, = cutCounter
	ldr r10, = sumHolder
	ldr r6, = boardOne
	ldr r7, = boardTwo
	ldr r8, =boardThree
	
	b scoreBoard
	
@@@@@@@@@@	
scoreBoard:	@ Outputs showing all current tablulations
@@@@@@@@@@
	ldr r0, = cutItUpCounterStr		
	ldr r1, [r9]
	bl printf
	
	ldr r0, = sumCut			
	ldr r1, [r10]
	bl printf
	
	ldr r0, = strBoardOne
	ldr r1, [r6]
	bl printf
	
	ldr r0, = strBoardTwo
	ldr r1, [r7]
	bl printf
	
	ldr r0, = strBoardThree
	ldr r1, [r8]
	bl printf
	
	b lengthCheckOne
	
lengthCheckOne:					@ Part 1 to check if 
	ldr r2, [r6]				@ all boards are below 6
	cmp r2, #6
	blt lengthCheckTwo
	
	b lengthInput				@ Falls through to user input

lengthCheckTwo:					@ Part 2 to check if
	ldr r2, [r7]				@ all boards are below 6
	cmp r2, #6
	blt lengthCheckThree
	
	b lengthInput				

lengthCheckThree:				
								@ Part 3 to check if
	ldr r2, [r8]				@ all boards are below 6
	cmp r2, #6					@ Moves on to Final Output
	blt wasteCalc
	
@@@@@@@@@@@@@@@	
lengthInput:
@@@@@@@@@@@@@@@

	ldr r0, = strInputPrompt	@ Prompt for user input
	bl printf
	
	ldr r0, =numInputPattern
	ldr r1, =intInput			@ Get an integer
	
	bl scanf
	cmp r0, #READERROR			@ error checker for chars
	beq readerror
	ldr r1, = intInput			
	ldr r1, [r1]
	cmp r1, #6					@ Checks that number is
	blt lengthInput				@ inbetween 6 & 144
	cmp r1, #144
	bgt lengthInput
	
	b boardComparisonChain
	
@@@@@@@@@@@@@@@	
boardComparisonChain:
@@@@@@@@@@@@@@@
	ldr r2, [r6]				@ load board1 into r2
	cmp r1, r2					@ compare if user input > board1
	bgt checkTwo
	
	ldr r3, [r9]				@ loads value of cuts counter into r3
	add r3, r3, #1				@ increases the counter by 1
	str r3, [r9]				@ puts value of r3 back into r9
	
	ldr r4, [r10]				@ loads length sum into r4
	add r4, r4, r1	 			@ increases the length sum by input value
	str r4, [r10]				@ puts value of addition back into sumHolder
	
	sub r2, r2, r1				@ subtracts the input from value of board1
	str r2, [r6]				@ puts the value back into board1 location
	
	b scoreBoard
	
@@@@@@@@@@@@@@@	
checkTwo:
@@@@@@@@@@@@@@@
	ldr r2, [r7]				@ load board2 into r2
	cmp r1, r2					@ compare if user input > board1
	bgt checkThree
	
	ldr r3, [r9]				@ loads value of counter into r3
	add r3, r3, #1				@ increases the counter by 1
	str r3, [r9]				@ puts value of r3 back into r9
	
	ldr r4, [r10]
	add r4, r4, r1	 			@ increases the length sum by input value
	str r4, [r10]				@ puts value of addition back into sumHolder
	
	sub r2, r2, r1				@ subtracts the input from value of board2
	str r2, [r7]				@ puts the value back into board2 location
	
	b scoreBoard

@@@@@@@@@@@@@@@	
checkThree:
@@@@@@@@@@@@@@@
	ldr r2, [r8]				@ load board3 into r2
	cmp r1, r2					@ compare if user input > board3
	bgt kickBack				
	
	ldr r3, [r9]				@ loads value of counter into r3
	add r3, r3, #1				@ increases the counter by 1
	str r3, [r9]				@ puts value of r3 back into r9
	
	ldr r4, [r10]
	add r4, r4, r1	 			@ increases the length sum by input value
	str r4, [r10]				@ puts value of addition back into sumHolder
	
	sub r2, r2, r1				@ subtracts the input from value of board2
	str r2, [r8]				@ puts the value back into board2 location
	
	b scoreBoard
	
@@@@@@@@@@@@@@@	
kickBack:	@ function if boards are all short, but not all shorter than 6
@@@@@@@@@@@@@@@
	ldr r0, = tryAgain
	bl printf
	
	b lengthInput

@@@@@@@@@@@@@@@
wasteCalc:	@ Wind up to the end. This is driving me crazy.
@@@@@@@@@@@@@@@	
	ldr r5, = wasteCounter		@ total # of inches in 3 boards, 432
	ldr r3, = reset
	ldr r4, = reset				@ no longer need these to persist,
	ldr r9, = reset				@ so intializing to zero if I need them
	
	ldr r9, [r5]				@ TRYING TO SUBTRACT 2 NUMBERS
	sub r9, r9, r10	 			@ increases the length sum by input value
	str r9, [r4]				@ puts value of SUB into r4
	
								@ These values are w/o lines 171-173
								@ [r5] = 432
								@ [r10] = 420 w/ 3 inputs of 140
								
								@ wasteMsg should be 12, but it's 
								@ -135620
	
	ldr r1, [r4]				@ change this address and see what happens
	ldr r0, = wasteMsg
	bl printf
	
	b myexit



@***********
readerror:
@***********
@ Got a read error from the scanf routine. Clear out the input buffer then
@ branch back for the user to enter a value.
@ Since an invalid entry was made, we now have to clear out the input buffer by
@ reading with this format %[^\n], which will read the buffer until the user
@ presses the CR.

ldr r0, =strInputPattern
ldr r1, =strInputError   @ Put the address into r1 for read.
bl scanf                 @ Scan the keyboard.
@  Not going to do anything with the input. This just cleans up the input buffer.
@  The input buffer should now be clear so get another input.

b lengthInput
	
@*******************
myexit:
@*******************
@ End of my code. Force the exit and return control to the OS

mov r7, #0x01 @ SVC call to exit
svc 0         @ Make the system call.
	
.data
.balign 4
cutItUpCounterStr: .asciz "**********\nCut-It-Up Saw\n**********\nBoards cut so far:%d\n" 

.balign 4
sumCut: .asciz "Line length of boards cut so far: %d inches\n"

.balign 4
strBoardOne: .asciz "Current Board Lengths:\nOne:\t%d inches\n"

.balign 4
strBoardTwo: .asciz "Two:\t%d inches\n"

.balign 4
strBoardThree: .asciz "Three:\t%d inches\n"

.balign 4
strInputPrompt: .asciz "Enter the length of board to cut in inches(at least 6 and no more than 144: \n"

.balign 4
tryAgain: .asciz "Not enough inventory to perform cut. Enter new smaller length.\n"

.balign 4
wasteMsg: .asciz "Inventory levels have dropped below minimum levels and will now terminate.\nWaste is %d inches.\n"



@ Format pattern for scanf call.

.balign 4
numInputPattern: .asciz "%d"  @ integer format for read.

.balign 4
strInputPattern: .asciz "%[^\n]" @ Used to clear the input buffer for invalid input.

.balign 4
strInputError: .skip 100*4  @ Used to clear the input buffer for invalid input.

.balign 4
intInput: .word 0   	@ Location used to store the user input.

.balign 4
sumHolder: .word 0   	@ Location used to store the sums.

.balign 4
boardOne: .word 144		@ Location used to store the board lengths.

.balign 4
boardTwo: .word 144

.balign 4
boardThree: .word 144

.balign 4
cutCounter: .word 0		@ Location used to hold # of runs

.balign 4
wasteCounter: .word 432	@ Location used to hold total inches

.balign 4
reset: .word 0



@ Let the assembler know these are the C library functions. 


.global printf
@  To use printf:
@     r0 - Contains the starting address of the string to be printed. The string
@          must conform to the C coding standards.
@     r1 - If the string contains an output parameter i.e., %d, %c, etc. register
@          r1 must contain the value to be printed. 
@ When the call returns registers: r0, r1, r2, r3 and r12 are changed. 


.global scanf

