#include "closureInstructions.h"
#include "../vm.h"
#include "storeLoad.h"

namespace PiELo{
    // Defines a closure
    void defineClosure(std::string closureName, ClosureData closureData) {
        std::cout << "defining closure " << std::endl;
        std::cout << "Defining closure with name " << closureName << std::endl;
        // Variable closureVar(closureData);
        // TODO: storeTagged with name closureName, value closureVar
        closureTemplates.push_back(closureData);
        stack.push((size_t) (closureTemplates.size() - 1));
        storeTagged(closureName, "global");
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
        std::cout << " calling closure! " << std::endl;
        returnAddrStack.push((scopeData){.scopeSymbolTable = currentSymbolTable, .codePointer = programCounter});
        if (stack.size() < 2) throw ShortOnElementsOnStackException("call_closure");

        // Copy closure template from the variable on the stack
        ClosureData closureData = closureTemplates[stack.top().getClosureIndex()];
        
        stack.pop();
        int numArgs = stack.top().getIntValue();
        stack.pop();
        if (stack.size() < numArgs) throw ShortOnElementsOnStackException("call_closure arguments");
        std::cout << " Num args: " << numArgs << std::endl;

        if (numArgs != closureData.argNames.size()) throw std::runtime_error("Mismatched number of arguments for call_closure");

        // Top of stack now holds the first argument
        for (int i = 0; i < numArgs; i++) {
            // Copy the arguments to the local symbol table
            std::string argName = closureData.argNames[i];
            Type argType = closureData.argTypes[i];
            if (stack.top().getType() != argType) throw std::runtime_error("Mismatched argument types for call_closure");
            closureData.localSymbolTable[argName] = stack.top();
            std::cout << " added arg name " << argName << " value ";
            closureData.localSymbolTable[argName].print();
            std::cout << "..." << std::endl;
            stack.pop();
        }

        // Update current local symbol table
        std::cout << " updating closure list " << std::endl;
        size_t closureIndex = closureList.size();
        closureList.push_back(closureData);
        
        currentSymbolTable = &closureList[closureIndex].localSymbolTable;
        currentClosureIndex = closureIndex;

        // Handle the dependency list
        if (closureData.dependencies.size() > 0) {
            for (std::string name : closureData.dependencies) {
                findVariable(name)->dependants.push_back(closureIndex);
            }
        }

        programCounter = closureData.codePointer;
        std::cout << " updated pc: " << programCounter << " state: " << state << std::endl;
    }

    // Expects the stack to have format:
    // Bottom
    // arg1 
    // arg2 
    // number of args : int
    // ClosureData
    // top
    void callClosureAndStore(std::string resultVarName) {
        // Call the closure
        callClosure();

        // Store the closure's index in the given variable name
        // callClosure updates currentClosureIndex
        stack.push(currentClosureIndex);
        storeTagged(resultVarName, "global");
    }

    // Expects the stack to have format:
    // Bottom
    // arg1 
    // arg2 
    // number of args : int
    // ClosureData
    // top
    void callClosureNoStore() {
        callClosure();
    }

    void retFromClosure() {
        programCounter = returnAddrStack.top().codePointer;
        if (stack.size() < 1) throw std::runtime_error("Stack empty before ret_from_closure");
        int hasReturn = stack.top().getIntValue();
        stack.pop();
        if (hasReturn == 1) {
            switch (stack.top().getType()) {
                case INT: closureList[currentClosureIndex].cachedValue = stack.top().getIntValue(); break;
                case FLOAT: closureList[currentClosureIndex].cachedValue = stack.top().getFloatValue(); break;
                case PIELO_CLOSURE: closureList[currentClosureIndex].cachedValue = stack.top().getClosureIndex(); break;
            }
            stack.pop();

            // TODO: Check if I have dependants!!
            // Need to run any closure which depends on a variable which contains this closure's index
            // This is hard. 
            // for (std::string dep : closureList[currentClosureIndex].) {

            // }
        }
        // stack.push(currentClosureIndex);
    }

    void rerunClosure(size_t closureIndex) {
        std::cout << "Rerunning closure with index " << closureIndex << std::endl;
        returnAddrStack.push((scopeData){.scopeSymbolTable = currentSymbolTable, .codePointer = programCounter});
        currentSymbolTable = &closureList[closureIndex].localSymbolTable;
        currentClosureIndex = closureIndex;
        programCounter = closureList[closureIndex].codePointer;
        std::cout << " pc now " << programCounter << std::endl;
    }

}