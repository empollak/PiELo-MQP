#include "comparisonJumps.h"
#include "../vm.h"

using namespace PiELo;

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
        } else if((a.type == PIELO_CLOSURE || b.type == PIELO_CLOSURE) || (a.type == C_CLOSURE || b.type == C_CLOSURE)){
            // check for closure, comparing closures should be illegal
            throw InvalidTypeForOperationException("EQL", "CLOSURE");
            state = ERROR;
        }
        else if(a.type == FLOAT && b.type == INT){
            float conversion_value = static_cast<float>(b.getIntValue());
            stack.push(a.getFloatValue() == conversion_value ? 1 : 0);
        } else if(a.type == INT && b.type == FLOAT){
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
        } else if((a.type == PIELO_CLOSURE || b.type == PIELO_CLOSURE) || (a.type == C_CLOSURE || b.type == C_CLOSURE)){
            // check for closure, comparing closures should be illegal
            throw InvalidTypeForOperationException("EQL", "CLOSURE");
            state = ERROR;
        }
        else if(a.type == FLOAT && b.type == INT){
            float conversion_value = static_cast<float>(b.getIntValue());
            stack.push(a.getFloatValue() != conversion_value ? 1 : 0);
        } else if(a.type == INT && b.type == FLOAT){
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
        } else if((a.type == PIELO_CLOSURE || b.type == PIELO_CLOSURE) || (a.type == C_CLOSURE || b.type == C_CLOSURE)){
            // check for closure, comparing closures should be illegal
            throw InvalidTypeForOperationException("GT", "CLOSURE");
            state = ERROR;
        }
        else if(a.type == FLOAT && b.type == INT){
            float conversion_value = static_cast<float>(b.getIntValue());
            stack.push(a.getFloatValue() > conversion_value ? 1 : 0);
        } else if(a.type == INT && b.type == FLOAT){
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
        } else if((a.type == PIELO_CLOSURE || b.type == PIELO_CLOSURE) || (a.type == C_CLOSURE || b.type == C_CLOSURE)){
            // check for closure, comparing closures should be illegal
            throw InvalidTypeForOperationException("GTE", "CLOSURE");
            state = ERROR;
        }
        else if(a.type == FLOAT && b.type == INT){
            float conversion_value = static_cast<float>(b.getIntValue());
            stack.push(a.getFloatValue() >= conversion_value ? 1 : 0);
        } else if(a.type == INT && b.type == FLOAT){
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
        else if((a.type == PIELO_CLOSURE || b.type == PIELO_CLOSURE) || (a.type == C_CLOSURE || b.type == C_CLOSURE)){
            // check for closure, comparing closures should be illegal
            throw InvalidTypeForOperationException("LT", "CLOSURE");
            state = ERROR;
        }
        else if(a.type == FLOAT && b.type == INT){
            float conversion_value = static_cast<float>(b.getIntValue());
            stack.push(a.getFloatValue() < conversion_value ? 1 : 0);
        } else if(a.type == INT && b.type == FLOAT){
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
        else if((a.type == PIELO_CLOSURE || b.type == PIELO_CLOSURE) || (a.type == C_CLOSURE || b.type == C_CLOSURE)){
            // check for closure, comparing closures should be illegal
            throw InvalidTypeForOperationException("LTE", "CLOSURE");
            state = ERROR;
        }
        else if(a.type == FLOAT && b.type == INT){
            float conversion_value = static_cast<float>(b.getIntValue());
            stack.push(a.getFloatValue() <= conversion_value ? 1 : 0);
        } else if(a.type == INT && b.type == FLOAT){
            float conversion_value = static_cast<float>(a.getIntValue());
            stack.push(conversion_value <= b.getFloatValue() ? 1 : 0);
        } else if(a.type == FLOAT && b.type == FLOAT){
            stack.push(a.getFloatValue() <= b.getFloatValue() ? 1 : 0);
        } else {
            stack.push(a.getIntValue() <= b.getIntValue() ? 1 : 0);
        }
    } else {
        throw ShortOnElementsOnStackException("LTE");
        state = ERROR;
    }
}

void jump(){
    programCounter++;
    if(bytecode.at(programCounter).getTypeAsString() == "INT"){
        int target_address = bytecode.at(programCounter).getIntFromMemory();
        programCounter = target_address;
    } else {
        throw AddressNotDecleredException();
        state = ERROR;
    }
}

void jump_if_zero(){
    programCounter++;
    Variable top = stack.top();
    if((top.type == FLOAT && top.getFloatValue() == 0.0f) || (top.type == INT && top.getIntValue() == 0)){
        if(bytecode.at(programCounter).getTypeAsString() == "INT"){
            int target_address = bytecode.at(programCounter).getIntFromMemory();
            programCounter = target_address;
        } else {
            throw AddressNotDecleredException();
            state = ERROR;
        }
    } else {
        throw TopStackNotZeroException();
        state = ERROR;
    }
}

void jump_if_not_zero(){
    programCounter++;
    Variable top = stack.top();
    if((top.type == FLOAT && top.getFloatValue() != 0.0f) || (top.type == INT && top.getIntValue() != 0)){
        if(bytecode.at(programCounter).getTypeAsString() == "INT"){
            int target_address = bytecode.at(programCounter).getIntFromMemory();
            programCounter = target_address;
        } else {
            throw AddressNotDecleredException();
            state = ERROR;
        }
    } else {
        throw TopStackNotZeroException();
        state = ERROR;
    }
}
