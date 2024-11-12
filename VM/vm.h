#include <map>
#include <vector>
#include <string>
#include <stack>

namespace PiELo {
    enum Type {
        NIL,
        PIELO_CLOSURE,
        C_CLOSURE,
        FLOAT,
        INT
    };

    // Temporary typedef for however we store code
    typedef void* codePtr;

    struct ClosureData {
        // Pointer to code, however we decide to store that
        codePtr codePointer;
        std::map<std::string, Variable> localSymbolTable; // Should this be a pointer? Probably
        std::vector<std::string> argNames;
        std::vector<Type> argTypes;
    };

    struct Tag {

    };

    // All variables should be malloc'd?
    struct Variable {
        Type type = NIL;
        bool changed = 0;
        union {
            int asInt;
            float asFloat;
            ClosureData asClosure;
        };
        std::vector<Variable> dependencies;
        std::vector<Variable> dependants;
        std::vector<Tag> tags;
        Variable* cachedValue; // Pointer because a struct definition can't include itself
    };

    std::map<std::string, Variable> taggedTable;
    std::vector<ClosureData> closureList;
    std::stack<Variable> stack;
    std::stack<codePtr> returnAddrStack;
}