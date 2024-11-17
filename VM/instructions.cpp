#include "instructions.h"

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

    void add(){
        if(stack.size() >= 2) {
            Variable a = stack.top(); stack.pop();
            Variable b = stack.top(); stack.pop();

            if(a.type == NAME || b.type == NAME){
                throw InvalidTypeForOperationException("ADD", "STRING");
                state = ERROR;
            } else if(a.type == NIL || b.type == NIL) {
                throw InvalidTypeForOperationException("ADD", "NIL");
                state = ERROR;
            } else if(a.type == FLOAT && b.type != FLOAT){
                Variable result = a.getFloatValue() + static_cast<float>(b.getIntValue());
                stack.push(result);
            } else if(a.type != FLOAT && b.type == FLOAT){
                Variable result =  static_cast<float>(a.getIntValue()) + b.getFloatValue();
                stack.push(result);
            } else if(a.type == INT && b.type == INT){
                Variable result =  a.getIntValue() + b.getIntValue();
                stack.push(result);
            } else { 
                throw InvalidTypeForOperationException("ADD", "CLOSURE");
                state = ERROR;
            }
        } else {
            throw ShortOnElementsOnStackException("ADD");
            state = ERROR;
        }
    }

    void sub(){ // subtracts from the second element in the stack(b) the top of the stack(a)
        if(stack.size() >= 2) {
            Variable a = stack.top(); stack.pop();
            Variable b = stack.top(); stack.pop();

            if(a.type == NAME || b.type == NAME){
                throw InvalidTypeForOperationException("SUB", "STRING");
                state = ERROR;
            } else if(a.type == NIL || b.type == NIL) {
                throw InvalidTypeForOperationException("SUB", "NIL");
                state = ERROR;
            } else if(a.type == FLOAT && b.type != FLOAT){
                Variable result = static_cast<float>(b.getIntValue()) - a.getFloatValue();
                stack.push(result);
            } else if(a.type != FLOAT && b.type == FLOAT){
                Variable result =  b.getFloatValue() - static_cast<float>(a.getIntValue());
                stack.push(result);
            } else if(a.type == INT && b.type == INT){
                Variable result = b.getIntValue() - a.getIntValue();
                stack.push(result);
            } else { 
                throw InvalidTypeForOperationException("SUB", "CLOSURE");
                state = ERROR;
            }
        } else {
            throw ShortOnElementsOnStackException("SUB");
            state = ERROR;
        }
    }

    void mul(){ 
        if(stack.size() >= 2) {
            Variable a = stack.top(); stack.pop();
            Variable b = stack.top(); stack.pop();

            if(a.type == NAME || b.type == NAME){
                throw InvalidTypeForOperationException("MUL", "STRING");
                state = ERROR;
            } else if(a.type == NIL || b.type == NIL) {
                throw InvalidTypeForOperationException("MUL", "NIL");
                state = ERROR;
            } else if(a.type == FLOAT && b.type != FLOAT){
                Variable result = static_cast<float>(b.getIntValue()) * a.getFloatValue();
                stack.push(result);
            } else if(a.type != FLOAT && b.type == FLOAT){
                Variable result = b.getFloatValue() * static_cast<float>(a.getIntValue());
                stack.push(result);
            } else if(a.type == INT && b.type == INT){
                Variable result = b.getIntValue() * a.getIntValue();
                stack.push(result);
            } else { 
                throw InvalidTypeForOperationException("MUL", "CLOSURE");
                state = ERROR;
            }
        } else {
            throw ShortOnElementsOnStackException("MUL");
            state = ERROR;
        }
    }

    void div(){ // divide from the second element in the stack(b) the top of the stack(a)
        if(stack.size() >= 2) {
            Variable a = stack.top(); stack.pop();
            Variable b = stack.top(); stack.pop();

            if(a.type == NAME || b.type == NAME){
                throw InvalidTypeForOperationException("DIV", "STRING");
                state = ERROR;
            } else if(a.type == NIL || b.type == NIL) {
                throw InvalidTypeForOperationException("DIV", "NIL");
                state = ERROR;
            } else if((a.type == INT && a.getIntValue() == 0) || (a.type == FLOAT && a.getFloatValue() == 0.0f)){
                throw DivisionByZeroException();
                state = ERROR;
            }
            else if(a.type == FLOAT && b.type != FLOAT){
                Variable result = static_cast<float>(b.getIntValue()) / a.getFloatValue();
                stack.push(result);
            } else if(a.type != FLOAT && b.type == FLOAT){
                Variable result =  b.getFloatValue() / static_cast<float>(a.getIntValue());
                stack.push(result);
            } else if(a.type == INT && b.type == INT){
                Variable result = b.getIntValue() / a.getIntValue();
                stack.push(result);
            } else { 
                throw InvalidTypeForOperationException("SUB", "CLOSURE");
                state = ERROR;
            }
        } else {
            throw ShortOnElementsOnStackException("SUB");
            state = ERROR;
        }
    }

    void mod(){
        if(stack.size() >= 2) {
            Variable a = stack.top(); stack.pop();
            Variable b = stack.top(); stack.pop();

            if(a.type != INT || b.type != INT) {
                throw InvalidTypeForOperationException("MOD", "!= than INT. Only INT type is allowed.");
                state = ERROR;
            } else if (a.getIntValue() == 0){
                throw DivisionByZeroException();
                state = ERROR;
            } else {
                Variable result = b.getIntValue() % a.getIntValue();
                stack.push(result);
            }
        } else {
            throw ShortOnElementsOnStackException("MOD");
            state = ERROR;
        }
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

    void eql(){
        if(stack.size() >= 2){
            Variable a = stack.top(); stack.pop();
            Variable b = stack.top(); stack.pop();

            if((a.type == NAME && b.type != NAME) || (a.type != NAME && b.type == NAME)){
                stack.push(0);
            } else if(a.type == NAME && b.type == NAME){
                stack.push(a.getNameValue() == b.getNameValue() ? 1:0);
            } else if((a.type == NIL && b.type != NIL) || (a.type != NIL && b.type == NIL)){
                stack.push(0);
            } else if(a.type == NIL && b.type == NIL){
                stack.push(1);
            } 
            // check for closure, comparing closures should be illegal
            else if(a.type == FLOAT && b.type != FLOAT){
                float conversion_value = static_cast<float>(b.getIntValue());
                stack.push(a.getFloatValue() == conversion_value ? 1 : 0);
            } else if(a.type != FLOAT && b.type == FLOAT){
                float conversion_value = static_cast<float>(a.getIntValue());
                stack.push(conversion_value == b.getFloatValue() ? 1 : 0);
            } else if(a.type == FLOAT && b.type == FLOAT){
                stack.push(a.getFloatValue() == b.getFloatValue() ? 1 : 0);
            } else {
                stack.push(a.getIntValue() == b.getIntValue() ? 1 : 0);
            }
        } else {
            throw ShortOnElementsOnStackException("EQL");
            state = ERROR;
        }
    }

    void neql(){
        if(stack.size() >= 2){
            Variable a = stack.top(); stack.pop();
            Variable b = stack.top(); stack.pop();

            if((a.type == NAME && b.type != NAME) || (a.type != NAME && b.type == NAME)){
                stack.push(1);
            } else if(a.type == NAME && b.type == NAME){
                stack.push(a.getNameValue() == b.getNameValue() ? 1:0);
            } else if((a.type == NIL && b.type != NIL) || (a.type != NIL && b.type == NIL)){
                stack.push(1);
            } else if(a.type == NIL && b.type == NIL){
                stack.push(0);
            } 
            // check for closure, comparing closures should be illegal
            else if(a.type == FLOAT && b.type != FLOAT){
                float conversion_value = static_cast<float>(b.getIntValue());
                stack.push(a.getFloatValue() != conversion_value ? 1 : 0);
            } else if(a.type != FLOAT && b.type == FLOAT){
                float conversion_value = static_cast<float>(a.getIntValue());
                stack.push(conversion_value != b.getFloatValue() ? 1 : 0);
            } else if(a.type == FLOAT && b.type == FLOAT){
                stack.push(a.getFloatValue() != b.getFloatValue() ? 1 : 0);
            } else {
                stack.push(a.getIntValue() != b.getIntValue() ? 1 : 0);
            }
        } else {
            throw ShortOnElementsOnStackException("NEQL");
            state = ERROR;
        }
    }

    void gt(){
        if(stack.size() >= 2){
            Variable a = stack.top(); stack.pop();
            Variable b = stack.top(); stack.pop();

            if((a.type == NAME && b.type != NAME) || (a.type != NAME && b.type == NAME)){
                throw InvalidTypeForOperationException("GT", "NAME");
                state = ERROR;
            } else if(a.type == NAME && b.type == NAME){
                stack.push(a.getNameValue() > b.getNameValue() ? 1:0);
            } else if((a.type == NIL && b.type != NIL) || (a.type != NIL && b.type == NIL)){
                throw InvalidTypeForOperationException("GT", "NIL");
                state = ERROR;
            } else if(a.type == NIL && b.type == NIL){
                stack.push(0);
            } 
            // check for closure, comparing closures should be illegal
            else if(a.type == FLOAT && b.type != FLOAT){
                float conversion_value = static_cast<float>(b.getIntValue());
                stack.push(a.getFloatValue() > conversion_value ? 1 : 0);
            } else if(a.type != FLOAT && b.type == FLOAT){
                float conversion_value = static_cast<float>(a.getIntValue());
                stack.push(conversion_value > b.getFloatValue() ? 1 : 0);
            } else if(a.type == FLOAT && b.type == FLOAT){
                stack.push(a.getFloatValue() > b.getFloatValue() ? 1 : 0);
            } else {
                stack.push(a.getIntValue() > b.getIntValue() ? 1 : 0);
            }
        } else {
            throw ShortOnElementsOnStackException("GT");
            state = ERROR;
        }
    }

    void gte(){
        if(stack.size() >= 2){
            Variable a = stack.top(); stack.pop();
            Variable b = stack.top(); stack.pop();

            if((a.type == NAME && b.type != NAME) || (a.type != NAME && b.type == NAME)){
                throw InvalidTypeForOperationException("GTE", "NAME");
                state = ERROR;
            } else if(a.type == NAME && b.type == NAME){
                stack.push(a.getNameValue() >= b.getNameValue() ? 1:0);
            } else if((a.type == NIL && b.type != NIL) || (a.type != NIL && b.type == NIL)){
                throw InvalidTypeForOperationException("GTE", "NIL");
                state = ERROR;
            } else if(a.type == NIL && b.type == NIL){
                stack.push(1);
            } 
            // check for closure, comparing closures should be illegal
            else if(a.type == FLOAT && b.type != FLOAT){
                float conversion_value = static_cast<float>(b.getIntValue());
                stack.push(a.getFloatValue() >= conversion_value ? 1 : 0);
            } else if(a.type != FLOAT && b.type == FLOAT){
                float conversion_value = static_cast<float>(a.getIntValue());
                stack.push(conversion_value >= b.getFloatValue() ? 1 : 0);
            } else if(a.type == FLOAT && b.type == FLOAT){
                stack.push(a.getFloatValue() >= b.getFloatValue() ? 1 : 0);
            } else {
                stack.push(a.getIntValue() >= b.getIntValue() ? 1 : 0);
            }
        } else {
            throw ShortOnElementsOnStackException("GTE");
            state = ERROR;
        }
    }


    void lt(){
        if(stack.size() >= 2){
            Variable a = stack.top(); stack.pop();
            Variable b = stack.top(); stack.pop();

            if((a.type == NAME && b.type != NAME) || (a.type != NAME && b.type == NAME)){
                throw InvalidTypeForOperationException("LT", "NAME");
                state = ERROR;
            } else if(a.type == NAME && b.type == NAME){
                stack.push(a.getNameValue() < b.getNameValue() ? 1:0);
            } else if((a.type == NIL && b.type != NIL) || (a.type != NIL && b.type == NIL)){
                throw InvalidTypeForOperationException("LT", "NIL");
                state = ERROR;
            } else if(a.type == NIL && b.type == NIL){
                stack.push(0);
            } 
            // check for closure, comparing closures should be illegal
            else if(a.type == FLOAT && b.type != FLOAT){
                float conversion_value = static_cast<float>(b.getIntValue());
                stack.push(a.getFloatValue() < conversion_value ? 1 : 0);
            } else if(a.type != FLOAT && b.type == FLOAT){
                float conversion_value = static_cast<float>(a.getIntValue());
                stack.push(conversion_value < b.getFloatValue() ? 1 : 0);
            } else if(a.type == FLOAT && b.type == FLOAT){
                stack.push(a.getFloatValue() < b.getFloatValue() ? 1 : 0);
            } else {
                stack.push(a.getIntValue() < b.getIntValue() ? 1 : 0);
            }
        } else {
            throw ShortOnElementsOnStackException("LT");
            state = ERROR;
        }
    }

    void lte(){
        if(stack.size() >= 2){
            Variable a = stack.top(); stack.pop();
            Variable b = stack.top(); stack.pop();

            if((a.type == NAME && b.type != NAME) || (a.type != NAME && b.type == NAME)){
                throw InvalidTypeForOperationException("LTE", "NAME");
                state = ERROR;
            } else if(a.type == NAME && b.type == NAME){
                stack.push(a.getNameValue() <= b.getNameValue() ? 1:0);
            } else if((a.type == NIL && b.type != NIL) || (a.type != NIL && b.type == NIL)){
                throw InvalidTypeForOperationException("LTE", "NIL");
                state = ERROR;
            } else if(a.type == NIL && b.type == NIL){
                stack.push(1);
            } 
            // check for closure, comparing closures should be illegal
            else if(a.type == FLOAT && b.type != FLOAT){
                float conversion_value = static_cast<float>(b.getIntValue());
                stack.push(a.getFloatValue() <= conversion_value ? 1 : 0);
            } else if(a.type != FLOAT && b.type == FLOAT){
                float conversion_value = static_cast<float>(a.getIntValue());
                stack.push(conversion_value <= b.getFloatValue() ? 1 : 0);
            } else if(a.type == FLOAT && b.type == FLOAT){
                stack.push(a.getFloatValue() <= b.getFloatValue() ? 1 : 0);
            } else {
                stack.push(a.getIntValue() <= b.getIntValue() ? 1 : 0);
            }
        } else {
            throw ShortOnElementsOnStackException("GTE");
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

    VMState handleInstruction(Instruction instruction) {
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