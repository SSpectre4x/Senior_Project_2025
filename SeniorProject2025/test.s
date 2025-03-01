.data            @ This should trigger a warning (before .global)
.align
.word 0x12345678

.global _start   @ The global entry point (should appear before .data)

.text
_start:
    MOV R0, #5      @ Move immediate value 5 to R0
    LDR R1, =0x1000 @ Load address 0x1000 into R1
    STR R2, [R1]    @ Store R2 at memory location R1 (this will trigger the .data missing error if .data is removed)
    CMP R0, #10     @ Compare R0 with 10
    BEQ _start      @ Branch if equal (example of a condition affecting cyclomatic complexity)
    SVC 0           @ System call (exit)
