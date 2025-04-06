#ifndef GC_H
#define GC_H
#pragma once
#include <vector>
#include <stack>
#include <map>
#include <string>

namespace PiELo {
    class VM;
    class GarbageCollector {
        VM* vm;
    public:

    
        // mark all reachable variables
        void markRoots();
        
        // mark single var (marks nested references recursively)
        void markVar(size_t closureIndex);

        // free unmarked variables and reset marks
        void sweep();

        // run gc cycle
        void collectGarbage(VM* vmPtr);

        // might be useful for debugging (?)
        size_t heapSize();
    };

}

#endif // GC_H