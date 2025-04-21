@Jeriel Leyble(pi version)
@ as -o lab5.o lab5.s
@ gcc -o lab5 lab5.o
@ ./lab5 ;echo $?
@ gdb --args ./lab5
.global main


@ ==== Initialization and Starting Display ====
main:
    @ Initialization of registers
    LDR r4, =144          @ Initial length of Board 1
    LDR r5, =144          @ Initial length of Board 2
    LDR r6, =144          @ Initial length of Board 3
    LDR r8, =0            @ Boards cut so far
    LDR r9, =0            @ Total length cut so far

    @ Display "Cut-It-Up Saw"
    LDR r0, =cutitup_msg
    BL printf

     @Display "Boards cut so far: "
    LDR r0, =boards_cut_msg
    MOV r1, r8            @ Loading the value of r8 for the number of boards cut
    BL printf

    @ Display "Linear length of boards cut so far: "
    LDR r0, =linear_length_msg
    MOV r1, r9            @ Loading the value of r9 for the linear length
    BL printf

    @ Display board lengths
    LDR r0, =board_lengths_msg
    BL printf

    LDR r0, =board_one_msg
    MOV r1, r4
    BL printf

    LDR r0, =board_two_msg
    MOV r1, r5
    BL printf

    LDR r0, =board_three_msg
    MOV r1, r6
    BL printf

    B main_loop @rkp
	
read_input:
   push {lr} @rkp
    MOV r7, #0x00
	@ Prompt the user
    LDR r0, =input_prompt
    BL  printf
    LDR r0, =input_template
    LDR r1, =input
    BL  scanf
    LDR r10, =input
    LDR r10, [r10]

    @Compare between 6 and 144
    CMP r10, #6
    BLT invalid_input
    CMP r10, #144
    BGT invalid_input
@rkp    B check_min_length
check_min_length:
    CMP r4, #6
    BLT all_boards_checked
    CMP r5, #6
    BLT all_boards_checked
    CMP r6, #6
    BLT all_boards_checked
 @rkp   B main_loop
    pop {pc} @rkp

invalid_input:
    @ Handle invalid input (e.g., print an error message) and branch back to read_input
    LDR r0, =error_msg
    BL  printf
    B read_input

main_loop:
    BL display_inventory
    BL read_input

    @ Check board one (r4)
    CMP r4, r10
    BGE cut_from_board_one

    @ Check board two (r5)
    CMP r5, r10
    BGE cut_from_board_two

    @ Check board three (r6)
    CMP r6, r10
    BGE cut_from_board_three

    @ If none of the boards are long enough
    LDR r0, =error_length
    BL printf
    B main_loop

cut_from_board_one:
    SUB r4, r4, r10  @ Subtract the desired length from board one
    B update_stats

cut_from_board_two:
    SUB r5, r5, r10  @ Subtract the desired length from board two
    B update_stats

cut_from_board_three:
    SUB r6, r6, r10  @ Subtract the desired length from board three
    B update_stats


all_boards_checked:
    BL calculate_waste

update_stats:
    ADD r8, r8, #1    @ Increment the boards cut counter
    ADD r9, r9, r10   @ Add the cut length to the total cut length counter
    BL display_inventory
    B main_loop

display_inventory:
    push {lr} @rkp
    LDR r0, =boards_cut_msg
    MOV r1, r8
    BL printf
    
    LDR r0, =length_cut_msg
    MOV r1, r9
    BL printf

    LDR r0, =current_inventory_msg
    MOV r1, r4
    MOV r2, r5
    MOV r3, r6
    BL printf
    pop {pc} @rkp
@rkp    B read_input

calculate_waste:
    ADD r10, r4, r5
    ADD r10, r10, r6
    LDR r0, =waste_msg
    BL printf

    @ Terminate program (assuming there's an exit function at this address)
    B exit_program

exit_program:
    @ Exit the program using a syscall or other method.
    MOV r7, #0x01
    SVC 0

.data
.balign 4
input_template: .asciz "%d"
input: .word 0

.balign 4
cutitup_msg:       .asciz "Cut-It-Up Saw\n"

.balign 4
boards_cut_msg:    .asciz "Boards cut so far: %d\n"

.balign 4
linear_length_msg: .asciz "Linear length of boards cut so far: %d inches\n"

.balign 4
board_lengths_msg: .asciz "Current Board Lengths:\n"

.balign 4
board_one_msg:     .asciz "One:\t%d inches\n"

.balign 4
board_two_msg:     .asciz "Two:\t%d inches\n"

.balign 4
board_three_msg:   .asciz "Three:\t%d inches\n"

.balign 4
error_msg: .asciz "Invalid input! Please enter a number between 1 and 144.\n"

.balign 4
input_prompt: .asciz "\n------------------------------\nEnter the length of board to cut (between 6 and 144): "

.balign 4
error_length: .asciz "Invalid input or no board long enough. Please try again.\n"

.balign 4
waste_msg: .asciz "Total waste: %d inches\n"

.balign 4
length_cut_msg: .asciz "Length of boards cut so far: %d inches\n"

.balign 4
current_inventory_msg: .asciz "Current Board Lengths:\nOne: %d inches\nTwo: %d inches\nThree: %d inches\n"
