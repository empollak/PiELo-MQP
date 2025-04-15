#include "storeLoad.h"
#include "../vm.h"
#include "../instructionHandler.h"
#include "../networking.h"
#include <sys/time.h>
#include "../gc.h"
#ifdef __DEBUG_INSTRUCTIONS__
#define debugPrint(e) logfile << e;
#else
#define debugPrint(e)
#endif

namespace PiELo {
    // Clear the variable from the given closure's list of dependencies
    void VM::removeClosureDependantForVariable(Variable* v, std::string varName) {
        if (v->getType() == PIELO_CLOSURE && (v->isStigmergy || !closureList[v->getClosureIndex()].isTemplate)) 
            if (!v->isStigmergy)
                closureList.at(v->getClosureIndex()).dependantVariables.erase(varName);
            else
            closureList.at(v->stigmergyData[robotID].asClosureIndex).dependantVariables.erase(varName);
    }

    // Update the given closure's list of dependant variables
    void VM::addClosureDependantForVariable(Variable* v, std::string varName) {
        if (v->getType() == PIELO_CLOSURE && (v->isStigmergy || !closureList[v->getClosureIndex()].isTemplate)) {
            if (!v->isStigmergy) {
                closureList.at(v->getClosureIndex()).dependantVariables[varName] = 1;
                debugPrint("Added " << varName << " as dependant for closure index " << v->getClosureIndex() 
                    << ". dependantVariables has size " << closureList.at(v->getClosureIndex()).dependantVariables.size() << std::endl)
            }
            else {
                closureList.at(v->stigmergyData[robotID].asClosureIndex).dependantVariables[varName] = 1;
                debugPrint("Added " << varName << " as dependant for closure index " << v->stigmergyData[robotID].asClosureIndex << std::endl)
            }
            
        }
    }

    void VM::storeLocal(std::string varName){
        if (stack.empty()){
            throw std::runtime_error("Stack underflow: storeLocal");
        }

        try {
            // This will throw an error if varName is not found
            Variable* var = &currentSymbolTable->at(varName);

            // Remove this variable from the old closure dependant, in case it was storing that
            removeClosureDependantForVariable(var, varName);

            // Mutate the value
            var->mutateValue(stack.top());

            // Add the variable to the closure contained in its new value, if necessary
            addClosureDependantForVariable(var, varName);
            
            handleDependants(*var, true);
        } catch (...) {
            (*currentSymbolTable)[varName] = stack.top();
            // Add the variable to the closure contained in its new value, if necessary
            addClosureDependantForVariable(&(*currentSymbolTable)[varName], varName);
        }

        stack.pop();

        // std::string varName = *(var.getNameValue());
        debugPrint("Stored local to " << varName << std::endl);
    }

    void VM::storeGlobal(std::string varName){
        if (stack.empty()){
            throw std::runtime_error("Stack underflow: storeLocal");
        }

        try {
            // This will throw an error if varName is not found
            Variable* var = &globalSymbolTable.at(varName);

            // Remove this variable from the old closure dependant, in case it was storing that
            removeClosureDependantForVariable(var, varName);

            // Mutate the value
            var->mutateValue(stack.top());

            // Add the variable to the closure contained in its new value, if necessary
            addClosureDependantForVariable(var, varName);

            handleDependants(*var, true);
        } catch (...) {
            (globalSymbolTable)[varName] = stack.top();
            // Add the variable to the closure contained in its new value, if necessary
            addClosureDependantForVariable(&globalSymbolTable[varName], varName);
        }

        stack.pop();

        // std::string varName = *(var.getNameValue());
        debugPrint("Stored global to " << varName << ", with type " << globalSymbolTable[varName].getTypeAsString() << std::endl);
        if(globalSymbolTable[varName].getType() == PIELO_CLOSURE) 
            debugPrint("    closure index: " << globalSymbolTable[varName].getClosureIndex()
                << ", cached value has type: " << closureList[globalSymbolTable[varName].getClosureIndex()].cachedValue.getTypeAsString() << std::endl);
    }

