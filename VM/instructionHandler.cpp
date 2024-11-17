#include "instructionHandler.h"
#include "arithmetic.h"
#include "vm.h"

namespace PiELo{

    void pushNil(){
        Variable null_val;
        null_val.type = NIL;

        stack.push(null_val);
    }

    void pushInt(){
        programCounter++;
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

    void handleInstruction(Instruction instruction) {
        switch (instruction) {
            case DEFINE_CLOSURE:
                programCounter++;
                std::string closureName = *bytecode[programCounter].asString;
                programCounter++;
                ClosureData closureData = bytecode[programCounter].asClosure;
                defineClosure(closureName, closureData);
                break;
        }
    }
}