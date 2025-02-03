/* Clements program Chapter 3 section 3.1.2 page 143 */
/* This is Case 1 P = 12, Q = 9 */
/* Result is 17 */
/* R. Kevin Preston */

/* Implement the following code */
/* X = P - Q */
/* IF X >= 0 THEN X = P + 5 ELSE X = P + 20 */

.data

.balign 4

X: .word 0
.balign 4
P: .word 12
.balign 4
Q: .word 9
.balign 4

.text
.balign 4

.global main 

main:  
      ldr  r0, addr_of_P
      ldr  r0, [r0]
      ldr  r1, addr_of_Q
      ldr  r1, [r1]
      ldr  r3, addr_of_X
      subs r2, r0, r1
      bpl  then
      add  r2, r0, #20
      b    exit
then: add  r2, r0, #5
exit: str  r2, [r3]
      mov  r0, r2  /*Put result in r0 to echo print */
      bx lr /* stop */

addr_of_P : .word P
addr_of_Q : .word Q
addr_of_X : .word X  
