#pragma once

// arm_operators.h

#ifndef ARM_OPERATORS_H
#define ARM_OPERATORS_H

#include <unordered_set>
#include <string>

const std::unordered_set<std::string> ARM_OPERATORS = {
    /* Arithmetic */
    "ADD", "ADDS", "ADC", "ADCS", "SUB", "SUBS", "SBC", "SBCS", "RSB", "RSBS", "RSC", "RSCS",
    "MUL", "MULS", "MLA", "MLAS", "UMULL", "UMLAL", "SMULL", "SMLAL",

    /* Logical */
    "AND", "ANDS", "ORR", "ORRS", "EOR", "EORS", "BIC", "BICS", "MVN", "MVNS",

    /* Shift and Rotate */
    "LSL", "LSR", "ASR", "ROR", "RRX",

    /* Comparison and Testing */
    "CMP", "CMN", "TST", "TEQ",

    /* Data Movement */
    "MOV", "MOVS", "MVN", "MVNS", "MRS", "MSR",

    /* Load/Store */
    "LDR", "LDRB", "LDRH", "LDRSB", "LDRSH", "STR", "STRB", "STRH", "LDM", "STM", "PUSH", "POP",

    /* Branching */
    "B", "BL", "BX", "BLX",

    /* Software Interrupts */
    "SWI", "SVC",

    /* Miscellaneous */
    "NOP", "BKPT", "CLZ", "REV", "REV16", "REVSH", "UXTB", "UXTH", "SXTB", "SXTH",

    /* Conditional Branching */
    "BEQ", "BNE", "BCS", "BCC", "BMI", "BPL", "BVS", "BVC",
    "BHI", "BLS", "BGE", "BLT", "BGT", "BLE",

    /* Conditional Execution */
    "ADDEQ", "ADDNE", "SUBGT", "MOVLE", "ANDMI", "ORRVS", "EORGE", "BICLE", "SUBCS", "RSBHI",


    /* arithmetic */
    "add", "adds", "adc", "adcs", "sub", "subs", "sbc", "sbcs", "rsb", "rsbs", "rsc", "rscs",
    "mul", "muls", "mla", "mlas", "umull", "umlal", "smull", "smlal",

    /* logical */
    "and", "ands", "orr", "orrs", "eor", "eors", "bic", "bics", "mvn", "mvns",

    /* shift and rotate */
    "lsl", "lsr", "asr", "ror", "rrx",

    /* comparison and testing */
    "cmp", "cmn", "tst", "teq",

    /* data movement */
    "mov", "movs", "mvn", "mvns", "mrs", "msr",

    /* load/store */
    "ldr", "ldrb", "ldrh", "ldrsb", "ldrsh", "str", "strb", "strh", "ldm", "stm", "push", "pop",

    /* branching */
    "b", "bl", "bx", "blx",

    /* software interrupts */
    "swi", "svc",

    /* miscellaneous */
    "nop", "bkpt", "clz", "rev", "rev16", "revsh", "uxtb", "uxth", "sxtb", "sxth",

    /* conditional branching */
    "beq", "bne", "bcs", "bcc", "bmi", "bpl", "bvs", "bvc",
    "bhi", "bls", "bge", "blt", "bgt", "ble",

    /* conditional execution */
    "addeq", "addne", "subgt", "movle", "andmi", "orrvs", "eorge", "bicle", "subcs", "rsbhi"
};

// ARM condition codes
const std::unordered_set<std::string> conditions = {
    "eq", "ne", "cs", "hs", "cc",
    "lo", "mi", "pl", "vs", "vc",
    "hi", "ls", "ge", "lt", "gt", "le",

    "EQ", "NE", "CS", "HS", "CC",
    "LO", "MI", "PL", "VS", "VC",
    "HI", "LS", "GE", "LT", "GT", "LE",
};

const std::unordered_set<std::string> branches = {
    /* Branching */
    "B", "BL", "BX", "BLX",
    /* branching */
    "b", "bl", "bx", "blx",
};

#endif // ARM_OPERATORS_H