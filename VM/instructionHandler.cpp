#include "instructionHandler.h"
// #include "arithmetic.h"
#include "instructions/arithmetic.h"
#include "instructions/closureInstructions.h"
#include "instructions/comparisonJumps.h"
#include "instructions/simpleInstructions.h"
#include "vm.h"

namespace PiELo{


    void storeLocal(){
        if (stack.empty()){
            throw std::runtime_error("Stack underflow: storeLocal");
        }

        Variable var = stack.top();
        stack.pop();

        std::string varName = *(var.getNameValue());
        (*currentSymbolTable)[varName] = var;
    }

    void loadToStack(const std::string& varName){
        Variable* var = nullptr;

        // search local sym table
        auto local = currentSymbolTable -> find(varName);
        if (local != currentSymbolTable -> end()) {
            var = &(local -> second);
        }

        // search tagged table
        if (!var){
            auto tag = taggedTable.find(varName);
            if (tag != taggedTable.end()) {
                var = &(tag -> second);
            }
        }
        
        if (!var){
            throw std::runtime_error("Variable not found (loadToStack): "+ varName);
        }

        stack.push(*var);
    }

    void tagVariable(const std::string& varName, const std::string& tagName) {
        Variable* var = nullptr;

        // search local sym table
        auto local = currentSymbolTable -> find(varName);
        if (local != currentSymbolTable -> end()){
            var = &(local -> second);
        }

        // search tagged table
        if (!var){
            auto tag = taggedTable.find(varName);
            if (tag != taggedTable.end()) {
                var = &(tag -> second);
            }
        }

        if (!var){
            throw std::runtime_error("Variable not found (tagVariable): " + varName);
        }

        // add tag
        var -> tags.push_back(Tag{tagName});
    }

    void tagRobot(const std::string& tagName) {
        
        // check if tag already exists
        for (const auto& tag : robotTagList){
            if (tag.tagName == tagName){
                // no need to add if it exists
                return;
            }
        }

        robotTagList.push_back(Tag{tagName});
    }

    void storeTagged(const std::string& tagName){
        if (stack.empty()){
            throw std::runtime_error("Stack underflow: storeTagged");
        }

        Variable var = stack.top();
        stack.pop();

        var.tags.push_back(Tag{tagName});
        
        taggedTable[tagName] = var;
    }

    void handleInstruction(opCodeInstructionOrArgument op) {
        if (op.type != op.INSTRUCTION) {
            throw std::runtime_error("Attempted to run non-instruction as instruction");
        }
        Instruction instruction = op.asInstruction;
        switch(instruction) {
            case DEFINE_CLOSURE:
                // Get closure name, closure data from bytecode
                defineClosure(*bytecode[programCounter++].asString, *bytecode[programCounter++].asClosure);
                break;
            case CALL_CLOSURE:
                callClosure();
                break;
            
            case RET_FROM_CLOSURE:
                retFromClosure();
                break;
            
            case STORE_LOCAL:
                storeLocal();
                break;
            case STORE_TAGGED:
                storeTagged(*bytecode[programCounter++].asString);
                break;
            case TAG_VARIABLE:
                tagVariable(*bytecode[programCounter++].asString, *bytecode[programCounter++].asString);
                break;

            case TAG_ROBOT:
                tagRobot(*bytecode[programCounter++].asString);
                break;

            case LOAD_TO_STACK:
                loadToStack(*bytecode[programCounter++].asString);
                break;

            case PRINT:
                std::cout << "Stack top: ";
                if(stack.top().type == NIL){
                    std::cout << "nil";
                } else if(stack.top().type == INT){
                    std::cout << stack.top().getIntValue();
                } else if(stack.top().type == FLOAT){
                    std::cout << stack.top().getFloatValue();
                } else if(stack.top().type == NAME){
                    std::cout << stack.top().getNameValue();
                } else {
                    std::cout << stack.top().getClosureDataValue()->codePointer;
                }
                std::cout << std::endl;
                break;

            case NOP:
                // do nothing
                break;
            
            case END:
			    state = DONE;
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