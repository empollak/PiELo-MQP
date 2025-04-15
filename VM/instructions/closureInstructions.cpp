#include "closureInstructions.h"
#include "../vm.h"
#include "storeLoad.h"
#include "../gc.h"
#ifdef __DEBUG_INSTRUCTIONS__
#define debugPrint(e) logfile << e;
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
        debugPrint("Pushed code pointer " << programCounter << ", closureIndex " << currentClosureIndex << " to return address stack" << std::endl)
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
        debugPrint("callClosure set currentClosureIndex to " << currentClosureIndex << std::endl);

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
                if (!dependency->isStigmergy && dependency->getType() == PIELO_CLOSURE) {
                    closureList[dependency->getClosureIndex()].dependants.push_back(closureIndex);
                } else if (dependency->isStigmergy && dependency->stigmergyData[robotID].getType() == PIELO_CLOSURE) {
                    closureList[dependency->stigmergyData[robotID].asClosureIndex].dependants.push_back(closureIndex);
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
            // Pop the closure's return value
            stack.pop();
            

            // Update all dependant variables with the new closure data
            // This prompts all dependants of that variable to update and the variable to be broadcast to the network, if necessary
            for (std::pair<std::string, int> varPair : closureList[currentClosureIndex].dependantVariables) {
                std::string varName = varPair.first;
                debugPrint(" Re-storing variable " << varName << std::endl);
                enum {GLOBAL, TAGGED, NOT_FOUND} foundVariable = NOT_FOUND;
                Variable dependant;
                try {
                    dependant = globalSymbolTable.at(varName);
                    foundVariable = GLOBAL;
                    debugPrint(" Found it in the global table." << std::endl);
                } catch (...) {}
                
                try {
                    if (foundVariable == NOT_FOUND) {
                        dependant = taggedTable.at(varName);
                        foundVariable = TAGGED;
                        debugPrint(" Found it in the tagged table." << std::endl);
                    }
                } catch (...) {}
                
                if (foundVariable == NOT_FOUND)
                    throw std::runtime_error("Tried to update dependant variable \"" + varName + 
                        "\" of closure index " + std::to_string(currentClosureIndex) + 
                        " but could not find it.");

                if ((!dependant.isStigmergy && dependant.getType() != PIELO_CLOSURE) 
                    || (dependant.isStigmergy && dependant.stigmergyData[robotID].getType() != PIELO_CLOSURE))
                        throw std::runtime_error("Tried to update dependant variable \"" + varName + 
                            "\" of closure index " + std::to_string(currentClosureIndex) + 
                            " but it had type " + dependant.getTypeAsString() + ". Expected a PIELO_CLOSURE");
                
                if ((!dependant.isStigmergy && dependant.getClosureIndex() != currentClosureIndex)
                    || dependant.isStigmergy && dependant.stigmergyData[robotID].asClosureIndex != currentClosureIndex)
                        throw std::runtime_error("Tried to update dependant variable \"" + varName + 
                            "\" of closure index " + std::to_string(currentClosureIndex) + 
                            " but it was storing the wrong closure index. It was storing closure index " + std::to_string(dependant.getClosureIndex()));
                
                // stack.push(currentClosureIndex);
                
                if (dependant.isStigmergy) {
                    Variable* var = &taggedTable[varName];
                    // Remove this variable from the old closure dependant, in case it was storing that
                    removeClosureDependantForVariable(var, varName);

                    var->resetIter();
                    gettimeofday(&(var->lastUpdated), NULL);

                    // Add the variable to the closure contained in its new value, if necessary
                    addClosureDependantForVariable(var, varName);

                    
                    handleDependants(taggedTable[varName], false);
                    network.broadcastVariable(varName, taggedTable[varName]);
                }
                else if (foundVariable == GLOBAL) {
                    handleDependants(globalSymbolTable[varName], false);
                }
                else {
                    // This will throw an error if varName is not found
                    Variable* var = &taggedTable.at(varName);
            
                    gettimeofday(&(var->lastUpdated), NULL);

                    handleDependants(*var, false);
                    network.broadcastVariable(varName, taggedTable[varName].getVariableData());
                }
            }
        }
        
        // If there is a dependant variable, this closure has been called before
        // Otherwise, it has never been called before and we should do all the normal return stuff.
        if (closureList[currentClosureIndex].dependantVariables.size() == 0) {
            stack.push(currentClosureIndex);
        }

        // Pop entries from the return address stack until you find a valid one.
        while((programCounter = returnAddrStack.top().codePointer) == -2) returnAddrStack.pop();
        debugPrint("Got code pointer " << programCounter << " from return address stack" << std::endl;)
        currentSymbolTable = returnAddrStack.top().scopeSymbolTable;
        currentClosureIndex = returnAddrStack.top().closureIndex;
        debugPrint("retFromClosure set currentClosureIndex to " << currentClosureIndex << std::endl)
        returnAddrStack.pop();
    }

    void VM::rerunClosure(size_t closureIndex) {
        debugPrint("Rerunning closure with index " << closureIndex << std::endl);
        returnAddrStack.push((scopeData){.scopeSymbolTable = currentSymbolTable, .codePointer = programCounter, .closureIndex = currentClosureIndex});
        currentSymbolTable = &closureList[closureIndex].localSymbolTable;
        currentClosureIndex = closureIndex;
        debugPrint("rerunClosure set currentClosureIndex to " << currentClosureIndex << std::endl)
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