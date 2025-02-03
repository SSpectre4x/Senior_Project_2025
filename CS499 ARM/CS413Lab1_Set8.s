@ Filename: CS413Lab1_Set8.s
@ Author:   Kevin Preston

@ Compilation:
@ as -o CS413Lab1_Set8.o CS413Lab1_Set8.s
@ gcc -o CS413Lab1_Set8 CS413Lab1_Set8.o
@ ./CS413Lab1_Set8 ; echo $?
@ gdb --args ./CS413Lab1_Set8

@ ****************************************************
@ The = (equal sign) is used in the ARM Assembler to get the address of a
@ label declared in the .data section. This takes the place of the ADR
@ instruction used in the textbook. 
@ ****************************************************

.equ READERROR, 0 @Used to check for scanf read error. 

.global main @ Have to use main because of C library uses. 

main:

@*******************
prompt:
@*******************

@ Ask the user to enter a number.
 
   ldr r0, =strInputPrompt @ Put the address of my string into the first parameter
   bl  printf              @ Call the C printf to display input prompt. 

@*******************
get_input:
@*******************

@ Set up r0 with the address of input pattern
@ scanf puts the input value at the address stored in r1. We are going
@ to use the address for our declared variable in the data section - intInput. 
@ After the call to scanf the input is at the address pointed to by r1 which in this
@ case will be intInput. 

   ldr r0, =numInputPattern @ Setup to read in one number.
   ldr r1, =intInput        @ load r1 with the address of where the
                            @ input value will be stored. 
   bl  scanf                @ scan the keyboard.
   cmp r0, #READERROR       @ Check for a read error.
   beq readerror            @ If there was a read error go handle it. 
   ldr r1, =intInput        @ Have to reload r1 because it gets wipped out. 
   ldr r1, [r1]             @ Read the contents of intInput and store in r1 so that

@ r6 Positive, negative or zero input from user. 
   mov r6, r1


set_up:
	mov r5, #0  @ Counter.
@ r7, current value of arrayOne.
@ r8, current value of arrayTwo.
@ r9, current value of arrayThree.

@ Starting addresses for each of the three arrays. 
@ Subtract 4 because of using Pre-indexed addressing mode. 

	ldr r10, =arrayOne - 4
	ldr r11, =arrayTwo - 4
	ldr r12, =arrayThree -4
	
