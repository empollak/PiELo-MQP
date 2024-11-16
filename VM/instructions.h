#pragma once
#include "vm.h"
#include "closureInstructions.h"

namespace PiELo {
    enum Instruction {
        NOP,
        END,
        PUSH_NIL,
        PUSHI,
        DEFINE_CLOSURE
    };

    VMState handleInstruction(Instruction instruction); 

    void pop();
    
}

