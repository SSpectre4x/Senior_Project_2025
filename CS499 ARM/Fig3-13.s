/* Clements program Chapter 3 section 3.1.4 Figure 3:13 page 158 */
/* R. Kevin Preston */

.global main 

main:  
      mov  r0, #0
      mov  r1, #10
next: mul  r2, r1, r1
      mla  r0, r2, r1, r0
      subs r1, r1, #1
      bne  next

@End of my code. Force the exit and return control to OS
   mov r7, #0x01 @SVC call to exit
   svc 0         @Make the system call. 

