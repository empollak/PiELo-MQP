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


//

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
	int pc = 0; // program counter
	int tc = 0; // token counter

	//std::vector<i32> globals;
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
    bool isFunction(const std::string &token);
	std::vector<std::string> tokenizer(const std::string &line);
	std::tuple<std::string, std::string, std::string> analyzeLine(std::string s);
	//Op tok2op(const std::string& s);
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



