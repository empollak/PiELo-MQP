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

