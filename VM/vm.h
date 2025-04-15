#ifndef __VM__H
#define __VM__H
#include <map>
#include <vector>
#include <string>
#include <stack>
#include "exceptions.h"
#include "instructionHandler.h"
#include <algorithm>
#include <uuid/uuid.h>
#include "parser.h"
#include "networking.h"
#include "gc.h"

namespace PiELo {

    struct Tag {
        std::string tagName;
    };

    enum Type {
        NIL,
        PIELO_CLOSURE,
        C_CLOSURE,
        FLOAT,
        INT,
        NAME,
        POINTER // Don't use this in the VM! Please! It's mostly for controllers for ARGoS
    };
    class Variable;
    class VM;
    typedef std::map<std::string, Variable> symbolTable;
    typedef Variable (*funp) (VM* vm);

    class VariableData {
        public:
        union {
            int asInt;
            float asFloat;
            size_t asClosureIndex;
            funp asFunctionPointer;
            void* asPointer;
        };
        Type type = Type::NIL;

        VariableData(int i) {asInt = i; type = Type::INT;}

        VariableData(float f) {asFloat = f; type = Type::FLOAT;}

        VariableData(size_t s) {asClosureIndex = s; type = Type::PIELO_CLOSURE;}

        VariableData(void* p) {asPointer = p; type = Type::POINTER;}

        VariableData(funp f) {asFunctionPointer = f; type = Type::C_CLOSURE;}

        VariableData() {type=Type::NIL;}

        Type getType() {return type;}

        std::string getTypeAsString() {
            switch (type) {
                case Type::NIL: return "NIL";
                case Type::PIELO_CLOSURE: return "PIELO_CLOSURE";
                case Type::C_CLOSURE: return "C_CLOSURE";
                case Type::FLOAT: return "FLOAT";
                case Type::INT: return "INT";
                case Type::NAME: return "NAME";
                case Type::POINTER: return "POINTER";
                default: return "invalid type";
            }
        }

        void print(VM* vm);
    };





    class Variable {
    private:
        VariableData data;
        std::map<int, VariableData>::iterator iter;
        // VariableData data;
        
    public:

        std::map<int, VariableData> stigmergyData;
        bool changed = 0;
        bool isStigmergy = 0;
        // List of indices in the closure list
        std::vector<size_t> dependants;
        std::vector<Tag> tags;
        timestamp_t lastUpdated;

        Variable() {data.type = NIL;}

        Variable(int i) {data.type = INT; data.asInt = i;}

        Variable(float f) {data.type = FLOAT; data.asFloat = f;}

        Variable(size_t s) {data.type = PIELO_CLOSURE; data.asClosureIndex = s;}

        Variable(void* p) {data.type = POINTER; data.asPointer = p;}

        Variable(funp f) {data.type = C_CLOSURE; data.asFunctionPointer = f;}

        Variable(VariableData v) {data = v;}

        std::string getTypeAsString() {
            return data.getTypeAsString();
        }

        float getFloatValue() {
            stigmergyData.begin();
            if (isStigmergy) throw std::runtime_error("Tried to access stigmergy variable as float");
            if (data.type != FLOAT) throw InvalidTypeAccessException("FLOAT", getTypeAsString());
            return data.asFloat;
        }

        int getIntValue() {
            if (isStigmergy) throw std::runtime_error("Tried to access stigmergy variable as int");
            if (data.type != INT) throw InvalidTypeAccessException("INT", getTypeAsString());
            return data.asInt;
        }

        size_t getClosureIndex() {
            if (isStigmergy) throw std::runtime_error("Tried to access stigmergy variable as closure index");
            if (data.type != PIELO_CLOSURE) throw InvalidTypeAccessException("PIELO_CLOSURE", getTypeAsString());
            return data.asClosureIndex;
        }

        funp getFunctionPointer() {
            if (isStigmergy) throw std::runtime_error("Tried to access stigmergy variable as function pointer");
            if (data.type != C_CLOSURE) throw InvalidTypeAccessException("C_CLOSURE", getTypeAsString());
            return data.asFunctionPointer;
        }

        void* getPointer() {
            if (isStigmergy) throw std::runtime_error("Tried to access stigmergy variable as pointer");
            if (data.type != POINTER) throw InvalidTypeAccessException("POINTER", getTypeAsString());
            return data.asPointer;
        }

        // DO NOT USE TO EXTRACT DATA!
        VariableData getVariableData() {
            return data;
        }

        Type getType() {return data.type;}

        void mutateValue(float f) {if (isStigmergy) throw std::runtime_error("Tried to access stigmergy variable as function pointer"); data = f;}
        void mutateValue(int i) {if (isStigmergy) throw std::runtime_error("Tried to access stigmergy variable as function pointer"); data = i;}
        void mutateValue(size_t s) {if (isStigmergy) throw std::runtime_error("Tried to access stigmergy variable as function pointer"); data = s;}
        void mutateValue(Variable v) {if (isStigmergy) throw std::runtime_error("Tried to access stigmergy variable as function pointer"); data = v.data;}

