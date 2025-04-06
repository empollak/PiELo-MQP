#include "simpleInstructions.h"
#include "../vm.h"

using namespace PiELo;

    void VM::pushNil(){
        Variable null_val;

        stack.push(null_val);
    }

    void VM::pushInt(){
        programCounter++;
        // std::cout << "type " << bytecode[programCounter].asInstruction << std::endl;
        Variable i_val = bytecode[programCounter].getIntFromMemory();

        stack.push(i_val);
    }

    void VM::pushFloat(){
        programCounter++;
        Variable f_val = bytecode[programCounter].getFloatFromMemory();

        stack.push(f_val);
    }

    void VM::dup(){
        if(!stack.empty()){
            Variable a = stack.top();
            stack.push(a);
        } else{
            throw ShortOnElementsOnStackException("DUP");
            state = ERROR;
        }
    }

    void VM::swap(){
        if(stack.size() >= 2){
            Variable a = stack.top(); stack.pop();
            Variable b = stack.top(); stack.pop();
            stack.push(a);
            stack.push(b);
        } else {
            throw ShortOnElementsOnStackException("SWAP");
            state = ERROR;
        }
    }

    
    void VM::pop() {
        if(stack.size() == 0) throw ShortOnElementsOnStackException("POP");
        stack.pop();
    }

    void VM::isNil() {
        if(stack.size() == 0) throw ShortOnElementsOnStackException("isNil");
        stack.push(stack.top().getType() == NIL);
    }