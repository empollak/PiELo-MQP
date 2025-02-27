#include "codegen.h"
#include <iostream>
#include <fstream>

namespace PiELo {
    std::fstream file;
    void codegen(Expression e); 

    // Top-level codegen function. Performs initialization and will write to the filename given
    void codegenProgram(Expression e, std::string filename) {
        // Open the file for input/output, and discard all current contents
        file.open(filename, std::fstream::in | std::fstream::out | std::fstream::trunc);
        codegen(e);
        file << "print" << std::endl;
        file << "end" << std::endl;
        file.close();
    }

    // Used when the first expression of a LIST is not a builtin procedure
    void codegenClosureCall(Expression e) {
        // handle closure calls
    }

    // Handles all procedures
    void codegenProcedure(Expression e) {
        if (e.symbolValue == "+") {
            file << "add" << std::endl;
        } else if (e.symbolValue == "print") {
            file << "print" << std::endl;
        } else {
            // Was not a built-in procedure
            codegenClosureCall(e);
        }
    }


    int ifCounter = 0;

    // For use when the expression is known to be of type LIST
    void codegenList(Expression e) {
        if (e.listValue[0].type != Expression::SYMBOL) {
            throw std::runtime_error("codegenList got LIST with a first element of type " + e.typeToString() + " expression string " + e.toString());
        }
        if (e.listValue[0].symbolValue == "if") {
            // if special case would go here
            if (e.listValue.size() != 4) {
                throw std::invalid_argument("Incorrect number of arguments to 'if'. Expected 4, got " + std::to_string(e.listValue.size()));
            }
            int localIfCounter = ifCounter++;
            // Conditional
            codegen(e.listValue[1]);
            file << "jmp_if_zero else_" << localIfCounter << std::endl;
            // Then
            codegen(e.listValue[2]);
            file << "jmp endif_" << localIfCounter << std::endl;
            // Else
            file << "label else_" << localIfCounter << std::endl;
            codegen(e.listValue[3]);
            // End
            file << "label endif_" << localIfCounter << std::endl;
        } else {
            // Codegen each arg in the list one by one (skip the procedure name)
            for (std::vector<Expression>::iterator it = e.listValue.begin() + 1; it != e.listValue.end(); it++) {
                codegen(*it);
            }
            // Need a special case because a non-built-in symbol here is a closure call
            codegenProcedure(e.listValue[0]);
        }
    }

    void codegen(Expression expression) {
        switch (expression.type) {
            case Expression::LIST:
            codegenList(expression);
            break;
            case Expression::INT:
            file << "push i " << std::to_string(expression.intValue) << std::endl;
            break;
            case Expression::FLOAT:
            file << "push f " << std::to_string(expression.floatValue) << std::endl;
            break;
            case Expression::SYMBOL:
            // Assuming this is not a procedure call as that would be in codegenProcedure
            file << "load " << expression.symbolValue << std::endl;
            break;
            default:
            std::cout << "nil!" << std::endl;
            break;
        }
    }
}