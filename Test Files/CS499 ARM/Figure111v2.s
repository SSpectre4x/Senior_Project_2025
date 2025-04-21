@ Clements Figure 1.11 v2 Program Page 35 
@ Author:  R. Kevin Preston 
@ Purpose: Take the program from the text book and codes it in 
@          ARM assembly. 
@
@ Filename: ARM-Programs/Figure111v2.s
@ 
@ Unix commands to assemble, link, run and debug this code. 
@  as  -o Figure111v2.o Figure111v2.s
@  gcc -o Figure111v2 Figure111v2.o
@  ./Figure111v2 ;echo $?
@  gdb --args ./Figure111v2

/* ----- Code Section ----- */

.text
.global main
main:
@
@ Register assignments:
@  Reg    Purpose
@  ---    -------
@  r4     Current_Run_Length
@  r5     Max_Run
@  r6     i - Counter to access Memory array
@  r8     Current element in Memory array
@  r9     New_Digit
@  r10    Current_Run_Value


@ Initilize all the registers

/* r8 = Memory[0] starting address of the array of numbers. */
   ldr r8, =Memory
   ldr r8, [r8]

/* r9 = New_Digit = Memory[0] */
   mov r9, r8 

/* Set r10 to r9 Current_Run_Value)to New_Digit */
   mov r10, r9 

/* Set r4 Current_Run_Length to 1 */
   mov r4, #1 

/* Set r5 Max_Run to 1 */
   mov r5, #1

/* Set r6 i to 0 */
   mov r6, #0

repeat: @ Line 5

   ldr r0, =str1          /* Put the address of my string into the first parameter */
   mov r1, r6             /* The counter is the second parameter. */
   bl printf              /* Call the C print command */


/* i = i + 1 (r6 <- r6 + 1) Increment the loop counter and array index. */
  add r6, r6, #1

/* r9 = r8 offset by r6 shifted left by 2 bits or New_Digit = Memory[i] */
   ldr r8, =Memory 
   ldr r9, [r8, r6, LSL #2]  

/* If New_Digit r9 = Current_Run_value r10 */
   cmp r9, r10 
   bne Else

/*Then Current_Run_Length r4 = Current_Run_Length + 1 */
  add r4, r4, #1
  b   Line13

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

stop: @ Line 16

   ldr r0, =str2  /* Put the address of my string into the first parameter */
   mov r1, r5     /* The Max_Run is the second parameter. */
   bl printf      /* Call the C print command */

Exit:
    MOV  r7, #0X01  @ This is the system service call to exit the program. 
    SVC  0

@ ----- Data section ------ 
.data
.balign 4
/* Array of 17 digits */

Memory: .word 2, 3, 2, 7, 7, 3, 6, 6, 6, 6, 6, 4, 7, 9, 2, 2, 2, 1

.balign 4

str1:
  .asciz "In the loop - Count %d \n"

.balign 4
str2:
  .asciz "\nThe most consective digits are %d \n"

/* External */
.global printf

