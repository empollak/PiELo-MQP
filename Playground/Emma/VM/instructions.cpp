#include "instructions.h"

using namespace PiELo;
std::map<std::string, Value> heap;
std::stack<Value> stack;

namespace PiELo {
    void pop() {
        if (stack.size() == 0) {
            printf("Attempted to pop with no elements on stack\n");
            exit(-1);
        }

        stack.pop();
    }

    // Pop from the stack into a place in memory identified by dest
    void pop_store(std::string dest) {
        if (stack.size() < 1) {
            printf("Popped from empty stack!\n");
            exit(-1);
        }
        heap[dest] = stack.top();
        stack.pop();
    }

    void store(Value e, std::string dest) {
        // printf("Pushed to heap: ");
        // for (auto it = heapElement.begin(); it != heapElement.end(); it++) {
        //     printf("%x ", *it);
        // }
        // printf("\n");
        heap[dest] = e;
    }

    void push_from_heap(std::string src) {
        Value e = heap[src];
        if (e.type == NIL || e.value.size() < 1) {
            printf("Tried to access nonexistent key %s from heap, or it was NIL\n", src.c_str());
            exit(-1);
        }
        stack.push(e);
    }

    void push(Value e) {
        stack.push(e);
    }

    void add() {
        Value op0 = stack.top();
        stack.pop();
        Value op1 = stack.top();
        stack.pop();

        stack.push(op0 + op1);
    }

    void sub() {
        Value op0 = stack.top();
        stack.pop();
        Value op1 = stack.top();
        stack.pop();

        stack.push(op1 - op0);
    }

    void print() {
        Value e = stack.top();
        switch (e.type) {
        case ValueType_e::DOUBLE:
            std::cout << e.as<double>() << std::endl;
            break;
        case ValueType_e::INT32:
            std::cout << e.as<int32_t>() << std::endl;
            break;
        case ValueType_e::STRING:
            std::cout << ((char*) e.value.data()) << std::endl;
            break;
        default:
            std::cout << "Bad type for printing! " << e.type << std::endl;
            break;
        }
    }

    void cmp() {
        if (stack.size() < 2) {
            printf("Attempted to compare with only %ld elements in stack\n", stack.size());
            exit(-1);
        }

        // Store the operands to be put back later
        Value op0 = stack.top();
        stack.pop();
        Value op1 = stack.top();
        stack.pop();

        // Place everything back on the stack in the correct order
        stack.push(op1);
        stack.push(op0);
        stack.push(op0 - op1);
    }

    // Intended to be called after cmp
    // Jump if top value of stack is > 0 (before cmp, top was greater than next)
    // Pops top of stack
    // Modifies the pc to be correct
    void jump_if_greater(std::string label, int32_t& pc) {
        int32_t cmpResult = stack.top().as<int32_t>();
        stack.pop();
        if (cmpResult > 0) {
            printf("Jumped to %s, cmpResult was %d\n", label.c_str(), cmpResult);
            jump(label, pc);
        }
    }

    // Intended to be called after cmp
    // Jump if top value of stack is < 0 (before cmp, top was greater than next)
    // Pops top of stack
    // Returns what the new pc should be, or -1 if the jump is unsuccessful
    void jump_if_less(std::string label, int32_t& pc) {
        int cmpResult = stack.top().as<int32_t>();
        stack.pop();
        if (cmpResult < 0) {
            printf("Jumped to %s, cmpResult was %d\n", label.c_str(), cmpResult);
            jump(label, pc);
        }
    }

    // Returns what the new PC should be
    void jump(std::string label, int32_t& pc) {
        int32_t newLoc = heap[label].as<int32_t>();
        pc = newLoc;
    }

    void call(std::string funcName, int32_t& pc) {
        // Store the location of the next instruction on the stack
        stack.push(pc);

        // Coerce the data from heap to be a location
        pc = heap[funcName].as<int32_t>();
    }

    // Pop the top of the stack, modify the pc to be equal to that
    void ret(int32_t& pc) {
        int32_t newLoc = stack.top().as<int32_t>();
        stack.pop();
        pc = newLoc;
    }
}