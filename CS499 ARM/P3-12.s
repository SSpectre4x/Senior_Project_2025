@ P3-12.s
@ Clements four-function calculator program
@ pages 221 - 223
@ Modified for Raspberry Pi - ARM by Kevin Preston

@ as -o P3-12.o P3-12.s
@ gcc -o P3-12 P3-12.o 
@ ./P3-12 ;echo $?

.text
.balign 4
.global main
 
main:

@Print the intro string.
        mov   r7, #0x04     @write command
        mov   r0, #0x01     @stdout
        mov   r2, #0x21     @length of newline
        ldr   r1, =strIntro @address of NewLine
        svc   0             @ do the system call

calc:   @mov   r13, #0xA000  @Stack pointer is already set. 
        bl    Newln 

        bl    input
        mov   r2, r0    @operator 
        mov   r3, r1    @first number
        bl    Newln
        bl    input
        mov   r4, r0   @second number
        bl    Newln
        bl    math     @do the calculation
        cmp   r4, #'h'
        bleq  outHex
        blne  outDec
        bl    Newln
        bl    getCh
        cmp   r0, #'y'
        bl    Newln
        beq   calc
        mov   r7, #01 @Add code to exit.  
        svc   0       @exit

input:  @read string of digits and total in r1. 
        @r0 is the input register
        @clear the registers
        mov   r0, #0      
        mov   r1, #0
next:   str   r14, [sp, #-4]
        bl    getCh
        ldr   r14, [sp], #4
        cmp   r0, #'0'
        movlt pc, r14
        cmp   r0, #'9'
        movgt pc, r14
        sub   r0, r0, #0x30
        mov   r4, r1
        mov   r5, #10
        mul   r1, r4, r5
        add   r1, r1, r0
        b     next

getCh:  mov   r7, #4     @character input
        svc   0
        mov   pc, r14    @return

putCh:  mov   r7, #0
        svc   0
        mov   pc, r14    @return

math:   cmp   r2, #'+'
        addeq r1, r1, r3 
        cmp   r2, #'-'
        subeq r1, r3, r1
        cmp   r2, #'*'
        moveq r4, r1
        muleq r2, r4, r3
        mov   pc, r14    @return

outHex: stmfd r13!, {r0, r1, r8, r14}
        mov   r8, #8
outNxt: mov   r1, r1, ror #28
        and   r0, r1, #0xf
        add   r0, r0, #0x30
        cmp   r0, #0x39
        addgt r0, r0, #7
        str   r14, [sp], #4
        bl    putCh
        ldr   r14, [sp], #4
        subs  r8, r8, #1
        bne   outNxt
        ldmfd r13!, {r0, r1, r8, pc} @return

outDec: stmfd r13!, {r0, r1, r2, r8, r14}
        mov   r8, #0
        mov   r4, #0
outNx2: mov   r8, r8, lsl #4
        add   r4, r4, #1
        bl    div10
        add   r8, r8, r2
        cmp   r1, #0
        bne   outNx2
outNx1: and   r0, r8, #0xf
        add   r0, r0, #0x30
        movs  r8, r8, lsr #4
        bl    putCh
        subs  r4, r4, #1
        bne   outNx1
outEx:  ldmfd r13!, {r0, r1, r2, r8, pc} @return

div10:  sub   r2, r1, #10
        sub   r1, r1, r1, lsr #2
        add   r1, r1, r1, lsr #4
        add   r1, r1, r1, lsr #8
        add   r1, r1, r1, lsr #16
        mov   r1, r1, lsr #3
        add   r3, r1, r1, asl #2
        subs  r2, r2, r3, asl #1
        addpl r1, r1, #1
        addmi r2, r2, #10
        mov   pc, r14     @return

@ Put carrage return and line feed to std out.
Newln:  stmfd r13!, {r0, r14}
        mov   r7, #0x04 @write command
        mov   r0, #0x01 @stdout
        mov   r2, #0x1  @length of newline
        ldr   r1, =strNewLine @address of NewLine
        svc   0         @ do the system call
        ldmfd r13!, {r0, pc}  @return

@end

.data
.balign 4

strIntro:
.asciz "Start of Calculator.\n" @length 0x21

strNewLine:
.asciz "\n" @length 0x1 
   

        