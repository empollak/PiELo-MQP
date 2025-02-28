#include "codegen.h"
#include <iostream>
#include <fstream>
#include <map>

namespace PiELo {
    // The file stream which contains the final code
    std::fstream codeFile;
    // Pointer to the file that assembly is currently being written to
    std::fstream* file;
    // Each function definition gets a temporary file
    // Each function is stored in the functionDefinitions list
    // The current function being written to is pointed to by the index
    // If a new function needs to be defined, the code must change the index to be the end of the list and create a new file for itself
    // Then, the code must set the index back to what it was before when it is finished defining the function
    // That way, if functions are defined inside of each other, the original code will continue to define the higher-level function
    std::vector<std::fstream*> functionDefinitions;
    // Index of -1 indicates codeFile
    long functionDefinitionsIndex = -1;

    void codegen(Expression e); 
    struct VariableInfo {
        std::string scope;
        std::string reactivity;
    };
    std::map<std::string, VariableInfo> env;

    // Top-level codegen function. Performs initialization and will write to the filename given
    void codegenProgram(Expression e, std::string filename) {
        // Open the file for input/output, and discard all current contents
        codeFile.open(filename, std::fstream::in | std::fstream::out | std::fstream::trunc);
        file = &codeFile;
        codegen(e);
        *file << "end\n" << std::endl;
        // Append all of the function definitions
        for (std::vector<std::fstream*>::iterator funcFile = functionDefinitions.begin(); funcFile != functionDefinitions.end(); funcFile++) {
            (*funcFile)->seekg(0, std::ios_base::beg);
            *file << (*funcFile)->rdbuf();
            (*funcFile)->close();
            delete *funcFile;
        }
        for (size_t i = 0; i < functionDefinitions.size(); i++) {
            std::string tmpFilename = "tmp_" + std::to_string(i);
            // int result = remove(tmpFilename.c_str());
            // if (result != 0) perror(("remove " + tmpFilename).c_str());
        }
        file->close();
    }

