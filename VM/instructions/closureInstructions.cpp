#include "closureInstructions.h"
#include "../vm.h"

namespace PiELo{
    // Defines a closure
    void defineClosure(std::string closureName, ClosureData closureData) {
        #ifdef __DEBUG_INSTRUCTIONS__
            std::cout << "Defining closure with name " << closureName << std::endl;
        #endif
        Variable closureVar(closureData);
        // TODO: storeTagged with name closureName, value closureVar
    }

    // Expects the stack to have format:
    // Bottom
    // arg1 
    // arg2 
    // number of args : int
    // ClosureData
    // top
    void callClosure() {
        // Save the current scope
        returnAddrStack.push((scopeData){.scopeSymbolTable = currentSymbolTable, .codePointer = programCounter});
        if (stack.size() < 2) throw ShortOnElementsOnStackException("call_closure");

        // Copy closure template from the variable on the stack
        ClosureData closureData = *stack.top().getClosureDataValue();
        
        stack.pop();
        int numArgs = stack.top().getIntValue();
        if (stack.size() < numArgs) throw ShortOnElementsOnStackException("call_closure arguments");

        if (numArgs != closureData.argNames.size()) throw std::runtime_error("Mismatched number of arguments for call_closure");

        // Top of stack now holds the first argument
        for (int i = 0; i < numArgs; i++) {
            // Copy the arguments to the local symbol table
            std::string argName = closureData.argNames[i];
            Type argType = closureData.argTypes[i];
            if (stack.top().type != argType) throw std::runtime_error("Mismatched argument types for call_closure");
            closureData.localSymbolTable[argName] = stack.top();
            stack.pop();
        }

        // Update current local symbol table
        currentSymbolTable = &closureData.localSymbolTable;

        // Handle the dependency list
        if (closureData.dependencies.size() > 0) {
            size_t closureIndex = closureList.size();
            closureList.push_back(closureData);
            for (Variable v : closureData.dependencies) {
                v.dependants.push_back(closureIndex);
            }
        }

        programCounter = closureData.codePointer;
    }

    void retFromClosure() {
        programCounter = returnAddrStack.top().codePointer;
        if (stack.size() < 1) throw std::runtime_error("Stack empty before ret_from_closure");
        if (stack.top().getIntValue() == 1) {
            
        }
    }

}