    void VM::loadToStack(const std::string& varName){
        debugPrint("Beginning loadToStack " << std::endl);
        Variable* var = nullptr;
        

        // search local sym table
        #ifdef __DEBUG_INSTRUCTIONS__
            debugPrint("Searching current symbol table for " << varName << std::endl);
            debugPrint(" symbol table has: " << std::endl);
            for (auto it : *currentSymbolTable) {
                debugPrint("  " << it.first << ":");
                it.second.print();
                debugPrint(std::endl);
            }
        #endif

        auto local = currentSymbolTable -> find(varName);
        if (local != currentSymbolTable -> end()) {
            debugPrint("Found it! " << std::endl);
            var = &(local -> second);
        }

        local = globalSymbolTable.find(varName);
        if (local != globalSymbolTable.end()) {
            debugPrint("Found it! " << std::endl);
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
        #ifdef __DEBUG_INSTRUCTIONS__
        debugPrint("load result: ");
        debugPrint("Stack top: ");
        stack.top().print();
        debugPrint(std::endl);
        #endif
    }

    void VM::tagVariable(const std::string& varName, const std::string& tagName) {
        Variable* var = nullptr;
        
        debugPrint("Tagging variable " << varName << " with tag " << tagName << std::endl);
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

    void VM::tagRobot(const std::string& tagName) {
        
        // check if tag already exists
        for (const auto& tag : robotTagList){
            if (tag.tagName == tagName){
                // no need to add if it exists
                return;
            }
        }

        robotTagList.push_back(Tag{tagName});
    }

    // Takes in a closure index and does what it says it will do
    void VM::recursivelyAddDependantsOfClosureToReturnAddrStack(size_t closureIndex, std::stack<scopeData> &dependants) {
        // Push the closure itself to the return address stack
        if (closureList.find(closureIndex) == closureList.end()) {
            debugPrint("Skipping dependant " << closureIndex << ". " << std::endl);
            return;
        }
        ClosureData* closure = &closureList[closureIndex];
        dependants.push((scopeData) {.scopeSymbolTable = &closure->localSymbolTable, .codePointer = closure->codePointer, .closureIndex = closureIndex});
        debugPrint(closureIndex << ", ");
        // Push each of its dependants to the dependants stack, recursively
        for (size_t depIndex : closure->dependants) {
            recursivelyAddDependantsOfClosureToReturnAddrStack(depIndex, dependants);
        }
    }

    void VM::handleDependants(Variable& var, bool storeCurrentScope) {
        if (var.dependants.size() > 0) {
            // Store where we currently are
            debugPrint(" Variable has closure index dependants: ");
            if (storeCurrentScope) {
                returnAddrStack.push((scopeData){.scopeSymbolTable = currentSymbolTable, .codePointer = programCounter, .closureIndex = currentClosureIndex});
            } else {
                // Store a bogus scope so that retFromClosure knows to skip it
                returnAddrStack.push((scopeData){.scopeSymbolTable = currentSymbolTable, .codePointer = (codePtr) -2, .closureIndex = currentClosureIndex});
            }
            
            std::stack<scopeData> dependants;
            for (int i = 0; i < var.dependants.size(); i++) {
                // Erase dependants which have been garbage collected
                if (closureList.find(var.dependants[i]) == closureList.end()) {
                    debugPrint("Skipping dependant " << var.dependants[i] << ". " << std::endl)
                    continue;
                }
                // Push the info of each dependant closure and all of its dependants to the dependants stack
                recursivelyAddDependantsOfClosureToReturnAddrStack(var.dependants[i], dependants);
            }

            // Reverse the order of the found dependants. This ensures that parent nodes are updated before their children
            size_t numDeps = dependants.size();
            for (size_t i = 0; i < numDeps; i++) {
                returnAddrStack.push(dependants.top());
                dependants.pop();
            }
            debugPrint(std::endl);

            // Go to the first closure in the list
            if (storeCurrentScope) {
                // If we are not storing the current scope, that means we are being called from a closure return
                // That closure return will handle going to the new address.
                programCounter = returnAddrStack.top().codePointer;
                currentSymbolTable = returnAddrStack.top().scopeSymbolTable;
                currentClosureIndex = returnAddrStack.top().closureIndex;
                debugPrint("handleDependants set currentClosureIndex to " << currentClosureIndex << std::endl)
                returnAddrStack.pop();
                debugPrint(" Jumped to PC " << programCounter << std::endl);
            }
        }
    }

    void VM::storeTagged(const std::string& varName){
        if (stack.empty()){
            throw std::runtime_error("Stack underflow: storeTagged");
        }
        debugPrint("Storing variable " << varName << std::endl);
        try {
            // This will throw an error if varName is not found
            Variable* var = &taggedTable.at(varName);
            

            // Remove this variable from the old closure dependant, in case it was storing that
            removeClosureDependantForVariable(var, varName);

            // Mutate the value
            var->mutateValue(stack.top());

            // Add the variable to the closure contained in its new value, if necessary
            addClosureDependantForVariable(var, varName);

            gettimeofday(&(var->lastUpdated), NULL);

            handleDependants(*var, true);
        } catch (...) {
            // Create a new entry in the tagged table
            taggedTable[varName] = stack.top();
            gettimeofday(&taggedTable[varName].lastUpdated, NULL);
            // Add the variable to the closure contained in its new value, if necessary
            addClosureDependantForVariable(&taggedTable[varName], varName);
        }

        // Can assume that the variable now must exist in the tagged table
        network.broadcastVariable(varName, taggedTable[varName].getVariableData());
        stack.pop();
    }

    void VM::storeStig(const std::string& varName) {
        if (stack.empty()){
            throw std::runtime_error("Stack underflow: storeStig");
        }
        
        auto it = taggedTable.find(varName);
        if (it != taggedTable.end()) {
            // Variable was found in the taggedTable
            Variable* var = &(*it).second;

            // Remove this variable from the old closure dependant, in case it was storing that
            removeClosureDependantForVariable(var, varName);

            var->updateStigValue(robotID, stack.top());
            var->resetIter();
            gettimeofday(&(var->lastUpdated), NULL);

            // Add the variable to the closure contained in its new value, if necessary
            addClosureDependantForVariable(var, varName);

            handleDependants(*var, true);
        } else {
            // Variable does not exist yet
            Variable empty;
            taggedTable[varName] = empty;
            taggedTable[varName].isStigmergy = true;
            taggedTable[varName].updateStigValue(robotID, stack.top());
            debugPrint("Store stig pushed type " << taggedTable[varName].getTypeAsString() << " value " << taggedTable[varName].stigmergyData[robotID].asInt << std::endl)
            taggedTable[varName].resetIter();
            debugPrint("   Iter type " << taggedTable[varName].peekIterValue().getTypeAsString() << " int value " << taggedTable[varName].peekIterValue().asInt << std::endl)
            gettimeofday(&taggedTable[varName].lastUpdated, NULL);
            // Add the variable to the closure contained in its new value, if necessary
            addClosureDependantForVariable(&taggedTable[varName], varName);
        }

        // Can assume that the variable now must exist in the tagged table
        network.broadcastVariable(varName, taggedTable[varName]);
        stack.pop();
    }

    void VM::pushNextElementOfStig(const std::string& varName) {
        auto it = taggedTable.find(varName);
        if (it == taggedTable.end()) {
            throw std::runtime_error("Attempted to push next element of non-existent stig variable " + varName);
        }
        Variable* var = &(*it).second;
        
        var->ensureStig();
        VariableData data = var->nextIterValue();
        debugPrint("pushNextElement of stig got type " << data.getTypeAsString() << " int value " << data.asInt << std::endl)
        stack.push(data);
    }

    void VM::isIterAtEnd(const std::string& varName) {
        auto it = taggedTable.find(varName);
        if (it == taggedTable.end()) {
            throw std::runtime_error("Attempted to check iter status of non-stig variable " + varName);
        }
        Variable* var = &(*it).second;
        
        var->ensureStig();
        bool retVal = var->isIterAtEnd();
        debugPrint("isIterAtEnd returned " << retVal << std::endl);
        stack.push(retVal);
    }

    void VM::resetIter(const std::string& varName) {
        auto it = taggedTable.find(varName);
        if (it == taggedTable.end()) {
            throw std::runtime_error("Attempted to check iter status of non-stig variable " + varName);
        }
        Variable* var = &(*it).second;
        var->ensureStig();
        var->resetIter();
    }
}