@ Filename: Johnson-Lab5.s
@ Author:   Adam Johnson
@ Email:    amj0034@uah.edu
@ Term:     CS309-02 2023
@ Purpose:  The purpose of this ARM assembly program is to simulate a board cutting operation for a lumber processing facility. It takes user input to determine the length of the board to be cut, performs the cutting operation by updating the lengths of available boards, and keeps track of the total number of boards cut and the linear length of boards cut so far. The program ensures that the requested cut lengths are within the specified minimum and maximum limits and that there is sufficient inventory to make the cut. If the inventory drops below the minimum cut length, the program calculates the waste and terminates, providing a summary of the operations performed and the waste generated.

@ Use these commands to assemble, link, run and debug this program:
@    	as -o Johnson-Lab5.o Johnson-Lab5.s
@    	gcc -o Johnson-Lab5 Johnson-Lab5.o
@    	./Johnson-Lab5 ;echo $?
@    	gdb --args ./Johnson-Lab5

.global main               @ Declare the main function as a global symbol for the linker.
.global printf             @ Declare printf as a global symbol, which will be linked with the C library.
.global scanf              @ Declare scanf as a global symbol, which will be linked with the C library.

.equ SUCCESS, 1            @ Define a constant for successful scanf return value.
.equ READERROR, 0          @ Define a constant for scanf read error.
.equ MIN_CUT_LENGTH, 6     @ Define a constant for the minimum cut length.
.equ MAX_CUT_LENGTH, 144   @ Define a constant for the maximum cut length.
.equ BOARD_LENGTH, 144     @ Define a constant for the initial length of the boards.
.equ BOARD_COUNT, 3        @ Define a constant for the number of boards.

main:                       @ Start of the main function.
    ldr r4, =BOARD_LENGTH  @ Load the initial length of board one into r4.
    ldr r5, =BOARD_LENGTH  @ Load the initial length of board two into r5.
    ldr r6, =BOARD_LENGTH  @ Load the initial length of board three into r6.
    mov r7, #0             @ Initialize the count of boards cut to 0 in r7.
    mov r8, #0             @ Initialize the linear length of boards cut to 0 in r8.

    @ Display the initial message.
    ldr r0, =initialMsg    @ Load the address of the initial message into r0.
    bl printf              @ Call printf to display the initial message.

    @ Start of the main input loop.
input_loop:
    @ Prompt for input.
    ldr r0, =inputPrompt   @ Load the address of the input prompt into r0.
    bl printf              @ Call printf to display the input prompt.

    @ Read user input.
    ldr r0, =scanPattern   @ Load the address of the scanf pattern into r0.
    ldr r1, =inputValue    @ Load the address where the input value will be stored into r1.
    bl scanf               @ Call scanf to read the user input.
    cmp r0, #SUCCESS       @ Compare the return value of scanf with SUCCESS.
    beq check_input        @ If scanf was successful, branch to check_input.
    b input_error          @ If scanf failed, branch to input_error.

check_input:
    ldr r1, =inputValue    @ Load the address of the input value into r1.
    ldr r1, [r1]           @ Load the actual input value from memory into r1.
    cmp r1, #MIN_CUT_LENGTH @ Compare the input value with the minimum cut length.
    blt input_error        @ If the input is less than the minimum, branch to input_error.
    cmp r1, #MAX_CUT_LENGTH @ Compare the input value with the maximum cut length.
    bgt input_error        @ If the input is greater than the maximum, branch to input_error.

    @ Cutting logic.
    bl cut_board           @ Call the cut_board function to attempt to cut the board.
    cmp r0, #0             @ Compare the return value of cut_board with 0.
    beq input_loop         @ If cut_board returned 0 (failure), loop back to prompt for input again.

    @ Update counters.
    add r7, r7, #1         @ Increment the count of boards cut in r7.
    add r8, r8, r0         @ Add the length of the cut made to the total linear length cut in r8.

    @ Display the current state.
    ldr r0, =cutSuccessMsg @ Load the address of the cut success message into r0.
    mov r1, r7             @ Move the count of boards cut into r1.
    mov r2, r8             @ Move the total linear length cut into r2.
    mov r3, r4             @ Move the length of board one into r3.
    push {r5-r6}           @ Push the lengths of boards two and three onto the stack.
    bl printf              @ Call printf to display the cut success message.
    pop {r5-r6}            @ Pop the lengths of boards two and three off the stack.
    ldr r0, =boardStatusMsg @ Load the address of the board status message into r0.
    mov r1, r4             @ Move the length of board one into r1.
    mov r2, r5             @ Move the length of board two into r2.
    mov r3, r6             @ Move the length of board three into r3.
    bl printf              @ Call printf to display the board status message.

    @ Check if all boards are less than the minimum cut length.
    cmp r4, #MIN_CUT_LENGTH @ Compare the length of board one with the minimum cut length.
    blt check_board_two    @ If board one is too short, branch to check_board_two.
    cmp r5, #MIN_CUT_LENGTH @ Compare the length of board two with the minimum cut length.
    blt check_board_three  @ If board two is too short, branch to check_board_three.
    cmp r6, #MIN_CUT_LENGTH @ Compare the length of board three with the minimum cut length.
    blt termination        @ If board three is too short, branch to termination.
    b input_loop           @ If none of the boards are too short, loop back to prompt for input again.

