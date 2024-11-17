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

        DEFINE_CLOSURE
    };

    void handleInstruction(Instruction instruction); 

    void pop();
    
}

