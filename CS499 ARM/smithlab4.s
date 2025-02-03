@ Filename: smith4.s
@ Author:   Paige Smith
@ Email:    pns0006@uah.edu
@ Class:    CS 309-01 2023
@ Lab 4 Description: Take a number 1 to 100 from the user and give all even and odd
@    numbers starting from 2 and ending with your number. Give the sum of all the 
@    even numbers and a sum of all the odd numbers.
@ 
@ Use these commands to assemble, link, run, and debug this program:
@	as -o smithlab4.o smithlab4.s
@	gcc -o smithlab4 smithlab4.o
@	./smithlab4 ;echo $?
@	gdb --args ./smithlab4
@
@
@

.global main        

main:

@----------------
prompt:
@----------------
@ Ask the user to enter a number between 1 to 100
	ldr r0, =strInputPrompt        @put address of prompt string into r0
	bl printf                      @call printf to display prompt
	
@---------
get_input:
@---------
@ Take the input from the user and store in intInput

	ldr r0, =numInputPattern       @setup to read a number input
	ldr r1, =intInput              @load r1 with address to store input
	bl scanf                 	   @scan keyboard for input
	ldr r1, =intInput              @reloard r1 because wiped out
	ldr r1, [r1]                   @read contents of intInput and store
								   @ to print 
	b compare100				   @check input parameters
	
	
@----------
compare100:
@----------
	cmp r1, #100	               @check for a read error
	bgt myexit	                   @if read error found, exit
	b compare2					   @check next parameter
	
@--------
compare2:
@--------

	cmp r1, #2					   @check for a read error
	blt myexit					   @if read error found, exit
	b output					   @input is valid move to loops

@------						   
output:
@------								   
@reprint user input back to them, move to loops
	ldr r0, =strOutputPrompt       @load string into r0
	ldr r1, =intInput
	ldr r1, [r1]
	bl printf                      @print string with r1 info out
	ldr r1, =intInput
	ldr r1, [r1]				   @reload input because gets wiped out
	b evenprompt

@----------
evenprompt:
@----------
@allocate r4 as current even number
@allocate r5 as even numbers sum

	ldr r0, =strEvenstart		   @load r0 with prompt for even loop
	bl printf					   @print out loop start sentence
	mov r4, #2					   @move constant 2 into r4
	mov r5, #0					   @move constant 0 into r5
	b evenloop					   @branch to start even loop

@--------
evenloop:
@--------
	mov r1, r4					   @move r4 current even into r1
	ldr r0, =numInputPattern	   @sets to read out an integer
	ldr r0, =outputNumber  		   @reads out an integer string
	bl printf					   @print the current number
	ldr r1, =intInput			   @reload r1 from printf wipeout
	ldr r1, [r1]				   @give ldr the value at r1
	ADD r5, r4, r5				   @add current number to sum
	ADD r4, r4, #2				   @increment current to next even
	CMP r4, r1					   @compare the new current to input
	BLE evenloop				   @if current< input loop again
	BEQ evensum					   @if equal print sum
	
@---------
oddprompt:
@---------
@allocate r7 as current odd number
@allocate r8 as odd numbers sum
	ldr r0, =strOddstart           @load r0 with prompt for odd loop
	ldr r1, =intInput			   @load r1 with the input address
	ldr r1, [r1]				   @load r1 with the number at input address
	bl printf					   @print out loop start sentence
	mov r7, #1					   @load r7 with starting odd value
	mov r8, #0					   @load r8 with odd sum
	b oddloop					   @branch to start odd loop
	
@-------
oddloop:
@-------
	mov r1, r7					   @move current number into r1
	ldr r0, =numInputPattern	   @format to print out integer
	ldr r0, =outputNumber		   @load string that prints number
	bl printf					   @print out current number
	ldr r1, =intInput 			   @reload r1 with address input
	ldr r1, [r1]				   @reload r1 with value of input
	ADD r8, r7, r8				   @add current number to sum
	ADD r7, r7, #2				   @increment to next odd number
	CMP r7, r1					   @compare current number to input
	BLT oddloop					   @if number <input loop again
	BEQ oddsum					   @if equal print the sum			

@-------
evensum:
@-------
	ldr r0, =strEvensum		       @load r0 with int pattern to print
	mov r1, r5		               @move r5 to r1 with the final sum of evens
	bl printf					   @print out the sum
	b oddloop					   @branch to the odd loop
	
@------
oddsum:
@------
	ldr r0, =strOddsum			   @load r0 with string to print
	mov r1, r8				   	   @move r8 to r1 with the final sum of odds
	bl printf					   @print out the sum
	b myexit					   @all tasks done, exit code




	

@------
myexit:
@------
@ End of my code, force the exit and return control to OS
	mov r7, #0x01     @make SVC call to exit
	svc 0             @make system call


@---------------------------------------------
.data
@ all read/write goes here and under

@declared statement/prompt strings
.balign 4
strInputPrompt: .asciz "Input a number from 1 to 100: \n"

.balign 4
strOutputPrompt: .asciz "You entered %d \n"

.balign 4
outputNumber: .asciz "%d \n"

.balign 4
strEvenstart: .asciz "The even numbers from 2 to %d are: \n"

.balign 4
strOddstart: .asciz "The odd numbers from 1 to %d are: \n"

.balign 4
strEvensum: .asciz "The sum of the even numbers are: %d \n"

.balign 4
strOddsum: .asciz "The sum of the odd numbers are: %d \n"



@scanf format patterns
.balign 4
strError: .asciz "Invalid input entered or out of range (1-100)!\n"

.balign 4
strInputPattern: .asciz "%[^\n]"     @clears input buffer for invalid input

.balign 4
strInputError: .skip 100*4           @clear the input buffer for invalid input

.balign 4 
numInputPattern: .asciz " %d"         @format to read in an integer


@declared input variables storage
.balign 4
intInput: .word 0                    @location to store user input num



@------------------------------
@C library functions being used

.global scanf

.global printf


@End of code and file, leave a blank line after this.