check_board_two:
    cmp r5, #MIN_CUT_LENGTH @ Compare the length of board two with the minimum cut length.
    blt check_board_three  @ If board two is too short, branch to check_board_three.
    b input_loop           @ If board two is not too short, loop back to prompt for input again.

check_board_three:
    cmp r6, #MIN_CUT_LENGTH @ Compare the length of board three with the minimum cut length.
    blt termination        @ If board three is too short, branch to termination.
    b input_loop           @ If board three is not too short, loop back to prompt for input again.

input_error:
    ldr r0, =inputErrorMsg @ Load the address of the input error message into r0.
    bl printf              @ Call printf to display the input error message.
    b input_loop           @ Loop back to prompt for input again.

termination:
    @ Calculate waste.
    mov r1, #BOARD_LENGTH  @ Move the board length constant into r1.
    mov r2, #BOARD_COUNT   @ Move the board count constant into r2.
    mul r3, r1, r2         @ Multiply the board length by the board count, result in r3.
@rkp    sub r0, r8, r3         @ Subtract the total length cut from the total original length, result in r0.
        sub r0, r3, r8        
@rsb r0, r8, #432

    @ Display the final message and waste.
    mov r1, r0 @rkp
    ldr r0, =terminationMsg @ Load the address of the termination message into r0.
@rkp    mov r1, r0              @ Move the address of the termination message into r1.
    bl printf               @ Call printf to display the termination message.

    @ Exit the program.
    mov r0, #0              @ Move 0 into r0 to indicate successful termination.
    bx lr                   @ Return from the main function.

cut_board:
    ldr r2, =inputValue     @ Load the address of the input value into r2.
    ldr r2, [r2]            @ Load the actual input value from memory into r2.

    @ Check and cut from board one.
    cmp r4, r2              @ Compare the length of board one with the requested cut length.
    blt try_board_two       @ If board one is too short, branch to try_board_two.
    sub r4, r4, r2          @ Subtract the requested cut length from board one.
    mov r0, r2              @ Move the requested cut length into r0 to indicate success.
    bx lr                   @ Return from the cut_board function.

try_board_two:
    @ Check if board two is less than the minimum cut length.
    cmp r5, r2              @ Compare the length of board two with the requested cut length.
    blt try_board_three     @ If board two is too short, branch to try_board_three.
    sub r5, r5, r2          @ Subtract the requested cut length from board two.
    mov r0, r2              @ Move the requested cut length into r0 to indicate success.
    bx lr                   @ Return from the cut_board function.

try_board_three:
    @ Check and cut from board three.
    cmp r6, r2              @ Compare the length of board three with the requested cut length.
    blt cut_fail            @ If board three is too short, branch to cut_fail.
    sub r6, r6, r2          @ Subtract the requested cut length from board three.
    mov r0, r2              @ Move the requested cut length into r0 to indicate success.
    bx lr                   @ Return from the cut_board function.

cut_fail:
    ldr r0, =notEnghInvMsg  @ Load the address of the not enough inventory message into r0.
    bl printf               @ Call printf to display the not enough inventory message.
    mov r0, #0              @ Move 0 into r0 to indicate failure.
    bx lr                   @ Return from the cut_board function.

clear_input_buffer:
    @ Clear the input buffer.
    ldr r1, =tempChar       @ Load the address of the temporary character storage into r1.

clear_loop:
    bl getchar              @ Call getchar to read a character from the input buffer.
    ldrb r1, [r0]           @ Load the character read into r1.
    cmp r1, #'\n'           @ Compare the character with newline.
    beq clear_done          @ If the character is newline, branch to clear_done.
    cmp r1, #0              @ Compare the character with null terminator.
    beq clear_done          @ If the character is null terminator, branch to clear_done.
    b clear_loop            @ Loop back to read the next character.

clear_done:
    bx lr                   @ Return from the clear_input_buffer function.

.data                       @ Start of the data section.

initialMsg:       .asciz "Cut-It-Up Saw\nBoards cut so far: 0\nLiner length of boards cut so far: 0 inches\nCurrent Board Lengths:\nOne:\t144 inches\nTwo:\t144 inches\nThree:\t144 inches\n\n"
inputPrompt:      .asciz "Enter the length of board to cut in inches (at least 6 and no more than 144):\n"
inputErrorMsg:    .asciz "Invalid input. Please enter a number between 6 and 144.\n"
cutSuccessMsg:    .asciz "Boards cut so far: %d\nLiner length of boards cut so far: %d inches\n"
boardStatusMsg:   .asciz "Current Board Lengths:\nOne:\t%d inches\nTwo:\t%d inches\nThree:\t%d inches\n"
terminationMsg:   .asciz "Inventory levels have dropped below minimum levels and will now terminate.\nWaste is %d inches.\n"
notEnghInvMsg:    .asciz "Not enough inventory to perform cut. Enter a new length.\n"
scanPattern:      .asciz " %d"
getcharPattern:   .asciz " %c"
.balign 4
inputValue:       .word 0
tempChar:         .byte 0

.end                       @ End of the file.
