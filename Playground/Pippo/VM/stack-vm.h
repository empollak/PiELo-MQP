#ifndef STACK_VM_H
#define STACK_VM_H

#include "lexer.h"
#include "exceptions.h"
#include <stack>
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>


// const char* to_s(Op op);

// Op from_s(const char* str);

// Instructions

enum Op {
  NOP,  // do nothing
  PUSHNULL,
  PUSHI,
  PUSHF,
  PUSHD,
  POP,
  ADD,  // pop a, pop b, push a + b
  SUB,  // pop a, pop b, push a - b
  MUL,
  DIV,
  MOD,
  EQL,
  NEQL,
  GT,
  GTE,
  LT,
  LTE,
  SWAP,
  DUP,
  JMP,
  JZ,
  CALL,
  RET,
  HALT

};


// TAGGED UNION

enum ValueType {
	INT,
	FLOAT,
	DOUBLE,
	STRING,
	NIL
};

struct TaggedValue {
	ValueType type;

	union {
		int intValue;
		float floatValue;
		double doubleValue;
		std::string* stringValue; // Pointer to avoid constructing std::string in union
	} as;

	// Constructor for int
    TaggedValue(int value) : type(INT) {as.intValue = value;}

    // Constructor for float
    TaggedValue(float value) : type(FLOAT) {as.floatValue = value;}
    // Constructor for double
    TaggedValue(double value) : type(DOUBLE)  {as.doubleValue = value;}
    // Constructor for string
    TaggedValue(const std::string& value) : type(STRING) {as.stringValue = new std::string(value);}

    // Default constructor for null (NONE)
    TaggedValue() : type(NIL) {}

	//Destructor to clean up dynamically allocated string
    ~TaggedValue() {
        if (type == STRING) {
            delete as.stringValue;
        }
    }

	ValueType getType() const{
		return type;
	}

	// Accessors

	double getDouble() const {
        if (type != DOUBLE){ std::cout << "Error: Type expected DOUBLE." << std::endl;}
        return as.doubleValue;
    }
	float getFloat() const {
        if (type != FLOAT){ std::cout << "Error: Type expected FLOAT." << std::endl;}
        return as.floatValue;
    }

    int getInt() const {
        if (type != INT){ std::cout << "Error: Type expected INT." << std::endl;}
        return as.intValue;
    }
    

    std::string getString() const {
        if (type != STRING){ std::cout << "Error: Type expected STRING." << std::endl;}
        return *as.stringValue;
    }
	

	// Copy constructor for safe string handling
    TaggedValue(const TaggedValue& other) : type(other.type) {
        switch (type) {
            case INT: as.intValue = other.as.intValue; break;
            case FLOAT: as.floatValue = other.as.floatValue; break;
            case DOUBLE: as.doubleValue = other.as.doubleValue; break;
            case STRING: as.stringValue = new std::string(*other.as.stringValue); break;
            default: break;
        }
    }

    // Assignment operator for safe string handling
    TaggedValue& operator=(const TaggedValue& other) {
        if (this != &other) {
            // Clean up existing string
            if (type == STRING) {
                delete as.stringValue;
            }

            // Copy new value
            type = other.type;
            switch (type) {
                case INT: as.intValue = other.as.intValue; break;
                case FLOAT: as.floatValue = other.as.floatValue; break;
                case DOUBLE: as.doubleValue = other.as.doubleValue; break;
                case STRING: as.stringValue = new std::string(*other.as.stringValue); break;
                default: break;
            }
        }
        return *this;
    }

	// Helper function to check if it's a nil value
    bool isNil() const { return type == NIL; }

};


// FunctionMap struct
// struct FunctionMap {
//     std::map<std::string, std::map<std::string, TaggedValue>> functions_params_map;

//     // add parameter
//     void addFunctionAndParams(std::string function_name, std::string args) { 
//         std::vector<std::string> variables_types;  // Vector to store types
//         std::vector<std::string> variables_names;
// 				// FIND VARIABLES NAMES
//         size_t start = args.find('(');
//         size_t end = args.find(')');
        
//         if (start == std::string::npos || end == std::string::npos || start >= end) {
//             // no params
//         }

//         std::string params = args.substr(start + 1, end - start - 1);  // Extract substring between parentheses
//         std::stringstream ss(params);
//         std::string type, var;

//         while (ss >> type) {  // Read each type
//             if (ss.peek() == ',') {  // Check if the next character is a comma
//                 ss.ignore();  // Ignore the comma
//             }
            
//             // Read the variable name
//             ss >> var;  // Read the variable name

//             // Remove any trailing commas from the variable name
//             if (var.back() == ',') {
//                 var.pop_back();
//             }
            
//             variables_names.push_back(var);  // Store the variable name
//             variables_types.push_back(type);  // Store the type
//         }

//         for(int i = 0; i < variables_names.size(); i++){
//             if(variables_types[i] == "int" || variables_types[i] == "INT"){
//                 int val = -1;
//                 functions_params_map[function_name][variables_names[i]] = TaggedValue(val);
//             } else if (variables_types[i] == "float" || variables_types[i] == "FLOAT"){
//                 float val = -1.0;
//                 functions_params_map[function_name][variables_names[i]] = TaggedValue(val);
//             } else if (variables_types[i] == "double" || variables_types[i] == "DOUBLE"){
//                 double val = -1;
//                 functions_params_map[function_name][variables_names[i]] = TaggedValue(val);
//             } else if (variables_types[i] == "string" || variables_types[i] == "STRING"){
//                 std::string val = "";
//                 functions_params_map[function_name][variables_names[i]] = TaggedValue(val);
//             } else {
//                 functions_params_map[function_name][variables_names[i]] = TaggedValue();
//             }
//         }


