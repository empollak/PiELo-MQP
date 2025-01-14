#include "instructionHandler.h"
// #include "arithmetic.h"
#include "instructions/arithmetic.h"
#include "instructions/closureInstructions.h"
#include "instructions/comparisonJumps.h"
#include "instructions/simpleInstructions.h"
#include "instructions/storeLoad.h"
#include "vm.h"
#ifdef __DEBUG_INSTRUCTIONS__
#define debugPrint(e) std::cout << e;
#else
#define debugPrint(e)
#endif

namespace PiELo{
    void handleInstruction(opCodeInstructionOrArgument op) {
        if (op.type != op.INSTRUCTION) {
            std::cout << "Attempted to run a type " << op.getTypeAsString() << " from pc " << programCounter << std::endl;
            std::cout << *op.asString << std::endl;
            throw std::runtime_error("Attempted to run non-instruction as instruction");
        }
        Instruction instruction = op.asInstruction;
        debugPrint("Running instruction " << instruction << std::endl);
        std::string name;
        
        ClosureData closure;
        switch(instruction) {
            case DEFINE_CLOSURE:
                // Get closure name, closure data from bytecode
                debugPrint("running define_closure");
                name = *bytecode[++programCounter].asString;
                debugPrint(" Got name " << name);
                closure = *bytecode[++programCounter].asClosure;
                debugPrint(" got closure " << std::endl);
                defineClosure(name, closure);
                break;
            case CALL_CLOSURE_NO_STORE:
                callClosureNoStore();
                break;

            case CALL_CLOSURE_STORE:
                callClosureAndStore(*bytecode[++programCounter].asString);
                break;
            
            case RET_FROM_CLOSURE:
                retFromClosure();
                break;

            case RERUN_CLOSURE:
                rerunClosure(bytecode[++programCounter].asInt);
                break;
            
            case CALL_C_CLOSURE:
                name = *bytecode[++programCounter].asString;
                debugPrint("Calling c closure: " << name);
                taggedTable[name].getFunctionPointer()();
                break;
            
            case STORE_LOCAL:
                storeLocal(*bytecode[++programCounter].asString);
                break;
            case STORE_TAGGED:
                name = *bytecode[++programCounter].asString;
                debugPrint("instructionHandler: store name: " << name << std::endl);
                std::cout << "instructionHandler: store name: " << name << std::endl;
                storeTagged(name, *bytecode[++programCounter].asString);
                break;
            case TAG_VARIABLE:
                tagVariable(*bytecode[++programCounter].asString, *bytecode[++programCounter].asString);
                break;

            case TAG_ROBOT:
                tagRobot(*bytecode[programCounter++].asString);
                break;

            case LOAD_TO_STACK:
                // printf("Loading!\n");
                // std::cout << " symbol table has: " << std::endl;
                // for (auto it : *currentSymbolTable) {
                //     std::cout << "  " << it.first << ":";
                //     it.second.print();
                //     std::cout << std::endl;
                // }
                // std::cout << "Bytecode size: " << bytecode.size() << " pc: " << programCounter << std::endl;
                // std::cout << "type: " << bytecode[programCounter].type << std::endl;
                name = *bytecode[++programCounter].asString;
                // printf("Got name\n");
                loadToStack(name);
                break;

            case PRINT:
                std::cout << "Stack top: ";
                if(stack.top().getType() == NIL){
                    std::cout << "nil";
                } else if(stack.top().getType() == INT){
                    std::cout << stack.top().getIntValue();
                } else if(stack.top().getType() == FLOAT){
                    std::cout << stack.top().getFloatValue();
                } else if (stack.top().getType() == PIELO_CLOSURE) {
                    std::cout << " cached value: ";
                    closureList[stack.top().getClosureIndex()].cachedValue.print();
                }
                std::cout << std::endl;
                break;

            case DEBUG_PRINT:
                std::cout << *bytecode[++programCounter].asString << std::endl;
                break;

            case NOP:
                // do nothing
                break;
            
            case END:
			    state = DONE;
                printf("Ran end.\n");
                break;

            case PUSH_NIL:
                pushNil();
                break;
            case PUSHI:
                pushInt();
                break;
            case PUSHF:
                pushFloat();
                break;
            case PUSHS:
                

                break;
            case POP:
                pop();
                break;
            case ADD:
                add();
                break;
            case SUB:
                sub();
                break;
            case MUL:
                mul();
                break;
            case DIV:
                div();
                break;
            case MOD:
                mod();
                break;
            case DUP:
                dup();
                break;
            case SWAP:
                swap();
                break;
            case EQL:
                eql();
                break;
            case NEQL:
                neql();
                break;
            case GT:
                gt();
                break;
            case GTE:
                gte();
                break;
            case LT:
                lt();
                break;
            case LTE:
                lte();
                break;
            case JMP:
                jump();
                break;
            case JMP_IF_ZERO:
                jump_if_zero();
                break;
            case JMP_IF_NOT_ZERO:
                jump_if_not_zero();
                break;
        }
    }
}