    // Handles all regular procedures. Expects to be handed a LIST.
    // Similar to codegenList except only handles normal procedures and closure calls
    void codegenProcedure(Expression e) {
        // e.listValue[0] was confirmed to have type SYMBOL by codegenList().
        std::string procedureName = e.listValue[0].symbolValue;
        std::string assemblyInstruction = "";
        if (procedureName == "+") {
            assemblyInstruction = "add";
        } else if (procedureName == "print") {
            assemblyInstruction = "print";
        }

        // Codegen each arg in the list one by one (skipping the procedure name)
        for (std::vector<Expression>::iterator it = e.listValue.begin() + 1; it != e.listValue.end(); it++) {
            codegen(*it);
        }

        if (assemblyInstruction != "") {
            // Built in procedure
            *file << assemblyInstruction << std::endl;
        } else {
            // Closure call
            *file << "push i " << e.listValue.size() - 1 << std::endl; // Number of arguments = number of expressions - 1 (for the procedure itself)
            *file << "load " << procedureName << std::endl;
            *file << "call_closure" << std::endl;
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

    void defineFunction(std::string name, std::vector<std::string> arguments, std::vector<std::string> dependencies, Expression body) {
        // Create new file and adjust file pointer as necessary
        size_t oldIndex = functionDefinitionsIndex;
        size_t functionDefinitionsIndex = functionDefinitions.size();
        std::fstream* funcFile = new std::fstream();
        funcFile->open("tmp_" + std::to_string(functionDefinitionsIndex), std::ios_base::trunc | std::ios_base::out | std::ios_base::in);
        functionDefinitions.push_back(funcFile);
        file = funcFile;
        
        // Add arguments
        *file << "define_closure " << name << " " << arguments.size() << " ";
        for (std::string argument : arguments) *file << argument << " ";
        
        // Add dependencies
        *file << dependencies.size() << " ";
        for (std::string dependency : dependencies) {
            VariableInfo info;
            try {
                info = env.at(dependency);
            } catch (std::out_of_range e) {
                throw std::out_of_range("Could not find info for dependency " + dependency + " of function " + name);
            }
            if (info.scope == "local") throw std::invalid_argument("Dependency " + dependency + " of function " + name + " has local scope. This is not allowed.");
            *file << dependency << " ";
        }

        *file << std::endl;

        // Codegen the body
        codegen(body);

        // Return from the closure and store the result (closure index) into the given variable name
        *file << "push i 1\n" << "ret_from_closure" << std::endl;

        // Set the file back to how it was before
        functionDefinitionsIndex = oldIndex;
        if (functionDefinitionsIndex == -1) file = &codeFile;
        else file = functionDefinitions[functionDefinitionsIndex];
    }

    // For use when the expression is known to be of type LIST. Can handle special cases.
    void codegenList(Expression e) {
        if (e.listValue[0].type != Expression::SYMBOL) {
            throw std::runtime_error("codegenList got LIST with a first element of type " + e.typeToString() + " expression string " + e.toString());
        }

        // Conditionals
        if (e.listValue[0].symbolValue == "if") {
            // if special case would go here
            if (e.listValue.size() != 4) {
                throw std::invalid_argument("Incorrect number of arguments to 'if'. Expected 4, got " + std::to_string(e.listValue.size()) + ". from " + e.toString());
            }
            int localIfCounter = ifCounter++;
            // Conditional
            codegen(e.listValue[1]);
            *file << "jmp_if_zero else_" << localIfCounter << std::endl;
            // Then
            codegen(e.listValue[2]);
            *file << "jmp endif_" << localIfCounter << std::endl;
            // Else
            *file << "label else_" << localIfCounter << std::endl;
            codegen(e.listValue[3]);
            // End
            *file << "label endif_" << localIfCounter << std::endl;
        } 
        
        // Variable declarations
        else if (e.listValue[0].symbolValue == "var") {
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

            *file << "push_nil" << std::endl;
            std::cout << "Defined variable " << name.symbolValue << std::endl;
        } 
        
        // Variable Definitions
        else if (e.listValue[0].symbolValue == "set") {
            int localLambdaCounter = lambdaCounter++;
            if (e.listValue.size() != 3) {
                throw std::invalid_argument("Incorrect number of arguments to 'set'. Expected 3, got " + std::to_string(e.listValue.size()) + ". from " + e.toString());
            }
            if (e.listValue[1].type != Expression::SYMBOL) {
                throw std::invalid_argument("Attempted to set a non-symbol. " + e.toString());
            }


            VariableInfo var = env.at(e.listValue[1].symbolValue);
            if (var.reactivity == "reactive") {
                // Define an anonymous function and then immediately call it and store the result in this variable
                std::string name = "__lambda_" + std::to_string(localLambdaCounter);
                std::vector<std::string> dependencies = findVariables(e.listValue[2]);
                defineFunction(name, std::vector<std::string>(), dependencies, e.listValue[2]);

                *file << "push i 0\n" << "load __lambda_" << localLambdaCounter << "\n" << "call_closure\n";
                *file << "store " << var.scope << " " << e.listValue[1].symbolValue << std::endl;
            } else {
                codegen(e.listValue[2]);
                *file << "store " << var.scope << " " << e.listValue[1].symbolValue << std::endl;
            }
            *file << "push_nil" << std::endl;
        } 
        
        // Code blocks
        else if (e.listValue[0].symbolValue == "begin") {
            // Code block
            for (int i = 1; i < e.listValue.size(); i++) {
                codegen(e.listValue[i]);
                // Discard the result of all expressions other than the last
                if (i < e.listValue.size() - 1) *file << "pop" << std::endl;
            }
        } 

        // Function Definitions
        else if (e.listValue[0].symbolValue == "fun") {
            if (e.listValue.size() != 5) throw std::invalid_argument("fun expects 5 expressions, got " + std::to_string(e.listValue.size()) + ". Expression: " + e.toString());
            Expression reactivity = e.listValue[1];
            Expression name = e.listValue[2];
            Expression args = e.listValue[3];
            Expression body = e.listValue[4];
            
            if (name.type != Expression::SYMBOL) 
                throw std::invalid_argument("Expected a SYMBOL as second argument to fun. Got a " + e.typeToString() + ". Expression: " + e.toString());
            if (reactivity.type != Expression::SYMBOL) 
                throw std::invalid_argument("Expected a SYMBOL as first argument to fun for function " + name.symbolValue + ". Got a " + e.typeToString() + ". Expression: " + e.toString());
            if (reactivity.symbolValue != "reactive" && reactivity.symbolValue != "inert")
                throw std::invalid_argument("Expected reactivity of function " + name.symbolValue + " to be either 'reactive' or 'inert'. Instead, got " + reactivity.symbolValue);
            if (args.type != Expression::LIST) 
                throw std::invalid_argument("Expected a LIST as third argument to fun for function " + name.symbolValue + ". Got a " + e.typeToString() + ". Expression: " + e.toString());

            // Collect argument names
            std::vector<std::string> argumentNames;
            for (Expression argExp : args.listValue) {
                if (argExp.type != Expression::SYMBOL) throw std::invalid_argument("Expected a SYMBOL as a parameter to function " + name.symbolValue + ". Got a " + e.typeToString() + ". Expression: " + e.toString() + ", argument: " + argExp.toString());
                argumentNames.push_back(argExp.symbolValue);
            }
            

            // Find all dependencies, but ignore arguments
            // If the function is not reactive, it has no dependencies.
            std::vector<std::string> dependencies;
            if (reactivity.symbolValue == "reactive") {
                dependencies = findVariables(body);
                // Loop over all dependencies
                for (std::vector<std::string>::iterator dependency = dependencies.begin(); dependency != dependencies.end(); dependency++) {
                    // For each dependency, check against all arguments
                    for (std::string arg : argumentNames) {
                        if (arg == *dependency) {
                            // If this dependency matches, erase it from the list
                            // Also set the iterator to the next element and end the arg loop
                            dependency = dependencies.erase(dependency);
                            break;
                        }
                    }
                }
            }
            defineFunction(name.symbolValue, argumentNames, dependencies, body); 
            // Everything has to push something!
            *file << "push_nil" << std::endl;
        } 
        else {
            // All builtin procedures and closure calls
            codegenProcedure(e);
        }
    }

    void codegen(Expression expression) {
        switch (expression.type) {
            case Expression::LIST:
            codegenList(expression);
            break;
            case Expression::INT:
            *file << "push i " << std::to_string(expression.intValue) << std::endl;
            break;
            case Expression::FLOAT:
            *file << "push f " << std::to_string(expression.floatValue) << std::endl;
            break;
            case Expression::SYMBOL:
            // Assuming this is not a procedure call as that would be in codegenProcedure
            *file << "load " << expression.symbolValue << std::endl;
            break;
            default:
            std::cout << "nil!" << std::endl;
            break;
        }
    }
}