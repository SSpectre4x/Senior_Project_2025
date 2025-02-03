/* ARM_Lab1_Hints.s */
/* Author: Kevin Preston */
/* Purpose: Provide enough assembly to allow the students */
/* to complete ARM Lab 1. */

/* Use these commands to assemble, link and run the program. */
/*     as -o ARM_Lab1_Hints.o ARM_Lab1_Hints.s */
/*     gcc -o ARM_Lab1_Hints ARM_Lab1_Hints.o */
/*     ./ARM_Lab1_Hints ;echo $? */

/* ----- Data section ------ */
.data
.balign 4 /* Forces the next data declaration to be on a 4 byte segment */

strExample:
.asciz "Print this string with the following number: %d \n"

.balign 4
return: .word 0 /* Needed to save the return address from this main program. */


/* ------ Code Section ----*/
/* Since we are using printf do not use r0 though r3 for this program */
/* unless you know what you are getting into. */

.text
.balign 4
.global main 

main: /* Program entry point */

/* Move a number into register 6.  */

   mov r6, #1 /*odd number*/
   mov r7, #0 /*odd number sum*/
   mov r8, #50 /*counter*/
   mov r9, #0 /*even number*/
   mov r10,  #0 /*missing comma */
 ldr r5, addr_of_return
 str lr, [r5]

   loop:
   add r7,r7,r6             /*add odd numbers to total sum*/
   ldr r0,addr_strExample   /*insert string before value*/
   mov r1,r6                /*load the current value of the odd numbers into the data*/
   bl printf                /*print the current odd value*/
   add r6,r6, #2            /*increment the odd value by 2*/
   cmp r6,r8                /*compare to see if the odd value is greater than 50*/

   blt loop

   mov r1,r7                /*Insert sum into data*/

   ldr r0,addr_strExample  
   bl printf                /*print sum of odd*/

                      
   loop2:
   add r10,r10,r9           /*add even numbers to total sum*/
   ldr r0,addr_strExample   /*insert string before value*/
   mov r1,r9                /*load the current value of the even numbers into the data*/
   bl printf                /*print the current even value*/
   add r9,r9, #2            /*increment the even value by 2*/

   cmp r9,r8                /*compare to see if the even value is greater than 50*/

   blt loop2
   ldr r0,addr_strExample  
   mov r1,r10               /*Insert sum into data*/
   bl printf                /*print sum of even*/

 ldr r5, addr_of_return
 ldr lr, [r5]

   bx  lr /*Return from main */


/* Labels needed to access data */
addr_strExample     : .word strExample
addr_of_return      : .word return

/* External calls to C library functions need to be declared here. */

.global printf
