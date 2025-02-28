#include "closureInstructions.h"
#include "../vm.h"
#include "storeLoad.h"

#ifdef __DEBUG_INSTRUCTIONS__
#define debugPrint(e) std::cout << e;
#else
#define debugPrint(e)
#endif

namespace PiELo{
    // Defines a closure
    void defineClosure(std::string closureName, ClosureData closureData) {
        debugPrint("Defining closure with name " << closureName << std::endl)

        // Variable closureVar(closureData);
        // TODO: storeTagged with name closureName, value closureVar
        closureTemplates.push_back(closureData);
        stack.push((size_t) (closureTemplates.size() - 1));
        debugPrint(" at closureTemplates index " << closureTemplates.size() - 1 << std::endl);
        storeLocal(closureName);
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
        debugPrint(" calling closure! " << std::endl);
        returnAddrStack.push((scopeData){.scopeSymbolTable = currentSymbolTable, .codePointer = programCounter, .closureIndex = currentClosureIndex});
        if (stack.size() < 2) throw ShortOnElementsOnStackException("call_closure");

        // Copy closure template from the variable on the stack
        ClosureData closureData = closureTemplates[stack.top().getClosureIndex()];
        
        stack.pop();
        int numArgs = stack.top().getIntValue();
        stack.pop();
        if (stack.size() < numArgs) throw ShortOnElementsOnStackException("call_closure arguments");
        debugPrint(" Num args: " << numArgs << std::endl);

        if (numArgs != closureData.argNames.size()) throw std::runtime_error("Mismatched number of arguments for call_closure");

        // Top of stack now holds the first argument
        for (int i = 0; i < numArgs; i++) {
            // Copy the arguments to the local symbol table
            std::string argName = closureData.argNames[i];
            Type argType = closureData.argTypes[i];
            if (stack.top().getType() != argType) throw std::runtime_error("Mismatched argument types for call_closure");
            closureData.localSymbolTable[argName] = stack.top();
            debugPrint(" added arg name " << argName << " value ");
            closureData.localSymbolTable[argName].print();
            debugPrint("..." << std::endl);
            stack.pop();
        }

        // Update current local symbol table
        debugPrint(" updating closure list " << std::endl);
        size_t closureIndex = closureList.size();
        closureList.push_back(closureData);
        
        currentSymbolTable = &closureList[closureIndex].localSymbolTable;
        currentClosureIndex = closureIndex;

        // Handle the dependency list
        if (closureData.dependencies.size() > 0) {
            for (std::string name : closureData.dependencies) {
                Variable* dependency = findVariable(name);
                dependency->dependants.push_back(closureIndex);
                if (dependency->getType() == PIELO_CLOSURE) {
                    closureList[dependency->getClosureIndex()].dependants.push_back(closureIndex);
                }
                debugPrint(" added closure to dependencies of variable " << name << std::endl);
            }
        }

        programCounter = closureData.codePointer;
        debugPrint(" updated pc: " << programCounter << " state: " << state << std::endl);
    }

    void retFromClosure() {
        debugPrint(" ret_from_closure got pc " << programCounter << std::endl);
        if (stack.size() < 1) throw std::runtime_error("Stack empty before ret_from_closure");
        int hasReturn = stack.top().getIntValue();
        stack.pop();
        debugPrint(" ret_from_closure got hasReturn " << hasReturn << std::endl);
        if (hasReturn == 1) {
            debugPrint("ret_from_closure stack size " << stack.size() << std::endl);
            
        
            switch (stack.top().getType()) {
                case INT: closureList[currentClosureIndex].cachedValue = 
                    stack.top().getIntValue(); 
                    debugPrint(" placed " << stack.top().getIntValue() << " in cache for closure index " << currentClosureIndex << std::endl);
                    break;
                case FLOAT: closureList[currentClosureIndex].cachedValue = 
                    stack.top().getFloatValue(); 
                    debugPrint(" placed " << stack.top().getIntValue() << " in cache for closure index " << currentClosureIndex << std::endl);
                    break;
                case PIELO_CLOSURE: closureList[currentClosureIndex].cachedValue = 
                    stack.top().getClosureIndex(); 
                    debugPrint(" placed " << stack.top().getIntValue() << " in cache for closure index " << currentClosureIndex << std::endl);
                    break;
            }

        
            stack.pop();
        }

        stack.push(currentClosureIndex);

        programCounter = returnAddrStack.top().codePointer;
        currentSymbolTable = returnAddrStack.top().scopeSymbolTable;
        currentClosureIndex = returnAddrStack.top().closureIndex;
        returnAddrStack.pop();
    }

    void rerunClosure(size_t closureIndex) {
        debugPrint("Rerunning closure with index " << closureIndex << std::endl);
        returnAddrStack.push((scopeData){.scopeSymbolTable = currentSymbolTable, .codePointer = programCounter, .closureIndex = currentClosureIndex});
        currentSymbolTable = &closureList[closureIndex].localSymbolTable;
        currentClosureIndex = closureIndex;
        programCounter = closureList[closureIndex].codePointer;
        debugPrint(" pc now " << programCounter << std::endl);
    }

}