        // Throws an error if this variable is not stigmergy
        void ensureStig() {
            if (!isStigmergy) throw std::runtime_error("Tried use stigmergy function on non-stigmergy variable");
        }

        void updateStigValue(int id, VariableData data) {
            ensureStig();
            this->data.type = data.type;
            stigmergyData[id] = data;
        }

        void updateStigValue(int id, Variable v) {
            updateStigValue(id, v.data);
        }

        int getStigSize() {
            ensureStig();
            return stigmergyData.size();
        }

        VariableData nextIterValue();
        
        VariableData peekIterValue();

        bool isIterAtEnd() {
            ensureStig();
            return iter == stigmergyData.end();
        }

        void resetIter() {
            ensureStig();
            iter = stigmergyData.begin();
        }
        
        void print() {
            if(getType() == NIL){
                std::cout << "nil";
            } else if(getType() == INT){
                std::cout << "int " << getIntValue();
            } else if(getType() == FLOAT){
                std::cout << "float " << getFloatValue();
            } else if (getType() == PIELO_CLOSURE) {
                std::cout << "closure index: ";
                std::cout << getClosureIndex();
            } else if (getType() == POINTER) {
                std::cout << "pointer " << (size_t) getPointer();
            } else {
                std::cout << "undefined type for printing: " << getTypeAsString();
            }
        }
    };

    struct ClosureData {
        // Pointer to code, however we decide to store that
        codePtr codePointer;
        symbolTable localSymbolTable; // Should this be a pointer? Probably
        std::vector<std::string> argNames;
        std::vector<std::string> dependencies;
        std::vector<size_t> dependants; // dependant closures
        std::map<std::string, int> dependantVariables; // dependant variables. int is unused.
        VariableData cachedValue;
        bool marked = false; // For garbage collection
        bool isTemplate;
    };

    class ClosureMap : public std::map<size_t, ClosureData> {
        private:
            size_t headOfList = 0;
        public:
            void push_back(ClosureData c);
            size_t getHeadOfList() {return headOfList;}
            void resetHeadOfList() {headOfList = 0;}
            ClosureMap() : headOfList(0) {headOfList = 0;}
    };

    struct opCodeInstructionOrArgument { // struct that helps handle the storing of values pushed by opcodes.
        enum {
            INSTRUCTION,
            FLOAT,
            INT,
            STRING,
            NIL,
            PIELO_CLOSURE,
            C_CLOSURE,
            NAME,
            LOCATION
        } type;
        union {
            Instruction asInstruction;
            float asFloat;
            int asInt;
            std::string* asString;
            ClosureData* asClosure;
            std::string* asLocation;
        };
    
        opCodeInstructionOrArgument(int value_i) : type(INT) {asInt = value_i;}

        opCodeInstructionOrArgument(float f) : type(FLOAT) {asFloat = f;}

        opCodeInstructionOrArgument(Instruction instruction): type(INSTRUCTION) {asInstruction = instruction;}

        opCodeInstructionOrArgument(codePtr codePointer, std::vector<std::string> dependencies, std::vector<std::string> args, std::vector<PiELo::Type> argTypes) {
            type = PIELO_CLOSURE;
            asClosure = (ClosureData*) malloc(sizeof(ClosureData));
            asClosure->codePointer = codePointer;
            asClosure->argNames = args;
        }

        opCodeInstructionOrArgument(ClosureData closureData) {
            type = PIELO_CLOSURE;
            printf("Typed\n");
            asClosure = new ClosureData;
            printf("malloc'd\n");
            *asClosure = closureData;
            printf("Done mallocing\n");
        }

        //opCodeInstructionOrArgument(const std::string& value_s) : type(STRING) {value.asString = new std::string(value);}
        
        opCodeInstructionOrArgument(const std::string s) : type(STRING) {
            asString = new std::string(s);
        }

        ~opCodeInstructionOrArgument() {
            if (type == STRING) {
                // std::cout << "freeing string ptr: " << asString << std::endl;
                // std::cout << "had value " << *asString << std::endl;
                delete asString;
            } else if (type == PIELO_CLOSURE) {
                // printf("Freeing closure \n");
                delete asClosure;
            } else if (type == LOCATION) {
                delete asLocation;
            }
        }

        opCodeInstructionOrArgument(const opCodeInstructionOrArgument& other) {
            // printf("Copying!\n");
            if (this != &other) {
                type = other.type;
                switch (other.type) {
                    case INSTRUCTION: asInstruction = other.asInstruction; break;
                    case FLOAT: asFloat = other.asFloat; break;
                    case INT: asInt = other.asInt; break;
                    case STRING: asString = new std::string(*other.asString); break;
                    case NIL: break;
                    case PIELO_CLOSURE: 
                        asClosure = new ClosureData();
                        *asClosure = *other.asClosure;
                        break;
                    case NAME: asString = new std::string(*other.asString); break;
                    case LOCATION: asLocation = new std::string(*other.asLocation); break;
                }
            }
        }

