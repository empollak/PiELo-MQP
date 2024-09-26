#include "instructions.h"

// Pop from the stack into a place in memory identified by dest
void pop_store(std::string dest) {
    if (stack.size() < 1) {
        printf("Popped from empty stack!\n");
        exit(-1);
    }
    heap[dest] = stack.top();
    stack.pop();
}


void push_from_heap(std::string src) {
    auto e = heap[src];
    if (e.size() < 1) {
        printf("Tried to access invalid string %s from heap\n", src.c_str());
        exit(-1);
    }
    stack.push(e);
}

void print_top() {
    for (auto it = stack.top().begin(); it != stack.top().end(); it++) {
        printf("%x ", *it);
    }
    printf("\n");
}

void call(std::string funcName) {
    // Store the location of the next instruction on the stack
    push(code.tellg());

    // Coerce the data from heap to be a location
    std::streampos newLoc = *((std::streampos*) heap[funcName].data());

    code.seekg(newLoc);
}

// Pop the top of the stack, treat it as a streampos, and go there
void ret() {
    std::streampos newLoc = *((std::streampos*) stack.top().data());
    stack.pop();
    code.seekg(newLoc);
}