buildArrayThree:
	cmp r5, #10
	beq printarrays
	ldr r7, [r10, #4]!
	ldr r8, [r11, #4]!
	add r9, r7, r8
	str r9, [r12, #4]!
	add r5, r5, #1

	b buildArrayThree

printarrays:
@Pass starting address of array - 4 via r4
@Pass to print positive, negative or zero value in r6.
      
        ldr r0, =strOutputArrayOne
        bl  printf
	ldr r4, =arrayOne - 4
	bl printArrays

        ldr r0, =strOutputArrayTwo
        bl  printf
	ldr r4, =arrayTwo - 4
	bl printArrays

        ldr r0, =strOutputArrayThree
        bl printf
	ldr r4, =arrayThree -4
	bl printArrays

@ Now print array three again but only the user selected ones.

        ldr r0, =strUserSelectedThree
        bl printf

@What did the user enter?
@        ldr r0, =strResult
@        mov r1, r6
@        bl printf

	ldr r4, =arrayThree -4
        mov r5, #0 @loop index

Userloop:
        cmp r5, #10
        beq myexit

        ldr r1, [r4, #4]!
	ldr r0, =strResult
        @ Got everything now figure out if it prints.
	add r5, r5, #1
 
        cmp   r6, #0
        beq pntzero
        blt pntnegative
pntpositive:
        cmp r1, #0
        ble Userloop
        bl  printf
        b Userloop
pntzero: @works!!
        cmp r1, #0
        bleq printf
        b Userloop
pntnegative:
        cmp r1, #0
        bge Userloop
        bl printf
        b Userloop

@********************
printArrays:
@********************
@ Subroutine to print an array of 10 elements 0-9. 
@ Starting address of array is in r4.
@
	mov r5, #0 @ Loop counter
	push {lr}  @ Save return address on stack. 

loop:
	cmp r5, #10
	beq branch
	
	ldr r1, [r4, #4]!
	ldr r0, =strResult
        bl  printf

	add r5, r5, #1
	b loop

branch:
	pop {pc} @ Finished printing return to calling routine
 

@***********
readerror:
@***********
@ Got a read error from the scanf routine. Clear out the input buffer and ask
@ for the user to enter a value. 
@ An invalid entry was made we now have to clear out the input buffer by
@ reading with this format %[^\n] which will read the buffer until the user 
@ presses the CR. 

   ldr r0,=strInputPattern
   ldr r1, =strInputError   @ Put address into r1 for read.
   bl scanf                 @ scan the keyboard.
@  Not going to do anything with the input. This just cleans up the input buffer.  
@  The input buffer should now be clear so get another input.

   b prompt


@*******************
myexit:
@*******************
@ End of my code. Force the exit and return control to OS

   mov r7, #0x01 @SVC call to exit
   svc 0         @Make the system call. 

.data

@ Declare the strings and data needed

.balign 4
strNewLine: .asciz "\n\n"

.balign 4
strInputPrompt: .asciz "Enter a positive or negative number, or zero: \n" @First text to display.

.balign 4
strOutputArrayOne: .asciz "Array One : \n" @ Example for printing a number.

.balign 4
strOutputArrayTwo: .asciz "Array Two: \n" @ Example for printing a number.

.balign 4
strUserSelectedThree: .asciz "User selected only print of array 3: \n" 
.balign 4
strOutputArrayThree: .asciz "Array Three: \n" @ Example for printing a number.

.balign 4
strResult: .asciz "  %d \n" @ Example for printing a number.

.balign 4
strOutputNum_new: .asciz "The character value is: %c \n" @ Example for printing a character.

@*******************
@ Arrays:

.balign 4
arrayOne: .word -4, -3, -2, -1, 0, 1, 2, 3, 4, -9 @ These numbers can be changed.

.balign 4
arrayTwo: .word 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 @ These numbers can be changed.

.balign 4
arrayThree: .skip 4*20 @ An array with enough room for 10 integers.

@*******************

@ Format pattern for scanf call.

.balign 4
numInputPattern: .asciz "%d"  @ integer format for read. 

.balign 4
strInputPattern: .asciz "%[^\n]" @ Used to clear the input buffer for invalid input. 

.balign
strOutputNew: .asciz "=============\n"

.balign 4
text_one: .asciz "TEXT HERE\n" 

.balign 4
strInputError: .skip 100*4  @ User to clear the input buffer for invalid input. 

.balign 4
intInput: .word 0   @ Location used to store the user input (integer). 

.balign 4
strInput: .word 0   @ Location used to store the user input (character). 

@ Let the assembler know these are the C library functions. 

.global printf
@  To use printf:
@     r0 - Contains the starting address of the string to be printed. The string
@          must conform to the C coding standards.
@     r1 - If the string contains an output parameter i.e., %d, %c, etc. register
@          r1 must contain the value to be printed. 
@ When the call returns registers: r0, r1, r2, r3 and r12 are changed. 

.global scanf
@  To use scanf:
@      r0 - Contains the address of the input format string used to read the user
@           input value. In this example it is numInputPattern.  
@      r1 - Must contain the address where the input value is going to be stored.
@           In this example memory location intInput declared in the .data section
@           is being used.  
@ When the call returns registers: r0, r1, r2, r3 and r12 are changed.
@ Important Notes about scanf:
@   If the user entered an input that does NOT conform to the input pattern, 
@   then register r0 will contain a 0. If it is a valid format
@   then r0 will contain a 1. The input buffer will NOT be cleared of the invalid
@   input so that needs to be cleared out before attempting anything else. 
@

@end of code and end of file. Leave a blank line after this.
