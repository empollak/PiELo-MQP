#pragma once
#include <iostream>
#include "vm.h"
#include <vector>

enum Instructions {
    PUSH,
    POP,
    ADD,
    SUB
};

void ret();
void call(std::string funcName);

template <class T>
void push(T e) {
    std::vector<uint8_t> stackElement;
    std::cout << "pushing " << e << " to stack\n";
    stackElement.resize(sizeof(T));
    *((T*) stackElement.data()) = e;
    // printf("Pushed: ");
    // for (auto it = stackElement.begin(); it != stackElement.end(); it++) {
    //     printf("%x ", *it);
    // }
    // printf("\n");
    stack.push(stackElement);
}

// Push an element to the heap
template <class T>
void push_to_heap(T e, std::string dest) {
    std::vector<uint8_t> heapElement;
    heapElement.resize(sizeof(T));
    *((T*) heapElement.data()) = e;
    // printf("Pushed to heap: ");
    // for (auto it = heapElement.begin(); it != heapElement.end(); it++) {
    //     printf("%x ", *it);
    // }
    // printf("\n");
    heap[dest] = heapElement;
}

// Pop the top element from the stack and store it in dest in the heap
void pop_store(std::string dest);

// Grab an element from the heap and push it onto the stack
void push_from_heap(std::string src);

// Print the top of the stack
void print_top();

// Add the top two numbers of the stack together
// Pops both numbers off the stack and then pushes the result
// Types are for top of stack, next top of stack.
template <class T0, class T1>
void add() {
    if (stack.size() < 2) {
        printf("Attempted to add with only %ld elements in stack\n", stack.size());
        exit(-1);
    }
    // Interpret the vector as however many bytes are in T0
    T0* op0ptr = (T0*) stack.top().data();
    T0 op0 = *op0ptr;
    stack.pop();

    T1* op1ptr = (T1*) stack.top().data();
    T1 op1 = *op1ptr;
    stack.pop();
    // std::cout << "op0 " << op0 << " op1 " << op1 << std::endl;

    push(op0 + op1);
}
#define add_ints() add<int, int>()
#define add_doubles() add<double, double>()

// Subtract the top of the stack from the next element on the stack
// Pops both numbers off the stack and then pushes the result
// push(1) push(2) sub() = 1-2 = -1
template <class T0, class T1>
void sub() {
    if (stack.size() < 2) {
        printf("Attempted to subtract with only %ld elements in stack\n", stack.size());
        exit(-1);
    }
    // Interpret the vector as however many bytes are in T0
    T0* op0ptr = (T0*) stack.top().data();
    T0 op0 = *op0ptr;
    stack.pop();

    T1* op1ptr = (T1*) stack.top().data();
    T1 op1 = *op1ptr;
    stack.pop();
    // std::cout << "op0 " << op0 << " op1 " << op1 << std::endl;

    push(op1 - op0);
}
#define sub_ints() sub<int, int>()
#define sub_doubles() sub<double, double>()

template <class T>
void print_with_type() {
    T e = *((T*) stack.top().data());
    std::cout << e << std::endl;
}

// Compares the top two elements of the stack.
// Returns 0 if they are equal, 1 if the top is greater than the next, -1 otherwise
// Pushes result to stack. Does not pop the compared values
// Types are top of stack, next on stack
template<class T0, class T1>
void cmp() {
    if (stack.size() < 2) {
        printf("Attempted to compare with only %ld elements in stack\n", stack.size());
        exit(-1);
    }
    // Interpret the vector as however many bytes are in T0
    T0* op0ptr = (T0*) stack.top().data();
    T0 op0 = *op0ptr;
    stack.pop();

    T1* op1ptr = (T1*) stack.top().data();
    T1 op1 = *op1ptr;
    std::cout << "cmp: op0: " << op0 << " op1 " << op1 << std::endl;

    // Put the stuff back on the stack, in order.
    push(op0);
    push((int) (op0 - op1));
}

void jump_if_greater(std::string label);
void jump_if_less(std::string label);
void jump(std::string label);