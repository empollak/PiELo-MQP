#pragma once
#include "instructions/closureInstructions.h"


namespace PiELo {
    enum Instruction {
        NOP,
        END,
        PUSH_NIL,
        PUSHI,
        PUSHF,
        PUSHS,
        POP,
        IS_NIL,

        ADD,
        SUB,
        MUL,
        DIV,
        MOD,

        DUP,
        SWAP,

        EQL,
        NEQL,
        GT,
        GTE,
        LT,
        LTE,

        LAND,
        LOR,
        LNOT,

        JMP,
        JMP_IF_ZERO,
        JMP_IF_NOT_ZERO,

        STORE_LOCAL,
        STORE_TAGGED,
        STORE_STIG,
        TAG_VARIABLE,
        TAG_ROBOT,
        LOAD_TO_STACK,  
        PUSH_NEXT_IN_STIG,
        IS_ITER_AT_END,
        RESET_ITER,

        STIG_SIZE,

        DEFINE_CLOSURE,
        CALL_CLOSURE,
        RET_FROM_CLOSURE,
        RERUN_CLOSURE,
        CALL_C_CLOSURE,

        PRINT,
        DEBUG_PRINT,
        SPIN,
    };
    struct opCodeInstructionOrArgument;

    void handleInstruction(opCodeInstructionOrArgument op); 
}

