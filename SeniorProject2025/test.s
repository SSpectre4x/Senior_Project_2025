.data            @ This should trigger a warning (before .global)
.align
.word 0x12345678

.global _start   @ The global entry point (should appear before .data)

.text
_start:
    mov r0, #5      @ Move immediate value 5 to R0
    ldr r1, =0x1000 @ Load address 0x1000 into R1
    str r2, [r1]    @ Store R2 at memory location R1 (this will trigger the .data missing error if .data is removed)
    cmp r0, #10     @ Compare R0 with 10
    svc 0           @ System call (exit)
    swi
    ldm
    ltm
    @ LTM is a bad instruction to use here.