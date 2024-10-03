#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <stack>
#include <fstream>
#include "parser.h"


namespace PiELo {

    enum Instructions_e {
        PUSHI,
        PUSHD,
        PUSHS, // Push string
        PUSH_FROM_HEAP, // Push to the stack from the heap
        POP,
        POP_STORE,
        STOREI, // Stores are given in "store value dest"
        STORED,
        ADD,
        SUB,
        PRINT,
        CMP,
        JMPG,
        JMPL,
        JMP,
        RET,
        CALL,
        EXIT
    };

    enum ValueType_e {
        NIL,
        INT32,
        DOUBLE,
        STRING,
        STREAMPOS
    };

    class Value {
    private:
        template<class T>
        void setValue(T value) {
            this->value.resize(sizeof(T));
            *((T*) this->value.data()) = value;
        }
    public:
        ValueType_e type;
        std::vector<uint8_t> value;

        Value() {
            type = NIL;
        }

        template <class T>
        T as() { return *((T*) this->value.data()); };

        // friends defined inside class body are inline and are hidden from non-ADL lookup
        friend Value operator+(Value lhs,        // passing lhs by value helps optimize chained a+b+c
            Value rhs) // otherwise, both parameters may be const references
        {
            Value result;
            if (lhs.type == DOUBLE || rhs.type == DOUBLE) {
                // Convert both to doubles and do the addition
                double op0Value;
                double op1Value;
                if (lhs.type != DOUBLE) {
                    if (lhs.type == ValueType_e::INT32) {
                        // Cast value to int32 pointer so that it can then be reinterpreted as double
                        op0Value = (double) lhs.as<int32_t>();
                    } else {
                        printf("Bad type for addition (op0)! %d\n", lhs.type);
                        exit(-1);
                    }
                }
                if (rhs.type != DOUBLE) {
                    if (rhs.type == ValueType_e::INT32) {
                        // Cast value to int32 pointer so that it can then be reinterpreted as double
                        op1Value = (double) rhs.as<int32_t>();
                    } else {
                        printf("Bad type for addition (op1)! %d\n", rhs.type);
                        exit(-1);
                    }
                }
                result.type = ValueType_e::DOUBLE;
                result.value.resize(sizeof(double));
                *result.value.data() = op0Value + op1Value;
            } else {
                // Integer addition
                int32_t op0Value;
                int32_t op1Value;
                op0Value = lhs.as<int32_t>();
                op1Value = rhs.as<int32_t>();
                result.type = ValueType_e::INT32;
                result.value.resize(sizeof(int32_t));
                *result.value.data() = op0Value + op1Value;
            }
            return result; // return the result by value (uses move constructor)
        }

        friend Value operator-(Value lhs,        // passing lhs by value helps optimize chained a+b+c
            Value rhs) // otherwise, both parameters may be const references
        {
            Value result;
            if (lhs.type == DOUBLE || rhs.type == DOUBLE) {
                // Convert both to doubles and do the addition
                double op0Value;
                double op1Value;
                if (lhs.type != DOUBLE) {
                    if (lhs.type == ValueType_e::INT32) {
                        // Cast value to int32 pointer so that it can then be reinterpreted as double
                        op0Value = (double) lhs.as<int32_t>();
                    } else {
                        printf("Bad type for addition (op0)! %d\n", lhs.type);
                        exit(-1);
                    }
                }
                if (rhs.type != DOUBLE) {
                    if (rhs.type == ValueType_e::INT32) {
                        // Cast value to int32 pointer so that it can then be reinterpreted as double
                        op1Value = (double) rhs.as<int32_t>();
                    } else {
                        printf("Bad type for addition (op1)! %d\n", rhs.type);
                        exit(-1);
                    }
                }
                result.type = ValueType_e::DOUBLE;
                result.value.resize(sizeof(double));
                *result.value.data() = op0Value - op1Value;
            } else {
                // Integer addition
                int32_t op0Value;
                int32_t op1Value;
                op0Value = lhs.as<int32_t>();
                op1Value = rhs.as<int32_t>();

                printf("op0value: %d, op1value %d sub %d\n", op0Value, op1Value, op0Value - op1Value);
                result = op0Value - op1Value;
            }
            return result; // return the result by value (uses move constructor)
        }

        Value(int32_t other) {
            this->type = INT32;
            setValue(other);
        }

        Value(double other) {
            this->type = DOUBLE;
            setValue(other);
        }

        Value& operator= (int32_t other) {
            this->type = INT32;
            setValue(other);

            return *this;
        }

        Value& operator= (const double other) {
            this->type = DOUBLE;
            setValue(other);

            return *this;
        }
    };

    // stack.pop(), but will exit if stack is empty
    void pop();

    // Pop the top element from the stack and store it in dest in the heap
    void pop_store(std::string dest);

    // Push an element to the heap
    void store(Value e, std::string dest);

    // Grab an element from the heap and push it onto the stack
    void push_from_heap(std::string src);

    // Push an element to the stack
    void push(Value e);


    // Add the top two numbers of the stack together
    // Pops both numbers off the stack and then pushes the result
    // Types are for top of stack, next top of stack.
    void add();

    // Subtract the top of the stack from the next element on the stack
    // Pops both numbers off the stack and then pushes the result
    // push(1) push(2) sub() = 1-2 = -1
    void sub();

    // Print the top of the stack
    void print();

    // Compares the top two elements of the stack.
    // Returns 0 if they are equal, 1 if the top is greater than the next, -1 otherwise
    // Pushes result to stack. Does not pop the compared values
    // Types are top of stack, next on stack
    void cmp();

    void jump_if_greater(std::string label, int32_t& pc);
    void jump_if_less(std::string label, int32_t& pc);
    void jump(std::string label, int32_t& pc);

    void ret(int32_t& pc);
    void call(std::string funcName, int32_t& pc);
}