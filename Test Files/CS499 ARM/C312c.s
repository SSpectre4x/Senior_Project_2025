/* Clements program Chapter 3 section 3.1.2 page 144 */
/* R. Kevin Preston */

.data
.text
.balign 4
.global main

main:
      mov  r0, #1     /*Counter set to 1 */
      mov  r1, #0     /*running sum set to 0 */
next: add  r1, r1, r0 /* add the counter to the running sum */
      add  r0, r0, #1 /* increment counter */
      cmp  r0, #21    /* if the counter is < 21 */
      bne  next       /* branch to do the next loop */
      mov  r0, r1     /* Put sum into r0 so it can be displayed */
      bx   lr         /*stop */

