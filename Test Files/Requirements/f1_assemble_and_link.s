@ Requirement validation test file.
@ Expected: Program should display output from assemble/link commands
@ and halt if program fails to assemble/link.

.global prompt

main:
    b myexit
myexit:
    mov r7, #0x01
    svc 0

.data