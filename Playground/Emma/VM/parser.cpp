#include "parser.h"

using namespace PiELo;

// The code file
std::ifstream file;

// The bytes of the code, in memory
std::vector<uint8_t> code;

// Program counter
int32_t pc = 0;

template <class T>
void pushNumber() {
    T value;
    for (int i = 0; i < sizeof(T); i++) {
        ((uint8_t*) &value)[i] = code[pc++];
    }

    push(value);
}

template <class T>
T getNumberFromCode() {
    T value;
    for (int i = 0; i < sizeof(T); i++) {
        ((uint8_t*) &value)[i] = code[pc++];
    }

    return value;
}

std::string getStringFromCode() {
    std::string s;
    char c;
    // Assume that there will be a null terminator
    while ((c = code[pc++])) {
        s.push_back(c);
    }

    return s;
}

int PiELo::step() {
    // printf("Running %d\n", code[pc]);
    switch (code[pc++]) {
    case PUSHI:
        pushNumber<int32_t>();
        break;
    case PUSHD:
        pushNumber<double>();
        break;
    case PUSHS:
        printf("no implementation for pushs\n");
        return -1;
    case PUSH_FROM_HEAP:
        push_from_heap(getStringFromCode());
        break;
    case POP:
        pop();
        break;
    case POP_STORE:
        pop_store(getStringFromCode());
        break;
    case STOREI:
        // number, label
        store(getNumberFromCode<int32_t>(), getStringFromCode());
        break;
    case STORED:
        store(getNumberFromCode<double>(), getStringFromCode());
        break;
    case ADD:
        add();
        break;
    case SUB:
        sub();
        break;
    case PRINT:
        print();
        break;
    case CMP:
        cmp();
        break;
    case JMPG:
        jump_if_greater(getStringFromCode(), pc);
        break;
    case JMPL:
        jump_if_less(getStringFromCode(), pc);
        break;
    case JMP:
        jump(getStringFromCode(), pc);
        break;
    case RET:
        ret(pc);
        break;
    case CALL:
        call(getStringFromCode(), pc);
        break;
    case EXIT:
        exit(0);
        break;
    default:
        std::cout << "Invalid opcode " << code[pc - 1] << std::endl;
        return -1;
        break;
    }
    if (pc >= code.size()) return 0;
    else return 1;
}

void invalidInstructionHandler(std::string instruction) {
    std::cout << "Invalid instruction " << instruction << std::endl;
    exit(-1);
}
// Push an element onto the back of the code vector
template<class T>
void pushNumberToCode(T e) {
    size_t startSize = code.size();

    // Make space for the element
    code.resize(startSize + sizeof(T));

    // Insert the int32 at the back of the code vector
    *((T*) (code.data() + startSize)) = e;
}

void pushStringToCode(std::string s) {
    size_t startSize = code.size();
    for (std::string::iterator it = s.begin(); it != s.end(); ++it) {
        code.push_back(*it);
    }

    // Null terminator
    code.push_back(0);
}

void pushHandler(std::string input) {
    if (input == "pushi") {
        file >> input;
        code.push_back(PUSHI);

        int32_t num = stoi(input);
        pushNumberToCode(num);
    } else if (input == "pushd") {
        file >> input;
        code.push_back(PUSHD);

        double num = stod(input);
        pushNumberToCode(num);
    } else if (input == "pushs") {
        file >> input;
        code.push_back(PUSHS);

        pushStringToCode(input);
    } else if (input == "pushh") {
        // printf("Parsing push from heap\n");
        file >> input;
        code.push_back(PUSH_FROM_HEAP);

        pushStringToCode(input);
    } else {
        invalidInstructionHandler(input);
    }
}

void storeHandler(std::string input) {
    if (input == "storei") {
        file >> input;
        code.push_back(STOREI);

        int32_t num = stoi(input);
        pushNumberToCode(num);
    } else if (input == "stored") {
        file >> input;
        code.push_back(STORED);

        // Push the number to be stored
        double num = stod(input);
        pushNumberToCode(num);
        file >> input;

        // Push the label
        pushStringToCode(input);
    } else {
        invalidInstructionHandler(input);
    }
}

void jumpHandler(std::string input) {
    if (input == "jmpg") {
        code.push_back(JMPG);
    } else if (input == "jmpl") {
        code.push_back(JMPL);
    } else if (input == "jmp") {
        code.push_back(JMP);
    } else {
        invalidInstructionHandler(input);
    }

    // All jumps come with a label
    file >> input;
    pushStringToCode(input);
}

std::vector<uint8_t>* PiELo::load(std::string filename) {
    file.open(filename);
    if (!file.is_open()) {
        printf("Error opening file %s\n", filename.c_str());
        exit(-1);
    }

    // Parse the code
    std::string input;
    while (file >> input) {
        // std::cout << "Parsing instruction " << input << std::endl;
        if (input.substr(0, 4) == "push") {
            pushHandler(input);
        } else if (input == "pop") {
            code.push_back(POP);
        } else if (input == "pops") {
            code.push_back(POP_STORE);
            file >> input;
            pushStringToCode(input);
        } else if (input.substr(0, 5) == "store") {
            storeHandler(input);
        } else if (input == "add") {
            code.push_back(ADD);
        } else if (input == "sub") {
            code.push_back(SUB);
        } else if (input == "print") {
            code.push_back(PRINT);
        } else if (input == "cmp") {
            code.push_back(CMP);
        } else if (input.substr(0, 3) == "jmp") {
            jumpHandler(input);
        } else if (input == "ret") {
            code.push_back(RET);
        } else if (input == "call") {
            code.push_back(CALL);
            file >> input;
            pushStringToCode(input);
        } else if (input == "func") {
            // The position in the code is equal to the size
            std::cout << "Creating function\n";
            int32_t position = code.size();
            Value pos = position;

            // Get the function name
            file >> input;
            store(pos, input);
        } else if (input == "label") {
            // The position in the code is equal to the size
            int32_t position = code.size();
            Value pos = position;

            // Get the label name
            file >> input;
            store(pos, input);
        } else if (input == "exit") {
            code.push_back(EXIT);
        } else {
            invalidInstructionHandler(input);
        }
    }

    return &code;
}

