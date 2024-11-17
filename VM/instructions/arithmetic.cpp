#include "arithmetic.h"
#include "../vm.h"

using namespace PiELo;

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
        } else if(a.type == FLOAT && b.type == INT){
            Variable result = a.getFloatValue() + static_cast<float>(b.getIntValue());
            stack.push(result);
        } else if(a.type == INT && b.type == FLOAT){
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
        } else if(a.type == FLOAT && b.type == INT){
            Variable result = static_cast<float>(b.getIntValue()) - a.getFloatValue();
            stack.push(result);
        } else if(a.type == INT && b.type == FLOAT){
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
        } else if(a.type == FLOAT && b.type == INT){
            Variable result = static_cast<float>(b.getIntValue()) * a.getFloatValue();
            stack.push(result);
        } else if(a.type == INT && b.type == FLOAT){
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
        else if(a.type == FLOAT && b.type == INT){
            Variable result = static_cast<float>(b.getIntValue()) / a.getFloatValue();
            stack.push(result);
        } else if(a.type == INT && b.type == FLOAT){
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
