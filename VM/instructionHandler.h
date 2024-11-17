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

        JMP,
        JMP_IF_ZERO,
        JMP_IF_NOT_ZERO,

        STORE_LOCAL,
        STORE_TAGGED,
        TAG_VARIABLE,
        TAG_ROBOT,
        LOAD_TO_STACK,  

        DEFINE_CLOSURE,
        CALL_CLOSURE,
        RET_FROM_CLOSURE,

        PRINT
    };
    struct opCodeInstructionOrArgument;

    void handleInstruction(opCodeInstructionOrArgument op); 
    
}

