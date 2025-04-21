/* Clements Figure 1.11 v1 Program Page 35 */
/* Author: R. Kevin Preston */

/* ----- Data section ------ */
.data
.balign 4
/* Array of 17 digits */
Memory:
      .word 2
      .word 3
      .word 2
      .word 7 
      .word 7
      .word 3
      .word 6
      .word 6
      .word 6
      .word 6
      .word 4
      .word 7
      .word 9
      .word 2
      .word 2
      .word 2
      .word 1

.balign 4
/* Counter */
i:
      .word 0
.balign 4
New_Digit:
      .word

.balign 4
Current_Run_Value:
      .word

.balign 4
Current_Run_Length:
      .word 1

.balign 4
Max_Run:
      .word 1 

str1:
  .asciz "In the loop - Count %d \n"
str2:
  .asciz "\nThe most consective digits are %d \n"

.balign 4
return: .word 0

/* ----- Code Section ----- */

.text
.balign 4
.global main
main:

/* r8 = Memory[0] */
   ldr r8, addr_of_Memory
   ldr r8, [r8]

/* r9 = New_Digit = Memory[0] */
   mov r9, r8 

/* Set r3 to 10 (Current_Run_Value) to New_Digit */
   mov r10, r9 

/* Set r4 (Current_Run_Length) to 1 */
   mov r4, #1 

/* Set r5 (Max_Run) to 1 */
   mov r5, #1

/* Set r6 (i) to 0 */
   mov r6, #0
/* REPEAT */

repeat:
   /* Before making a function call (bl) save the current return address. */
   ldr r7, addr_of_return 
   str lr, [r7]           /* Store the return address at addr_of_return */
   ldr r0, addr_of_str1   /* Put the address of my string into the first parameter */
   mov r1, r6             /* The counter is the second parameter. */
   bl printf              /* Call the C print command */
   ldr r7, addr_of_return
   ldr lr, [r7]           /* Restore the return address. */ 

/* i = i + 1 (r6 <- r6 + 1) Increment the loop counter and array index. */
  add r6, r6, #1

/* r9 = r8 offset by r6 shifted left by 2 bits or New_Digit = Memory[i] */
   ldr r8, addr_of_Memory   
   ldr r9, [r8, r6, LSL #2]  

/* If New_Digit r9 = Current_Run_value r10 */
   cmp r9, r10 
   bne Else
/*Then Current_Run_Length r4 = Current_Run_Length + 1 */
  add r4, r4, #1
   b Line13
Else:
/*Current_Run_Length r4 = 1 */
   mov r4, #1
/*Current_Run_Value r3 = New_Digit r9 */
   mov r10, r9

Line13: 
/* If Current_Run_Length r4 > Max_Run r5 */
   cmp r4, r5
   ble Line15
/* then Max_Run r5 = Current_Run_Length r4 */
   mov r5, r4

Line15:
   cmp r6, #17
   bne repeat

   mov r0, r5
stop: 
   /* Before making a function call (bl) save the current return address. */
   ldr r7, addr_of_return 
   str lr, [r7]           /* Store the return address at addr_of_return */
   ldr r0, addr_of_str2   /* Put the address of my string into the first parameter */
   mov r1, r5             /* The Max_Run is the second parameter. */
   bl printf              /* Call the C print command */
   ldr r7, addr_of_return
   ldr lr, [r7]           /* Restore the return address. */ 

    bx lr


/* Labels needed to access data */
addr_of_New_Digit          : .word New_Digit
addr_of_Current_Run_Value  : .word Current_Run_Value
addr_of_Current_Run_Length : .word Current_Run_Length
addr_of_Max_Run            : .word Max_Run
addr_of_Memory             : .word Memory
addr_of_i                  : .word i
addr_of_str1               : .word str1
addr_of_str2               : .word str2
addr_of_return             : .word return

/* External */
.global printf



