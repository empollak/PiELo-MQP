#include "gc.h"
#include "vm.h"
#include <iostream>
#include <algorithm>
#ifdef __DEBUG_INSTRUCTIONS__
#define debugPrint(e) vm->logfile << e;
#else
#define debugPrint(e)
#endif

namespace PiELo {
    // mark one var
    void GarbageCollector::markVar(size_t closureIndex){
        // Avoid marking the global context
        if (closureIndex == (size_t) -1) {
            debugPrint("Attempted to mark top-level" << std::endl);
            return;
        }

        // look for entry in gcelement and mark it
        ClosureData* closure;
        try {
            closure = &vm->closureList.at(closureIndex);
        } catch (std::out_of_range e) {
            throw std::out_of_range(std::string("closureList.at(") +  std::to_string(closureIndex) + ")");
        }
        
        if (closure->marked) return;

        closure->marked = true;

        if (closure->isTemplate) return;
        // if (closureIndex >= vm->closureList.getHeadOfList()) {
            debugPrint("Closure at index " << closureIndex << " has pc " << vm->closureList[closureIndex].codePointer << "\n");
        // }
        debugPrint("Symbol Table:" << std::endl)
        // symbol table
        for (auto &pair: closure->localSymbolTable){
            if (pair.second.getType() == PIELO_CLOSURE) {
                markVar(pair.second.getClosureIndex());
            }
        }
        
        debugPrint("Dependencies: " << std::endl);
        // Mark all dependencies
        for (auto varName : closure->dependencies) {
            Variable* var = vm->findVariable(varName);
            if (!var->isStigmergy && var->getType() == PIELO_CLOSURE) markVar(var->getClosureIndex());
            else if (var->isStigmergy && var->stigmergyData[vm->robotID].getType() == PIELO_CLOSURE) {
                markVar(var->stigmergyData[vm->robotID].asClosureIndex);
            }
        }
        
        if (closure->cachedValue.getType() == PIELO_CLOSURE) markVar(closure->cachedValue.asClosureIndex);
    }

    // mark all roots
    void GarbageCollector::markRoots(){
        // Mark the closure we're currently in
        debugPrint("\n------ Marking current closure.\n");
        markVar(vm->currentClosureIndex);
        debugPrint("\n------ Marking global symbol table.\n");
        // global symbol table
        for (auto &pair : vm->globalSymbolTable){
            if (pair.second.getType() == PIELO_CLOSURE)
                markVar(pair.second.getClosureIndex());
        }

        debugPrint("\n------ Marking tagged table.\n");
        // tagged table
        for (auto &pair : vm->taggedTable){
            debugPrint("Looking at type " << pair.second.getTypeAsString() << std::endl)
            if (pair.second.isStigmergy) {
                debugPrint("\tIt's a stigmergy! The data for me (id " << vm->robotID << ") has type " << pair.second.stigmergyData[vm->robotID].getTypeAsString() << std::endl)
                if (pair.second.stigmergyData[vm->robotID].getType() == PIELO_CLOSURE)
                    markVar(pair.second.stigmergyData[vm->robotID].asClosureIndex);
            } else {
                if (pair.second.getType() == PIELO_CLOSURE)
                    markVar(pair.second.getClosureIndex());
            }
        }
        
        debugPrint("\n------ Marking stack.\n");
        // call stack
        std::stack<Variable> tempStack = vm->stack;
        while (!tempStack.empty()) {
            if (tempStack.top().getType() == PIELO_CLOSURE) 
                markVar(tempStack.top().getClosureIndex());
            tempStack.pop();
        }

        debugPrint("\n------ Marking local symbol tables.\n");
        // local sym tables
        for (auto &closurePair : vm->closureList) {
            if (closurePair.second.marked) {
                for (auto &localPair : closurePair.second.localSymbolTable) {
                    if (localPair.second.getType() == PIELO_CLOSURE)
                        markVar(localPair.second.getClosureIndex());
                }
            }
            
        }

        debugPrint("\n------ Marking return address stack.\n");
        // Mark all closures in the return address stack
        std::stack<VM::scopeData> tempReturnStack = vm->returnAddrStack;
        while (!tempReturnStack.empty()) {
            markVar(tempReturnStack.top().closureIndex);
            tempReturnStack.pop();
        }
    }

    // sweep through gc heap and free unmarked
    void GarbageCollector::sweep() {

        // std::cout << "sweeping" << std::endl;
        for (std::map<size_t, ClosureData>::iterator it = vm->closureList.begin(); it != vm->closureList.end();) {
            // std::cout << "Looking at closure index " << it->first << std::endl;
            // Erase if not marked and move to the next item (erase does ++ for you)
            if (!it->second.marked) {
                // std::cout << "erasing closure index" << it->first << std::endl;
                it = vm->closureList.erase(it);
            }
            else {
                it->second.marked = false;
                it++;
            }
        }
    }

    // run full gc cycle
    void GarbageCollector::collectGarbage(VM* vmPtr) {
        vm = vmPtr;
        debugPrint("GC: Starting Collection. vm->closureList size = " << vmPtr->closureList.size() << std::endl)
        markRoots();
        sweep();
        debugPrint("GC  Collection complete. vm->closureList size = " << vmPtr->closureList.size() << std::endl)
    }

    size_t GarbageCollector::heapSize(){
        return vm->closureList.size();
    }

}
