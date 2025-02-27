#include "codegen.h"
#include <iostream>
#include <fstream>
#include <map>

namespace PiELo {
    std::fstream file;
    void codegen(Expression e); 
    struct VariableInfo {
        std::string scope;
        std::string reactivity;
    };
    std::map<std::string, VariableInfo> env;

    // Top-level codegen function. Performs initialization and will write to the filename given
    void codegenProgram(Expression e, std::string filename) {
        // Open the file for input/output, and discard all current contents
        file.open(filename, std::fstream::in | std::fstream::out | std::fstream::trunc);
        codegen(e);
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
    int lambdaCounter = 0;

    // Finds all variable names in the expression. Ignores procedures.
    std::vector<std::string> findVariables(Expression e) {
        std::vector<std::string> variables;
        if (e.type == Expression::SYMBOL) {
            // Just the one variable
            variables.push_back(e.symbolValue);
        } else if (e.type == Expression::LIST) {
            if (e.listValue.size() > 1) {
                // Skip the procedure
                for (int i = 1; i < e.listValue.size(); i++) {
                    std::vector<std::string> subVariables = findVariables(e.listValue[i]);
                    variables.insert(variables.end(), subVariables.begin(), subVariables.end());
                }
            }
        }
        return variables;
    }

    // For use when the expression is known to be of type LIST
    void codegenList(Expression e) {
        if (e.listValue[0].type != Expression::SYMBOL) {
            throw std::runtime_error("codegenList got LIST with a first element of type " + e.typeToString() + " expression string " + e.toString());
        }
        if (e.listValue[0].symbolValue == "if") {
            // if special case would go here
            if (e.listValue.size() != 4) {
                throw std::invalid_argument("Incorrect number of arguments to 'if'. Expected 4, got " + std::to_string(e.listValue.size()) + ". from " + e.toString());
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
        } else if (e.listValue[0].symbolValue == "var") {
            if (e.listValue.size() != 4) {
                throw std::invalid_argument("Incorrect number of arguments to 'vardef'. Expected 4, got " + std::to_string(e.listValue.size()) + ". from " + e.toString());
            }
            Expression scope = e.listValue[1];
            Expression reactivity = e.listValue[2];
            Expression name = e.listValue[3];
            if (scope.type != Expression::SYMBOL || (scope.symbolValue != "local" && scope.symbolValue != "global" && scope.symbolValue != "tagged")) {
                throw std::invalid_argument("The scope argument must be one of 'local', 'global', or 'tagged'. Expression was: " + e.toString());
            }
            if (reactivity.type != Expression::SYMBOL || (reactivity.symbolValue != "reactive" && reactivity.symbolValue != "inert")) {
                throw std::invalid_argument("The reactivity argument must be one of 'reactive' or 'inert'. Expression was: " + e.toString());
            }
            if (name.type != Expression::SYMBOL) {
                throw std::invalid_argument("The name argument must be a symbol. Expression was: " + e.toString());
            }
            env[name.symbolValue].reactivity = reactivity.symbolValue;
            env[name.symbolValue].scope = scope.symbolValue;

            file << "push_nil" << std::endl;
            std::cout << "Defined variable " << name.symbolValue << std::endl;
        } else if (e.listValue[0].symbolValue == "set") {
            int localLambdaCounter = lambdaCounter++;
            if (e.listValue.size() != 3) {
                throw std::invalid_argument("Incorrect number of arguments to 'set'. Expected 3, got " + std::to_string(e.listValue.size()) + ". from " + e.toString());
            }
            if (e.listValue[1].type != Expression::SYMBOL) {
                throw std::invalid_argument("Attempted to set a non-symbol. " + e.toString());
            }
            VariableInfo var = env.at(e.listValue[1].symbolValue);
            if (var.reactivity == "reactive") {
                std::vector<std::string> dependencies = findVariables(e.listValue[2]);
                file << "define_closure __lambda_" << localLambdaCounter << " 0 " << dependencies.size() << " ";
                for (std::string dependency : dependencies) file << dependency << " ";
                file << std::endl;
                // Codegen what the variable will be set to
                codegen(e.listValue[2]);
                // Return from the closure and store the result (closure index) into the given variable name
                file << "push i 1\n" << "ret_from_closure\n" << "store " << var.scope << " " << e.listValue[1].symbolValue << std::endl;
            } else {
                codegen(e.listValue[2]);
                file << "store " << var.scope << " " << e.listValue[1].symbolValue << std::endl;
            }
            file << "push_nil" << std::endl;
        } else if (e.listValue[0].symbolValue == "begin") {
            // Code block
            for (int i = 1; i < e.listValue.size(); i++) {
                codegen(e.listValue[i]);
                // Discard the result of all expressions other than the last
                if (i < e.listValue.size() - 1) file << "pop" << std::endl;
            }
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