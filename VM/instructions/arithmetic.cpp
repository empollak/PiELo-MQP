#include "arithmetic.h"
#include "../vm.h"

using namespace PiELo;

void VM::add(){
    if(stack.size() >= 2) {
        Variable a = stack.top(); stack.pop();
        Variable b = stack.top(); stack.pop();

        if(a.getType() == NAME || b.getType() == NAME){
            throw InvalidTypeForOperationException("ADD", "NAME");
            state = ERROR;
        } else if(a.getType() == NIL || b.getType() == NIL) {
            throw InvalidTypeForOperationException("ADD", "NIL");
            state = ERROR;
        } else if(a.getType() == FLOAT && b.getType() == INT){
            Variable result = a.getFloatValue() + static_cast<float>(b.getIntValue());
            stack.push(result);
        } else if(a.getType() == INT && b.getType() == FLOAT){
            Variable result =  static_cast<float>(a.getIntValue()) + b.getFloatValue();
            stack.push(result);
        } else if(a.getType() == INT && b.getType() == INT){
            Variable result =  a.getIntValue() + b.getIntValue();
            stack.push(result);
        } else if(a.getType() == FLOAT && b.getType() == FLOAT){
            Variable result =  a.getFloatValue() + b.getFloatValue();
            stack.push(result);
        } else if (a.getType() == PIELO_CLOSURE || b.getType() == PIELO_CLOSURE){ 
            VariableData a_cv;
            if(a.getType() == PIELO_CLOSURE){
                a_cv = closureList[a.getClosureIndex()].cachedValue;
                if(a_cv.getType() == NIL || a_cv.getType() == PIELO_CLOSURE){
                    // throw an error
                } 
            }  else {
                a_cv = a.getVariableData();
            }
            VariableData b_cv;
            if(b.getType() == PIELO_CLOSURE){
                b_cv = closureList[b.getClosureIndex()].cachedValue;
            } else {
                b_cv = b.getVariableData();
            }

            // Add using the cached values (these will get popped)
            stack.push(a_cv);
            stack.push(b_cv);
            add();
            
            // throw InvalidTypeForOperationException("ADD", "CLOSURE");
            // state = ERROR;
        } else {
            throw InvalidTypeForOperationException("ADD", a.getTypeAsString() + "+" + b.getTypeAsString());
        }
    } else {
        throw ShortOnElementsOnStackException("ADD");
        state = ERROR;
    }
}

void VM::sub(){ // b - a: subtracts from the second element in the stack(b) the top of the stack(a)
    if(stack.size() >= 2) {
        Variable a = stack.top(); stack.pop();
        Variable b = stack.top(); stack.pop();

        if(a.getType() == NAME || b.getType() == NAME){
            throw InvalidTypeForOperationException("SUB", "NAME");
            state = ERROR;
        } else if(a.getType() == NIL || b.getType() == NIL) {
            throw InvalidTypeForOperationException("SUB", "NIL");
            state = ERROR;
        } else if(a.getType() == FLOAT && b.getType() == INT){
            Variable result = static_cast<float>(b.getIntValue()) - a.getFloatValue();
            stack.push(result);
        } else if(a.getType() == INT && b.getType() == FLOAT){
            Variable result =  b.getFloatValue() - static_cast<float>(a.getIntValue());
            stack.push(result);
        } else if(a.getType() == INT && b.getType() == INT){
            Variable result = b.getIntValue() - a.getIntValue();
            stack.push(result);
        } else if(a.getType() == FLOAT && b.getType() == FLOAT){
            Variable result =  b.getFloatValue() - a.getFloatValue();
            stack.push(result);
        } else if (a.getType() == PIELO_CLOSURE || b.getType() == PIELO_CLOSURE){ 
            VariableData a_cv;
            if(a.getType() == PIELO_CLOSURE){
                a_cv = closureList[a.getClosureIndex()].cachedValue;
                if(a_cv.getType() == NIL || a_cv.getType() == PIELO_CLOSURE){
                    // throw an error
                } 
            }  else {
                a_cv = a.getVariableData();
            }
            VariableData b_cv;
            if(b.getType() == PIELO_CLOSURE){
                b_cv = closureList[b.getClosureIndex()].cachedValue;
            } else {
                b_cv = b.getVariableData();
            }

            // Add using the cached values (these will get popped)
            stack.push(a_cv);
            stack.push(b_cv);
            sub();
            
            // throw InvalidTypeForOperationException("ADD", "CLOSURE");
            // state = ERROR;
        } else {
            throw InvalidTypeForOperationException("SUB", a.getTypeAsString() + "+" + b.getTypeAsString());
        }

    } else {
        throw ShortOnElementsOnStackException("SUB");
        state = ERROR;
    }
}

