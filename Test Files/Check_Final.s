@ Error Test File
@ 
@ missing \n for strEvenSum in .data
@ invalid mov instruction in [main]
@ invalid str instruction in [Q24]
@ miss-spelled printf on directive instruction [.global], printf won't work

/* Check_Final.s */
/* Kevin Preston */

/* Generate the even numbers from 1 to 50, print each even number.  */
/* Add these even numbers and print the sum. */

/* Generate the odd numbers from 1 to 50, print each odd number.  */
/* Add these odd numbers and print the sum. */

/* Subtract the sum of the even numbers from the sum of the odd numbers and print this value. */

/* ----- Data section ------ */
.data
.balign 4

strEvenSum:
.asciz "The sum of the even numbers from 1 to 50 is %d "

strOddSum:
.asciz "The sum of the odd numbers from 1 to 50 is %d \n"

strDiff:
.asciz "The difference between the even and odd sum is %d \n"

.balign 4
return: .word 0 


/* ------ Code Section ----*/
/* Since we are using printf do you use r0 though r4 for this program */

.text
.balign 4
.global main 

main: /* Program entry point */

   mov r0
   mov r1, #0x400
   mov r2, #0x8

a: mov r0,r1, LSL r2

   mov r0, #0
   mov r1, #0x400
   mov r2, #0x8

b: mov r0,r1, LSR r2

   mov r0, #0
   mov r1, #0x400
   mov r2, #0x8

c: add r0, r1, r1, LSL #2

   mov r0, #0
   mov r1, #0x400
   mov r2, #0x8

d: add r0, r1, r1, ROR #8

   mov r0, #0
   mov r1, #0x400
   mov r2, #0x8

e: sub r0, r1, r2

/* Question 24. */

Q24: mov r3, #0xFF

   strr r0, [r3]
   str r0, [r3,  #8]
   str r0, [r3, #4]!
   str r0, [r3], #4
/*   str r0, [r3, LSL #8] */


   mov r6, #0 /* even_sum = 0 */
   mov r7, #0 /* odd_sum = 0 */
   mov r8, #1 /* counter */
   mov r9, #50 /* upper limit of the counter */

loop:
   /* top of the loop will always be odd */
   add r7, r7, r8  /* odd_sum = odd_sum + counter */

   add r8, r8, #1 /* update counter. It is now even */

   add r6, r6, r8 /* even_sum = even_sum + counter */

   add r8, r8, #1 /* update counter. It is now odd */

   cmp r8, r9  /* see if the limit of 50 has been reached */

   blt  loop

/* Calculations for the sums have been completed so print the results */

/* Before making a function call (bl) save the current return address. */
   ldr r5, addr_of_return 
   str lr, [r5]              /* Store the return address at addr_of_return */
   ldr r0, addr_strEvenSum   /* Put the address of my string into the first parameter */
   mov r1, r6             /* The counter is the second parameter. */
   bl printf              /* Call the C print command */
   ldr r5, addr_of_return
   ldr lr, [r5]           /* Restore the return address. */ 

   ldr r5, addr_of_return 
   str lr, [r5]              /* Store the return address at addr_of_return */
   ldr r0, addr_strOddSum   /* Put the address of my string into the first parameter */
   mov r1, r7             /* The counter is the second parameter. */
   bl printf              /* Call the C print command */
   ldr r5, addr_of_return
   ldr lr, [r5]           /* Restore the return address. */ 

/* Everything have been printed except for the difference */
/* Calculate the difference then print */
/* calculate odd - even or r7  - r6 */
   
   sub r6, r7, r6   /* everything have been printed so r6 can be used */
   ldr r5, addr_of_return 
   str lr, [r5]              /* Store the return address at addr_of_return */
   ldr r0, addr_strEvenSum   /* Put the address of my string into the first parameter */
   mov r1, r6             /* The counter is the second parameter. */
   bl printf              /* Call the C print command */
   ldr r5, addr_of_return
   ldr lr, [r5]           /* Restore the return address. */ 

   bx  lr /*Return from main */


/* Labels needed to access data */
addr_strEvenSum     : .word strEvenSum
addr_strOddSum      : .word strOddSum
addr_strDiff        : .word strDiff
addr_of_return      : .word return


/* External */

.global pintf
