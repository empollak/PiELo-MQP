#include "simpleInstructions.h"
#include "../vm.h"

using namespace PiELo;

    void pushNil(){
        Variable null_val;
        null_val.type = NIL;

        stack.push(null_val);
    }

    void pushInt(){
        programCounter++;
        // std::cout << "type " << bytecode[programCounter].asInstruction << std::endl;
        Variable i_val = bytecode[programCounter].getIntFromMemory();

        stack.push(i_val);
    }

    void pushFloat(){
        programCounter++;
        Variable f_val = bytecode[programCounter].getFloatFromMemory();

        stack.push(f_val);
    }

    void dup(){
        if(!stack.empty()){
            Variable a = stack.top();
            stack.push(a);
        } else{
            throw ShortOnElementsOnStackException("DUP");
            state = ERROR;
        }
    }

    void swap(){
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

    
    void pop() {
        stack.pop();
    }