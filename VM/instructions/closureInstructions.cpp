#include "closureInstructions.h"
#include "../vm.h"
#include "storeLoad.h"
#include "../gc.h"

#ifdef __DEBUG_INSTRUCTIONS__
#define debugPrint(e) std::cout << e;
#else
#define debugPrint(e)
#endif

namespace PiELo{
    // Defines a closure
    void VM::defineClosure(std::string closureName, ClosureData closureData) {
        debugPrint("Defining closure with name " << closureName << ", code pointer: " << closureData.codePointer << std::endl)
        // Variable closureVar(closureData);
        // TODO: storeTagged with name closureName, value closureVar
        
        debugPrint("local symbol table size " << closureData.localSymbolTable.size() << std::endl)
        // closureList.push_back(closureData);
        closureData.isTemplate = true;
        closureList.push_back(closureData);
        // std::cout << "Pushed closure data to stack. index: " << closureList.getHeadOfList() - 1 << ". marked
        debugPrint("Pushing closure index to stack")
        stack.push((size_t) (closureList.getHeadOfList() - 1));
        debugPrint(" at closureTemplates index " << closureList.getHeadOfList() - 1 << std::endl);
        storeGlobal(closureName);

        // Register all the dependencies of a closure
        // for (const std::string& dep : closureData.dependencies) {
        //     Variable* depVar = findVariable(dep);
        //     GarbageCollector::regVar(depVar);
        // }

    }

    // Expects the stack to have format:
    // Bottom
    // arg1 
    // arg2 
    // number of args : int
    // ClosureData
    // top
    void VM::callClosure() {
        // Save the current scope
        debugPrint(" calling closure! " << std::endl);
        returnAddrStack.push((scopeData){.scopeSymbolTable = currentSymbolTable, .codePointer = programCounter, .closureIndex = currentClosureIndex});
        debugPrint("Pushed code pointer " << programCounter << " to return address stack" << std::endl)
        if (stack.size() < 2) throw ShortOnElementsOnStackException("call_closure");

        // Copy closure template from the variable on the stack
        ClosureData closureData = closureList[stack.top().getClosureIndex()];
        debugPrint(" updating closure list with symbol table size " << closureData.localSymbolTable.size() << std::endl);
        debugPrint(" called closure has code pointer " << closureData.codePointer << std::endl)
        size_t closureIndex = closureList.getHeadOfList();
        closureList.push_back(closureData);
        closureList[closureIndex].isTemplate = false;
        
        stack.pop();
        int numArgs = stack.top().getIntValue();
        stack.pop();
        if (stack.size() < numArgs) throw ShortOnElementsOnStackException("call_closure arguments");
        debugPrint(" Num args: " << numArgs << std::endl);

        if (numArgs != closureList[closureIndex].argNames.size()) throw std::runtime_error("Mismatched number of arguments for call_closure");

        // Top of stack now holds the first argument
        for (int i = 0; i < numArgs; i++) {
            // Copy the arguments to the local symbol table
            std::string argName = closureList[closureIndex].argNames[i];
            closureList[closureIndex].localSymbolTable[argName] = stack.top();
            debugPrint(" added arg name " << argName << " value ");
            closureList[closureIndex].localSymbolTable[argName].print();
            debugPrint("..." << std::endl);
            stack.pop();
        }

        // Update current local symbol table
        
        currentSymbolTable = &closureList[closureIndex].localSymbolTable;
        currentClosureIndex = closureIndex;

        #ifdef __DEBUG_INSTRUCTIONS__
            debugPrint(" new local symbol table has size " << currentSymbolTable->size() << " and variables: " << std::endl);
            for (auto it : *currentSymbolTable) {
                debugPrint("  " << it.first << ":");
                it.second.print();
                debugPrint(std::endl);
            }
        #endif

        // Handle the dependency list
        if (closureList[closureIndex].dependencies.size() > 0) {
            for (std::string name : closureList[closureIndex].dependencies) {
                // std::cout << "Looking for dependency " << name << std::endl;
                Variable* dependency = findVariable(name);
                dependency->dependants.push_back(closureIndex);
                if (dependency->getType() == PIELO_CLOSURE) {
                    closureList[dependency->getClosureIndex()].dependants.push_back(closureIndex);
                }
                debugPrint(" added closure to dependencies of variable " << name << std::endl);
            }
        }

        programCounter = closureList[closureIndex].codePointer;
        debugPrint(" updated pc: " << programCounter << " state: " << state << std::endl);
    }

    void VM::retFromClosure() {
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
                    debugPrint(" placed " << stack.top().getFloatValue() << " in cache for closure index " << currentClosureIndex << std::endl);
                    break;
                case PIELO_CLOSURE: closureList[currentClosureIndex].cachedValue = 
                    stack.top().getClosureIndex(); 
                    debugPrint(" placed " << stack.top().getClosureIndex() << " in cache for closure index " << currentClosureIndex << std::endl);
                    break;
            }

        
            stack.pop();
        }

        stack.push(currentClosureIndex);

        programCounter = returnAddrStack.top().codePointer;
        debugPrint("Got code pointer " << programCounter << " from return address stack" << std::endl;)
        currentSymbolTable = returnAddrStack.top().scopeSymbolTable;
        currentClosureIndex = returnAddrStack.top().closureIndex;
        returnAddrStack.pop();
    }

    void VM::rerunClosure(size_t closureIndex) {
        debugPrint("Rerunning closure with index " << closureIndex << std::endl);
        returnAddrStack.push((scopeData){.scopeSymbolTable = currentSymbolTable, .codePointer = programCounter, .closureIndex = currentClosureIndex});
        currentSymbolTable = &closureList[closureIndex].localSymbolTable;
        currentClosureIndex = closureIndex;
        programCounter = closureList[closureIndex].codePointer;
        debugPrint(" pc now " << programCounter << std::endl);
    }

    void VM::uncache() {
        if (stack.size() < 1) throw ShortOnElementsOnStackException("uncache");
        // Uncaching a non-cached value is fine.
        if (stack.top().getType() != Type::PIELO_CLOSURE) return;
        
        Variable var = stack.top();
        stack.pop();
        stack.push(closureList[var.getClosureIndex()].cachedValue);
        uncache();
    }

}