//     }

//     void MapValuesToVars(std::string s) { // should take test2(4,2)
//         std::string function_name = s.substr(0, s.find('('));
//         std::vector<TaggedValue> values;

//         // Find the opening and closing parentheses
//         size_t open_paren = s.find('(');
//         size_t close_paren = s.find(')');

//         // Extract the arguments string
//         std::string arg_str = s.substr(open_paren + 1, close_paren - open_paren - 1);

//         // Create a string stream to parse the arguments
//         std::stringstream ss(arg_str);

//         // Parse each argument and push it into the vector
//         int arg;
//         while (ss >> arg) {
//             values.push_back(arg);

//             // Skip the comma if there's another argument
//             char comma;
//             ss >> comma;
//         }

//         std::vector<std::string> vars_names;
//         int h = 0;
//         // Iterate through the map and collect keys
//         for(const auto& pair : functions_params_map[function_name]) {
//             vars_names.push_back(pair.first);
//         }

//         for(int j = 0; j < values.size(); j++){
//             functions_params_map[function_name][vars_names[j]] = values[j];
//         }

//         // we have a vector

//     }

//     //add function
// };

// Virtual Machine

struct FunctionInfo {
    std::string function_name;
    std::vector<std::string> paramNames;
    std::vector<ValueType> paramTypes;
    int address;
};


enum VMs_Status {
	READY,
	DONE,
	ERROR
};


class StackVM {
public: 
	StackVM();
	void printTaggedValue(TaggedValue data_ex);
	void printStack();
	void step(std::vector<TaggedValue> instruction_line);
	// void run(std::vector<std::vector<i32>> program);
	void run();
	void printProgram();


	std::stack<TaggedValue> stack;
	std::vector<std::vector<TaggedValue>> policy;

    std::map<std::string, int> labelMap;
    int next_pc = 0;
	int pc = 10; // program counter
	int tc = 0; // token counter

    std::map<std::string, FunctionInfo> function_table;
	//std::shared_ptr<Contex> ctx;

	//std::vector<std::shared_ptr<FunctionMetaData>> metadata;

	//i32 typ = 0;
	TaggedValue dat; // data register
	int running = 1;
	//bool jumping = false;
	VMs_Status status = READY;


	// functions to compile

	bool isPrimitive(std::string s);
	bool isLabel(const std::string &token);
    bool isCreatingFunction(const std::string &token);
    bool isCallingFunction(const std::string &token);
	std::vector<std::string> tokenizer(const std::string &line);
	std::tuple<std::string, std::string, std::string> analyzeLine(std::string s);
	//Op tok2op(const std::string& s);
    FunctionInfo createFunctionInfo(std::string label, std::string operation, int address);
	void loadInstructions(strings s);	
    void printMap();

	//TaggedValue getType(TaggedValue instruction_token);
	TaggedValue getData(TaggedValue instruction_token);
	void fetch();
	void decode(TaggedValue instruction_token);
	Op convertToPrimitive(std::string s);
	void execute(TaggedValue instruction_token);

	void pushNull_operation();
	void pushi_operation();
	void pushf_operation();
	void pushd_operation();
	void pop_operation();
	void add_operation();
	void sub_operation();
	void mul_operation();
	void div_operation();
	void mod_operation();

	void eql_operation();
	void neql_operation();
	void gt_operation();
	void gte_operation();	
	void lt_operation();
	void lte_operation();

	void swap_operation();
	void dup_operation();
	void jump_operation();
	void jump_if_zero_operation();
    void call_function();
    void ret_operation();

	void doPrimitive(Op operation);
	
};


// enum Instruction {
// 	HALT = 0,
//     PUSH = 1,
// 	POP = 2,
// 	ADD = 3,
// 	SUB = 4,
// 	MUL = 5,
// 	DIV = 6,
// 	MOD = 7,
// 	EQL = 8, // is equal
// 	BTN = 9, // top-of-stack bigger than the element below
// 	STN = 10, // top-of-stack smaller than the element below
// 	SWAP = 11,
// 	DUP = 12,
// 	JMP = 13,
// 	JZ = 14, 
// 	CALL = 15,
// 	RET = 16,
// };


// typedef struct FunctionMetaData
// {
// 	string name;
// 	int numArgs;
// 	int numLocals;
// 	int address;
// 	FunctionMetaData(string n, int na, int nl, int addr) :
//     	name(n), numArgs(na), numLocals(nl), address(addr) {};
// } FunctionMetaData;

// typedef struct Context {
// 	std::shared_ptr<Context> invokingContext; // parent in the stack, i.e. the caller
// 	//FunctionMetaData metadata; // info about the function we're executing
// 	std::shared_ptr<FunctionMetaData> metadata; // info about the function we're executing
// 	int returnIp; // ip location to return to once function returns
// 	vector<int> locals; // args + locals, indexed from 0

// 	Context(std::shared_ptr<Context> ic, int rip, shared_ptr<FunctionMetaData> fmd) :
//     	invokingContext(ic), returnIp(rip), metadata(fmd) {
//         	vector<int>(metadata->numArgs + metadata->numLocals).swap(locals);
//     	};
// } Context;

#endif



