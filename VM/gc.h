#pragma once
#include "vm.h"
#include <vector>
#include <stack>
#include <map>
#include <string>

namespace PiELo {
    
    // wraps a pointer to variable and a mark flag
    struct GCElement {
        Variable* var;
        bool marked;
    };

    class GarbageCollector {
    public:
        // call whenever a new variable is allocated
        static void regVar(Variable* var);

        // mark all reachable variables
        static void markRoots();
        
        // mark single var (marks nested references recursively)
        static void markVar(Variable* var);

        // free unmarked variables and reset marks
        static void sweep();

        // run gc cycle
        static void collectGarbage();

        // might be useful for debugging (?)
        static size_t heapSize();

    private:
        static std::vector<GCElement*> gcHeap;
    
    };

}