void VM::mul(){ 
    if(stack.size() >= 2) {
        Variable a = stack.top(); stack.pop();
        Variable b = stack.top(); stack.pop();

        if(a.getType() == NAME || b.getType() == NAME){
            throw InvalidTypeForOperationException("MUL", "STRING");
            state = ERROR;
        } else if(a.getType() == NIL || b.getType() == NIL) {
            throw InvalidTypeForOperationException("MUL", "NIL");
            state = ERROR;
        } else if(a.getType() == FLOAT && b.getType() == INT){
            Variable result = static_cast<float>(b.getIntValue()) * a.getFloatValue();
            stack.push(result);
        } else if(a.getType() == INT && b.getType() == FLOAT){
            Variable result = b.getFloatValue() * static_cast<float>(a.getIntValue());
            stack.push(result);
        } else if(a.getType() == INT && b.getType() == INT){
            Variable result = b.getIntValue() * a.getIntValue();
            stack.push(result);
        } else if(a.getType() == FLOAT && b.getType() == FLOAT){
            Variable result =  a.getFloatValue() * b.getFloatValue();
            stack.push(result);
        } else if (a.getType() == PIELO_CLOSURE || b.getType() == PIELO_CLOSURE){ 
            VariableData a_cv;
            if(a.getType() == PIELO_CLOSURE){
                a_cv = closureList[a.getClosureIndex()].cachedValue;
                if(a_cv.getType() == NIL || a_cv.getType() == PIELO_CLOSURE){
                    // throw an error
                } 
            }  else {
                a_cv = a.getVariableData();
            }
            VariableData b_cv;
            if(b.getType() == PIELO_CLOSURE){
                b_cv = closureList[b.getClosureIndex()].cachedValue;
            } else {
                b_cv = b.getVariableData();
            }

            // Add using the cached values (these will get popped)
            stack.push(a_cv);
            stack.push(b_cv);
            mul();
            
            // throw InvalidTypeForOperationException("ADD", "CLOSURE");
            // state = ERROR;
        } else {
            throw InvalidTypeForOperationException("ADD", a.getTypeAsString() + "+" + b.getTypeAsString());
        }

    } else {
        throw ShortOnElementsOnStackException("MUL");
        state = ERROR;
    }
}