        opCodeInstructionOrArgument& operator= (const opCodeInstructionOrArgument& other) {
            // printf("operator=\n");
            if (this != &other) {
                type = other.type;
                switch (other.type) {
                    case INSTRUCTION: asInstruction = other.asInstruction; break;
                    case FLOAT: asFloat = other.asFloat; break;
                    case INT: asInt = other.asInt; break;
                    case STRING: asString = new std::string(*other.asString); break;
                    case NIL: break;
                    case PIELO_CLOSURE: 
                        asClosure = new ClosureData;
                        *asClosure = *other.asClosure;
                    case NAME: asString = new std::string(*other.asString); break;
                    case LOCATION: asLocation = new std::string(*other.asLocation); break;
                }
            }
            return *this;
        }
        
        std::string getTypeAsString() const {
            switch (type) {
                case NIL: return "NIL";
                case PIELO_CLOSURE: return "PIELO_CLOSURE";
                case C_CLOSURE: return "C_CLOSURE";
                case FLOAT: return "FLOAT";
                case INT: return "INT";
                case NAME: return "NAME";
                case INSTRUCTION: return "INSTRUCTION";
                case STRING: return "STRING";
                case LOCATION: return "LOCATION";
                default: return "invalid type";
            }
        }
        
        float getFloatFromMemory() const {
            if (type != FLOAT) throw InvalidTypeAccessException("FLOAT", getTypeAsString());
            return asFloat;
        }

        int getIntFromMemory() const{
            if (type != INT) throw InvalidTypeAccessException("INT", getTypeAsString());
            return asInt;
        }

        // ClosureData* getClosureDataFromMemory() {
        //     if (type != PIELO_CLOSURE) throw InvalidTypeAccessException("PIELO_CLOSURE", getTypeAsString());
        //     return asClosure;
        // }
        
        std::string* getNameValueFromMemory() const{
            if (type != NAME) throw InvalidTypeAccessException("NAME", getTypeAsString());
            return asString;
        }
    };



    
    struct Message {
        timestamp_t variableLastUpdated;
        int robotID;
        long senderX;
        long senderY;
        long senderZ;
        // TODO: Update this or make it the law
        char variableName[100];
        VariableData data;
        bool isStigmergy;
    };
    

    class VM {
        public:
        Parser parser; 
        Networking network; 
        GarbageCollector garbageCollector;
        std::fstream logfile;
        enum VMState {
            READY,
            DONE,
            ERROR
        };

        Type stringToType(std::string s);

        int robotID;

        ClosureMap closureList;


        struct scopeData{
            symbolTable* scopeSymbolTable;
            codePtr codePointer;
            size_t closureIndex;
        };

        
        std::vector<opCodeInstructionOrArgument> bytecode;
        codePtr programCounter = 0;
        
        symbolTable taggedTable;
        // Variables which are at the top level but not tagged
        symbolTable globalSymbolTable;
        std::stack<Variable> stack;


        std::stack<scopeData> returnAddrStack;
        
        std::map<std::string, Variable>* currentSymbolTable = &globalSymbolTable; 

        std::vector<Tag> robotTagList;
        
        // Initialize the global context to closure index -1
        size_t currentClosureIndex = -1;

        VMState state = ERROR;

        Variable* findVariable(std::string name);

        // Run one line of assembly code
        VMState step();

        // Take in a string filename. Expects a file of the format:
        // instruction <arguments> \n
        // Allows comments with #
        // Loads the instructions into the bytecode vector.
        VMState load(std::string filename);


        void handleInstruction(opCodeInstructionOrArgument op); 

        // ARITHMETIC INSTRUCTION
        void add();
        void sub();
        void mul();
        void div();
        void mod();

        void registerFunction(std::string name, funp f);
        void removeClosureDependantForVariable(Variable* v, std::string varName);
        void addClosureDependantForVariable(Variable* v, std::string varName);
        void storeLocal(std::string varName);
        void storeGlobal(std::string varName);
        void loadToStack(const std::string& varName);
        void tagVariable(const std::string& varName, const std::string& tagName);
        void tagRobot(const std::string& tagName);
        void storeTagged(const std::string& varName);
        void handleDependants(Variable& var, bool storeCurrentScope = true);
        void storeStig(const std::string& varName);
        void pushNextElementOfStig(const std::string& varName);
        void isIterAtEnd(const std::string& varName);
        void resetIter(const std::string& varName);
        void recursivelyAddDependantsOfClosureToReturnAddrStack(size_t closureIndex, std::stack<scopeData> &dependants);
        void stigSize(std::string varName);
        void pushNil();
        void pushInt();
        void pushFloat();
        void dup();
        void swap();
        void pop();
        void isNil();
        void eql();
        void neql();
        void gt();
        void gte();
        void lt();
        void lte();
        void land();
        void lor();
        void lnot();
        void jump();
        void jump_if_zero();
        void jump_if_not_zero();
        void defineClosure(std::string closureName, ClosureData closureData); 
        void callClosure();
        void retFromClosure();
        void rerunClosure(size_t closureIndex);
        void uncache();
    };
}
#endif