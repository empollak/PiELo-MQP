#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <iostream>
#include <stdexcept>
#include <string>
#include "vm.h"

// Base exception class (inheriting from std::exception)
class CustomException : public std::exception {
public:
    explicit CustomException(const std::string& message) : message_(message) {}

    // Override what() to return the error message
    const char* what() const noexcept override {
        return message_.c_str();
    }

private:
    std::string message_;
};

// Derived exception classes for specific errors

class EmptyInstructionLineException : public CustomException {
public:
    EmptyInstructionLineException() : CustomException("Error 1: EmptyInstructionLineException -- The instruction line does not contain neither a Label nor an Operation!") {}
};

class InvalidPrimitiveException  : public CustomException {
public:
    InvalidPrimitiveException(std::string operation) : CustomException("Error 2: InvalidPrimitiveException -- Primitive " + operation + " not identified!") {}
};

class InvalidOperandException  : public CustomException {
public:
    InvalidOperandException(std::string operation) : CustomException("Error 3: InvalideOperandException -- Operation " + operation + " does NOT accept an operand!") {}
};

class InvalidTypeException  : public CustomException {
public:
    InvalidTypeException() : CustomException("Error 4: InvalidTypeException -- Type not identified!") {}
};

class InvalidTypeForOperationException  : public CustomException {
public:
    InvalidTypeForOperationException(std::string operation, std::string type) : CustomException("Error 5: InvalidTypeForOperationException -- Cannot perform " + operation + " operation with a type " + type + "!") {}
};

class ShortOnElementsOnStackException  : public CustomException {
public:
    ShortOnElementsOnStackException(std::string operation) : CustomException("Error 6: ShortOnElementsOnStackException -- Stack has not enough elements to perform the " + operation + " operation.") {}
};

class PopEmptyStackException  : public CustomException {
public:
    PopEmptyStackException() : CustomException("Error 7: PopEmptyStackException -- Stack is Empty and cannot perform POP operation!") {}
};

class DivisionByZeroException  : public CustomException {
public:
    DivisionByZeroException() : CustomException("Error 8: DivisionByZeroException -- Cannot perform Division by Zero.") {}
};

class AddressNotDecleredException  : public CustomException {
public:
    AddressNotDecleredException() : CustomException("Error 9: AddressNotDecleredException -- Label was not decleared before this JMP operation.") {}
};

class TopStackNotZeroException  : public CustomException {
public:
    TopStackNotZeroException() : CustomException("Error 10: TopStackNotZeroException --  Cannot perform JZ (JUMP_IF-ZERO) because Top of the Stack is not Zero.") {}
};

class InvalidTypeAccessException : public CustomException {
public:
    InvalidTypeAccessException(std::string accessType, std::string variableType) : CustomException("Error 11: InvalidTypeAccessException -- Attempted to access variable as type " + accessType + ", but variable has type " + variableType + ".") {}
};

#endif