void VM::div(){ // b / a: divide from the second element in the stack(b) the top of the stack(a)
    if(stack.size() >= 2) {
        Variable a = stack.top(); stack.pop();
        Variable b = stack.top(); stack.pop();

        if(a.getType() == NAME || b.getType() == NAME){
            throw InvalidTypeForOperationException("DIV", "STRING");
            state = ERROR;
        } else if(a.getType() == NIL || b.getType() == NIL) {
            throw InvalidTypeForOperationException("DIV", "NIL");
            state = ERROR;
        } else if((a.getType() == INT && a.getIntValue() == 0) || (a.getType() == FLOAT && a.getFloatValue() == 0.0f)){
            throw DivisionByZeroException();
            state = ERROR;
        }
        else if(a.getType() == FLOAT && b.getType() == INT){
            Variable result = static_cast<float>(b.getIntValue()) / a.getFloatValue();
            stack.push(result);
        } else if(a.getType() == INT && b.getType() == FLOAT){
            Variable result =  b.getFloatValue() / static_cast<float>(a.getIntValue());
            stack.push(result);
        } else if(a.getType() == INT && b.getType() == INT){
            Variable result = b.getIntValue() / a.getIntValue();
            stack.push(result);
        } else if(a.getType() == FLOAT && b.getType() == FLOAT){
            Variable result =  b.getFloatValue() / a.getFloatValue();
            stack.push(result);
        } else if (a.getType() == PIELO_CLOSURE || b.getType() == PIELO_CLOSURE){ 
            VariableData a_cv;
            if(a.getType() == PIELO_CLOSURE){
                a_cv = closureList[a.getClosureIndex()].cachedValue;
                if(a_cv.getType() == NIL || a_cv.getType() == PIELO_CLOSURE){
                    // throw an error
                } 
            }  else {
                a_cv = a.getVariableData();
            }
            VariableData b_cv;
            if(b.getType() == PIELO_CLOSURE){
                b_cv = closureList[b.getClosureIndex()].cachedValue;
            } else {
                b_cv = b.getVariableData();
            }

            // Add using the cached values (these will get popped)
            stack.push(a_cv);
            stack.push(b_cv);
            div();
            
            // throw InvalidTypeForOperationException("ADD", "CLOSURE");
            // state = ERROR;
        } else {
            throw InvalidTypeForOperationException("ADD", a.getTypeAsString() + "+" + b.getTypeAsString());
        }

    } else {
        throw ShortOnElementsOnStackException("SUB");
        state = ERROR;
    }
}

void VM::mod(){
    if(stack.size() >= 2) {
        Variable a = stack.top(); stack.pop();
        Variable b = stack.top(); stack.pop();
     
        VariableData a_cv;
        VariableData b_cv;

        if(a.getType() == PIELO_CLOSURE && b.getType() != PIELO_CLOSURE){

            a_cv = closureList[a.getClosureIndex()].cachedValue;

            if(a_cv.getType() != INT || b.getType() != INT){
                throw InvalidTypeForOperationException("MOD", "!= than INT. Only INT type is allowed.");
                state = ERROR;
            } else if (a_cv.asInt == 0){
                throw DivisionByZeroException();
                state = ERROR;
            } else {
                Variable result = b.getIntValue() % a_cv.asInt;
                stack.push(result);
            } 
            
        } 
        else if(a.getType() != PIELO_CLOSURE && b.getType() == PIELO_CLOSURE){

           b_cv = closureList[b.getClosureIndex()].cachedValue;

            if(b_cv.getType() != INT || a.getType() != INT){
                throw InvalidTypeForOperationException("MOD", "!= than INT. Only INT type is allowed.");
                state = ERROR;
            } else if (a.getIntValue() == 0){
                throw DivisionByZeroException();
                state = ERROR;
            }
            else {
                Variable result = b_cv.asInt % a.getIntValue();
                stack.push(result);
            } 
            
        } else if(a.getType() == PIELO_CLOSURE && b.getType() == PIELO_CLOSURE){

            a_cv = closureList[a.getClosureIndex()].cachedValue;
            b_cv = closureList[b.getClosureIndex()].cachedValue;

            if(a_cv.getType() != INT || b_cv.getType() != INT){
                throw InvalidTypeForOperationException("MOD", "!= than INT. Only INT type is allowed.");
                state = ERROR;
            } else if (a_cv.asInt == 0){
                throw DivisionByZeroException();
                state = ERROR;
            } else {
                Variable result = b_cv.asInt % a_cv.asInt;
                stack.push(result);
            } 
            
        } else if(a.getType() == INT || b.getType() == INT) {
            if (a.getIntValue() == 0){
                throw DivisionByZeroException();
                state = ERROR;
            } else {
                Variable result = b.getIntValue() % a.getIntValue();
                stack.push(result);
            } 
        } else {
            throw InvalidTypeForOperationException("MOD", "!= than INT. Only INT type is allowed.");
            state = ERROR;
        } 
    } else {
        throw ShortOnElementsOnStackException("MOD");
        state = ERROR;
    }
}
