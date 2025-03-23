#include "gc.h"
#include "vm.h"
#include <iostream>
#include <algorithm>

namespace PiELo {
    // mark one var
    void GarbageCollector::markVar(size_t closureIndex){
        // look for entry in gcelement and mark it
        ClosureData* closure = &closureList.at(closureIndex);
        closure->marked = true;
        // symbol table
        for (auto &pair: closure->localSymbolTable){
            if (pair.second.getType() == PIELO_CLOSURE) {
                markVar(pair.second.getClosureIndex());
            }
        }

        // Mark all dependencies
        for (auto varName : closure->dependencies) {
            Variable* var = findVariable(varName);
            if (var->getType() == PIELO_CLOSURE) markVar(var->getClosureIndex());
        }
        
        if (closure->cachedValue.getType() == PIELO_CLOSURE) markVar(closure->cachedValue.asClosureIndex);
    }

    // mark all roots
    void GarbageCollector::markRoots(){
        // Mark the closure we're currently in
        markVar(currentClosureIndex);

        // global symbol table
        for (auto &pair : globalSymbolTable){
            if (pair.second.getType() == PIELO_CLOSURE)
                markVar(pair.second.getClosureIndex());
        }

        // tagged table
        for (auto &pair : taggedTable){
            if (pair.second.getType() == PIELO_CLOSURE)
                markVar(pair.second.getClosureIndex());
        }

        // call stack
        std::stack<Variable> tempStack = stack;
        while (!tempStack.empty()) {
            if (tempStack.top().getType() == PIELO_CLOSURE) 
                markVar(tempStack.top().getClosureIndex());
            tempStack.pop();
        }

        // local sym tables
        for (auto &closurePair : closureList) {
            if (closurePair.second.marked) {
                for (auto &localPair : closurePair.second.localSymbolTable) {
                    if (localPair.second.getType() == PIELO_CLOSURE)
                        markVar(localPair.second.getClosureIndex());
                }
            }
            
        }

        // Mark all closures in the return address stack
        std::stack<scopeData> tempReturnStack = returnAddrStack;
        while (!tempReturnStack.empty()) {
            markVar(tempReturnStack.top().closureIndex);
            tempReturnStack.pop();
        }
    }

    // sweep through gc heap and free unmarked
    void GarbageCollector::sweep() {

        // std::cout << "sweeping" << std::endl;
        for (std::map<size_t, ClosureData>::iterator it = closureList.begin(); it != closureList.end();) {
            // std::cout << "Looking at closure index " << it->first << std::endl;
            // Erase if not marked and move to the next item (erase does ++ for you)
            if (!it->second.marked) {
                // std::cout << "erasing closure index" << it->first << std::endl;
                it = closureList.erase(it);
            }
            else {
                it->second.marked = false;
                it++;
            }
        }
    }

    // run full gc cycle
    void GarbageCollector::collectGarbage() {
        std::cout << "GC: Starting Collection. closureList size = " << closureList.size() << std::endl;
        markRoots();
        sweep();
        std::cout << "GC  Collection complete. closureList size = " << closureList.size() << std::endl;
    }

    size_t GarbageCollector::heapSize(){
        return closureList.size();
    }

}
