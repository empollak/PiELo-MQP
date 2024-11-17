#include "stack-vm.h"


using namespace std;

// Compiler Functions

bool StackVM::isPrimitive(std::string s) {
    if (s.empty()) return false;

    if((s == "halt" || s == "HALT") || (s.find("PUSH") != std::string::npos || s.find("push")) || (s == "pop" || s == "POP") || 
		(s == "+" || s == "add" || s == "ADD") || (s == "-" || s == "sub" || s == "SUB") || (s == "*" || s == "mul" || s == "MUL") ||
		(s== "/" || s == "div" || s == "DIV") || (s== "%" || s == "mod" || s == "MOD") || (s == "=" || s == "eql" || s == "EQL") || 
		(s == "!=" || s == "neql" || s == "NEQL") || ( s == ">" || s == "gt" || s == "GT") || (s == ">=" || s == "gte" || s == "GTE") ||
		( s == "<" || s == "lt" || s == "LT") || (s == "<=" || s == "lte" || s == "LTE") || (s == "swap" || s == "SWAP") || (s == "dup" || s == "DUP") || 
		(s == "jump" || s == "JUMP" || s == "JMP" || s == "jmp") || (s == "jump_if_zero" || s == "JUMP_IF_ZERO" || s == "JMP_IF_ZERO" || s == "jmp_if_zero" || s == "jz" || s == "JZ") ||
		(s == "CALL" || s == "call") || (s == "RET" || s == "ret"))  {
			return true;
	}
	else{
		return false;
	}	
}

bool StackVM::isLabel(const std::string &token) {
    return (!token.empty() && (token.back() == ':' || token.front() == ':'));
}

bool StackVM::isCreatingFunction(const std::string &token) {
    return (!token.empty() && (token.front() == 'f' || token.front() == 'F') && token.back() == '@');
}

bool StackVM::isCallingFunction(const std::string &token) {
    return (!token.empty() && (token == "CALL"));
}

Op StackVM::convertToPrimitive(std::string s){
	//if (s.empty()) return HALT;

	if(s == "halt" || s == "HALT"){
		return Op::HALT;
	} else if(s.find("PUSHNULL") != std::string::npos){
		return Op::PUSHNULL;
	} else if(s.find("PUSHI") != std::string::npos){
		return Op::PUSHI;
	} else if(s.find("PUSHF") != std::string::npos){
		return Op::PUSHF;
	} else if(s.find("PUSHD") != std::string::npos){
		return Op::PUSHD;
	} else if(s == "POP"){
		return Op::POP;
	} else if(s == "+" || s == "add" || s == "ADD"){
		return Op::ADD;
	} else if(s == "-" || s == "sub" || s == "SUB"){
		return Op::SUB;
	} else if(s == "*" || s == "mul" || s == "MUL"){
		return Op::MUL;
	} else if(s== "/" || s == "div" || s == "DIV"){
		return Op::DIV;
	} else if(s== "%" || s == "mod" || s == "MOD"){
		return Op::MOD;
	} else if(s == "=" || s == "eql" || s == "EQL"){
		return Op::EQL;
	} else if(s == "!=" || s == "neql" || s == "NEQL"){
		return Op::NEQL;
	} else if( s == ">" || s == "gt" || s == "GT"){
		return Op::GT;
	} else if(s == ">=" || s == "gte" || s == "GTE"){
		return Op::GTE;
	} else if(s == "<" || s == "lt" || s == "LT"){
		return Op::LT;
	} else if(s == "<=" || s == "lte" || s == "LTE"){
		return Op::LTE;
	} else if(s == "swap" || s == "SWAP"){
		return Op::SWAP;
	} else if(s == "dup" || s == "DUP") {
		return Op::DUP;
	} else if(s == "jump" || s == "JUMP" || s == "JMP" || s == "jmp"){
		return Op::JMP;
	} else if(s == "jump_if_zero" || s == "JUMP_IF_ZERO" || s == "JMP_IF_ZERO" || s == "jmp_if_zero" || s == "jz" || s == "JZ"){
		return Op::JZ;
	} else if(s == "call" || s == "CALL"){
		return Op::CALL;
	} else if(s == "ret" || s == "RET"){
		return Op::RET;
	}
	else {
		throw InvalidPrimitiveException(s);
	}

}

std::vector<std::string> StackVM::tokenizer(const std::string &line){
	std::vector<std::string> tokens;
	std::stringstream ss(line);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }

    return tokens;
	for(int i = 0; i < tokens.size(); i++){
		std:: cout << tokens[i] << " ";
	}
	std::cout << std::endl;
	
}

std::tuple<std::string, std::string, std::string> StackVM::analyzeLine(std::string s) {
	
	vector<std::string> tokens = tokenizer(s);
	bool lookingForParams = false;

	if (tokens.empty()) {
		return std::make_tuple("", "", "");  // Return empty strings in the tuple
    } else{
		std::string label = "";
		std::string operation = "";
		std::string operand = "";
		if(isCreatingFunction(tokens[1])){

			operand = tokens[0]; // store the return type of the function as the operand

			for(int h = 0; h < tokens[2].size(); h++){

				if(!lookingForParams){
					if (tokens[2][h] == '(') {
						lookingForParams = true;
						operation += tokens[2][h];
        			} else{
						label += tokens[2][h]; // store the name of the function in the label variable
					}
				} else{
					operation += tokens[2][h];
				}
				
				
			}
			
			if(tokens.size() > 2){
				for(int p = 3; p < tokens.size(); p++){
					
					operation += ' ';
					for(int d = 0; d < tokens[p].size(); d++){
						
						if (tokens[p][d] == ')') {
							operation += tokens[p][d];
							break;
						} 
						else{
							operation += tokens[p][d];
						}
					
					}
				
				}
			} else{
				operation = "()";
			}
			
			// if(tokens.size() > 2){
			// 	operation = tokens[3];
			// 	operand = tokens[0];
			// } else {
			// 	operand = tokens[0];
			// }
		}
		// Controlla il primo token
		else if (isLabel(tokens[0])) {
			// Check if the token ends with ':'
			if (tokens[0].back() == ':') {
				// Remove the ':' and store only the label part
				label = tokens[0].substr(0, tokens[0].size() - 1);
			} else {
				label = tokens[0];
			}
			if(tokens.size() > 1){
				operation = tokens[1];
				if (tokens.size() > 2) {
					operand = tokens[2];
				}
			}
		}
		else {
			operation = tokens[0];

			if(isCallingFunction(operation)){
				if(tokens.size() > 1){
					for(int p = 1; p < tokens.size(); p++){
						operand += ' ';
						for(int d = 0; d < tokens[p].size(); d++){
							
							if (tokens[p][d] == ')') {
								operand += tokens[p][d];
								break;
							} 
							else{
								operand += tokens[p][d];
							}
						
						}
					}
				} else{
					operation = "()";
				}
			} 
			else{
				if(tokens.size() > 1){
					operand = tokens[1];
				}
			}
			
		}
		std::cout << "Label: " << label << "   Operat: " << operation << "   Operand: " << operand << std::endl;
		return make_tuple(label, operation, operand);
	}

	
}

void StackVM::printProgram() {
	std::cout << std::endl;
    for (int i = 0; i < policy.size(); i++) {

		std::cout << "Instruction @ Line " << i << ": ";

        for (int j = 0; j < policy[i].size(); j++) {

			printTaggedValue(policy[i][j]);
			std::cout  << " ";
        }
        std::cout << std::endl;
    }
}

void StackVM::printMap(){
	std::cout << std::endl;
	std::cout << "Map contents:" << std::endl;
    for (const auto& pair : labelMap) {
        std::cout << "Label: " << pair.first << ", PC Address: " << pair.second << std::endl;
    }
}


// Create and initialize a FunctionInfo instance
FunctionInfo StackVM::createFunctionInfo(std::string label, std::string operation, int address) {
    std::vector<ValueType> variables_types;  // Vector to store types
    std::vector<std::string> variables_names;
            // FIND VARIABLES NAMES
    size_t start = operation.find('(');
    size_t end = operation.find(')');
    
    if (start == std::string::npos || end == std::string::npos || start >= end) {
        // no params
    }

    std::string params = operation.substr(start + 1, end - start - 1);  // Extract substring between parentheses
    std::stringstream ss(params);
    std::string type, var;

    while (ss >> type) {  // Read each type
        if (ss.peek() == ',') {  // Check if the next character is a comma
            ss.ignore();  // Ignore the comma
        }
        
        // Read the variable name
        ss >> var;  // Read the variable name

        // Remove any trailing commas from the variable name
        if (var.back() == ',') {
            var.pop_back();
        }
        
        variables_names.push_back(var);  // Store the variable name

        if(type == "int" || type == "INT"){
            variables_types.push_back(INT);
        } else if(type == "float" || type == "FLOAT"){
            variables_types.push_back(FLOAT);
        } else if(type == "double" || type == "DOUBLE"){
            variables_types.push_back(DOUBLE);
        } else if(type == "string" || type == "STRING"){
            variables_types.push_back(STRING);
        } else if(type == "nil" || type == "null" || type == "NIL"){
            variables_types.push_back(NIL);
        }
    }

    return {label, variables_names, variables_types, address};
}


void StackVM::loadInstructions(strings s){//, StackVM::StackVM vm){

	bool amIinFunction = false;

	for(int i = 0; i < s.size(); i++){

		std::vector<TaggedValue> instruction_line;
		std::tuple<std::string, std::string, std::string> line_analysis = analyzeLine(s[i]);

		// Accedi agli elementi della tupla
        std::string label = std::get<0>(line_analysis);     // Ottieni il primo elemento
        std::string operation = std::get<1>(line_analysis); // Ottieni il secondo elemento
        std::string operand = std::get<2>(line_analysis);   // Ottieni il terzo elemento

		// Stampa il risultato dell'analisi
    	if (!label.empty()) {
        	//std::cout << "Label: " << label << "	";
			labelMap[label] = i; 
    	}
		
		if (!operation.empty()) {
			//std::cout << "Operazione: " << operation <<  "	";
			if(operation.at(0) == '('){	// if we are creating a function
				amIinFunction = true;
				function_table[label] = createFunctionInfo(label, operation, i); 

			}
			else if(isPrimitive(operation)){
				//Op instruction = tok2op(operation);
				if(amIinFunction){
					if(operation != "pushi" && operation != "PUSHI" &&  operation != "pushf" && operation != "PUSHF" && 
					operation != "pushd" && operation != "PUSHD"){
						instruction_line.push_back(TaggedValue(operation));
					}
					
				}
				
			} else {
				throw InvalidPrimitiveException(operation);
				
			}
    	}

		if (!operand.empty()) {

			//std::cout << "Operando: " << operand << "\n";
			if(operand == "INT" || operand == "FLOAT" || operand == "DOUBLE" || operand == "STRING" || operand == "NIL"){
				//do nothing
			}
			else if(operation == "CALL" || operation == "call"){

				size_t start = 0;
				while (start < operand.length() && std::isspace(operand[start])) {
					++start;
				}

				// Find the position of the first '(' after any leading spaces
				size_t pos = operand.find('(', start);

				// Return the substring from the first non-space character to '(' or the end of the string
				std::string function_call_name = (pos != std::string::npos) ? operand.substr(start, pos - start) : operand.substr(start);

				TaggedValue instruction(function_call_name);
				instruction_line.push_back(instruction);

				// Extract values
				std::vector<TaggedValue> values;

				// Find the opening and closing parentheses
				size_t open_paren = operand.find('(');
				size_t close_paren = operand.find(')');

				// Extract the arguments string
				std::string arg_str = operand.substr(open_paren + 1, close_paren - open_paren - 1);

				// Create a string stream to parse the arguments
				std::stringstream ss(arg_str);

				// Parse each argument and push it into the vector
				int arg;
				while (ss >> arg) {
					values.push_back(arg);

					// Skip the comma if there's another argument
					char comma;
					ss >> comma;
				}

				if(function_table.find(function_call_name) != function_table.end()){
					
					for(int i = 0; i < function_table[function_call_name].paramNames.size(); i++){
						if(function_table[function_call_name].paramTypes[i] == INT){
							TaggedValue instruction = values[i];
							instruction_line.push_back(instruction);
						} else if(function_table[function_call_name].paramTypes[i] == FLOAT){
							TaggedValue instruction = values[i];
							instruction_line.push_back(instruction);
						} else if(function_table[function_call_name].paramTypes[i] == DOUBLE){
							TaggedValue instruction = values[i];
							instruction_line.push_back(instruction);
						} else if(function_table[function_call_name].paramTypes[i] == STRING){
							TaggedValue instruction = values[i];
							instruction_line.push_back(instruction);
						}
					}
					

				} else {
					throw FunctionNotDefinedException(function_call_name);
				}


			} else if((operation == "JUMP" || operation == "JMP" || operation == "jump" || operation == "jmp") || (operation == "jump_if_zero" || operation == "JUMP_IF_ZERO" || operation == "JMP_IF_ZERO" || operation == "jmp_if_zero" || operation == "jz" || operation == "JZ")){
				TaggedValue instruction(operand);
				instruction_line.push_back(instruction);

			} else if(operation == "PUSHI" || operation == "pushi"){
				if(amIinFunction){
					try{
						TaggedValue instruction(stoi(operand));
						instruction_line.push_back(instruction);
					} catch(const std::exception& e){
						// if the operand is a variable we do nothing, because we already are pushing onto the stack the parameteres when the function is called.
					}
				} else {
					TaggedValue instruction(stoi(operand));
					instruction_line.push_back(instruction);
				}

			} else if(operation == "PUSHF" || operation == "pushf"){

				if(amIinFunction){
					try{
						TaggedValue instruction(stof(operand));
						instruction_line.push_back(instruction);
					} catch(const std::exception& e){
						// if the operand is a variable we do nothing, because we already are pushing onto the stack the parameteres when the function is called.
					}
				} else {
					TaggedValue instruction(stof(operand));
					instruction_line.push_back(instruction);
				}

			} else if(operation == "PUSHD" || operation == "pushd"){
				if(amIinFunction){
					try{
						TaggedValue instruction(stod(operand));
						instruction_line.push_back(instruction);
					} catch(const std::exception& e){
						// if the operand is a variable we do nothing, because we already are pushing onto the stack the parameteres when the function is called.
					}
				} else {
					TaggedValue instruction(stod(operand));
					instruction_line.push_back(instruction);
				}
			} else {
				throw InvalidOperandException(operation);
			}

		}

		if (label.empty() && operation.empty()) {
			throw EmptyInstructionLineException();
		}

		policy.push_back(instruction_line);
		//std::cout << std::endl;
	}

	if(policy[policy.size() - 1][0].getString() != "HALT"){
		vector<TaggedValue> end_of_program;
		end_of_program.push_back(TaggedValue("HALT"));
		policy.push_back(end_of_program); // always halt at the end
	}

	printProgram();
	printMap();
}

// Stack VM functions

StackVM::StackVM() {
	policy.reserve(1000000);
}

TaggedValue StackVM::getData(TaggedValue instruction_token) {
	TaggedValue data;
	if(instruction_token.getType() == INT){
		int i_data = 0x3fffffff; // 30 bits
		data = i_data & instruction_token.getInt();
	}
	else if(instruction_token.getType() == FLOAT){
		float f_data = instruction_token.getFloat();
		data = TaggedValue(f_data);
	}
	else if(instruction_token.getType() == DOUBLE){
		double d_data = instruction_token.getDouble();
		data = TaggedValue(d_data);
	}
	else if(instruction_token.getType() == STRING){
		string s_data = instruction_token.getString();
		data = TaggedValue(s_data);
	} else if(instruction_token.getType() == NIL){
		data = TaggedValue();
	} 
	else {
		throw InvalidTypeException();
		status = ERROR;
	}

	// i32 data = 0x3fffffff; // 30 bits
	// data = data & instruction_token;
	return data;
}

void StackVM::fetch() {
	pc++;
}

void StackVM::decode(TaggedValue instruction_token) {
	//typ = getType(instruction_token);
	dat = getData(instruction_token);
	//printTaggedValue(instruction_token);
	
}

void StackVM::execute(TaggedValue instruction_token) {

	//if(instruction_token.getType() == STRING && instruction_token.getString().at(0) == '(')


	if(instruction_token.getType() == STRING && isPrimitive(instruction_token.getString())){
		//Op operation = tok2op(instruction_token.getString());
		//std::cout << instruction_token.getString() << " ";
		Op operation = convertToPrimitive(instruction_token.getString());
		doPrimitive(operation);
		
	}
    else if ((instruction_token.getType() == INT) || (instruction_token.getType() == FLOAT) || (instruction_token.getType() == DOUBLE) || (instruction_token.getType() == STRING) || (instruction_token.getType() == NIL)){
        //float value = static_cast<float>(getData(instruction_token)); // Cast to float and push
		printTaggedValue(instruction_token);
		std::cout << "	";
        stack.push(getData(instruction_token));
    } 
	else{
		throw InvalidTypeException();
		status = ERROR;
	}
	
}

void StackVM::pushNull_operation(){
	std::cout << "PUSH NULL" << "	";
	stack.push(TaggedValue());

}

void StackVM::pushi_operation(){
	int int_value = 0;
	std::cout << "PUSH INT: ";
 	// move to the next instruction that contains the value
	tc++;
	int_value = policy[pc][tc].getInt();

	stack.push(TaggedValue(int_value)); // push the actual value onto the stack
	std::cout << int_value  << "	";

}

void StackVM::pushf_operation(){
	float float_value = 0.0f;
	std::cout << "PUSH FLOAT: ";
 	// move to the next instruction that contains the value
	tc++;
	float_value = policy[pc][tc].getFloat();

	stack.push(TaggedValue(float_value)); // push the actual value onto the stack
	std::cout << float_value << "	"; 

}

void StackVM::pushd_operation(){
	double double_value = 0;
	std::cout << "PUSH DOUBLE: ";
 	// move to the next instruction that contains the value
	tc++;
	double_value = policy[pc][tc].getDouble();

	stack.push(TaggedValue(double_value)); // push the actual value onto the stack
	std::cout << double_value << "	";  

}

void StackVM::pop_operation(){

	if(!stack.empty()){
		std::cout << "POP: ";
		switch (stack.top().type) {
			case INT: cout << stack.top().getInt(); break;
            case FLOAT: cout << stack.top().getFloat(); break;
            case DOUBLE: cout << stack.top().getDouble(); break;
            case STRING: cout << stack.top().getString(); break;
            default: cout << "NULL"; break;

		}
		cout << "	";
		stack.pop();
		//pc++;
	} else {
		throw PopEmptyStackException();
		status = ERROR;
	}
}

void StackVM::add_operation(){
	if(stack.size() >= 2) {

		TaggedValue a = stack.top(); stack.pop();
		TaggedValue b = stack.top(); stack.pop();

		// Check if the values' types is acceptable for the ADD operation
		if(a.type == STRING || b.type == STRING) {
			InvalidTypeForOperationException("ADD", "STRING");
			status = ERROR;
		} else if(a.type == NIL || b.type == NIL) {
			InvalidTypeForOperationException("ADD", "NIL");
			status = ERROR;
		}
		// Handle ADD as a DOUBLE
		else if(a.type == DOUBLE && b.type != DOUBLE){
			// Perform addition as doubles
            double result = a.getDouble() + static_cast<double>(b.type == FLOAT ? b.as.floatValue : b.as.intValue);
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "ADD (double): " << a.getDouble() << " + " << static_cast<double>(b.type == FLOAT ? b.as.floatValue : b.as.intValue) << " = " << result << "	";
		}
		else if(a.type != DOUBLE && b.type == DOUBLE){
			// Perform addition as doubles
            double result = static_cast<double>(a.type == FLOAT ? a.as.floatValue : a.as.intValue) + b.getDouble();
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "ADD (double): " << static_cast<double>(a.type == FLOAT ? a.as.floatValue : a.as.intValue) << " + " << b.getDouble() << " = " << result  << "	";
		}
		else if(a.type == DOUBLE && b.type == DOUBLE){
			// Perform addition as doubles
            double result = a.getDouble() + b.getDouble();
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "ADD (double): " << a.getDouble() << " + " << b.getDouble() << " = " << result << "	";
		}
		// Handle ADD as a FLOAT
		else if(a.type == FLOAT && b.type != FLOAT){
			// Perform addition as doubles
            float result = a.getFloat() + static_cast<float>(b.getInt());
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "ADD (float): " << a.getFloat() << " + " << static_cast<float>(b.getInt()) << " = " << result <<  "	";
		}
		else if(a.type != FLOAT && b.type == FLOAT){
			// Perform addition as doubles
            float result = static_cast<float>(a.getInt()) + b.getFloat();
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "ADD (float): " << static_cast<float>(a.getInt())  << " + " << b.getFloat() << " = " << result << "	";
		}
		else if(a.type == FLOAT && b.type == FLOAT){
			// Perform addition as float
            float result = a.getFloat() + b.getFloat();
            stack.push(TaggedValue(result));  // Push result as float
            std::cout << "ADD (float): " << a.getFloat() << " + " << b.getFloat() << " = " << result << "	";
		}
		// Handle ADD as an INT
		else{
			//Perform addition as integers
			int result = a.getInt() + b.getInt();
			stack.push(TaggedValue(result));  // Push result as int
			std::cout << "ADD (int): " << a.as.intValue << " + " << b.as.intValue << " = " << result << "	";
		}

	}
	else {
		throw ShortOnElementsOnStackException("ADD");
		status = ERROR;
	}

}

void StackVM::sub_operation(){
	if(stack.size() >= 2) {

		TaggedValue a = stack.top(); stack.pop();
		TaggedValue b = stack.top(); stack.pop();

		// Check if the values' types is acceptable for the SUB operation
		if(a.type == STRING || b.type == STRING) {
			throw InvalidTypeForOperationException("SUB", "STRING");
			status = ERROR;
		} 
		else if(a.type == NIL || b.type == NIL) {
			throw InvalidTypeForOperationException("SUB", "NIL");
			status = ERROR;
		}
		// Handle SUB as a DOUBLE
		else if(a.type == DOUBLE && b.type != DOUBLE){

            double result = static_cast<double>(b.type == FLOAT ? b.as.floatValue : b.as.intValue) - a.getDouble();
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "SUB (double): " << static_cast<double>(b.type == FLOAT ? b.as.floatValue : b.as.intValue)  << " - " << a.getDouble() << " = " << result << "	";
		}
		else if(a.type != DOUBLE && b.type == DOUBLE){

            double result = b.getDouble() - static_cast<double>(a.type == FLOAT ? a.as.floatValue : a.as.intValue);
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "SUB (double): " << b.getDouble() << " - " << static_cast<double>(a.type == FLOAT ? a.as.floatValue : a.as.intValue) << " = " << result << "	";
		}
		else if(a.type == DOUBLE && b.type == DOUBLE){

            double result = b.getDouble() - a.getDouble();
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "SUB (double): " << b.getDouble() << " - " << a.getDouble() << " = " << result << "	";
		}
		// Handle SUB as a FLOAT
		else if(a.type == FLOAT && b.type != FLOAT){

            float result = static_cast<float>(b.getInt()) - a.getFloat();
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "SUB (float): " << static_cast<float>(b.getInt()) << " - " << a.getFloat() << " = " << result  << "	";
		}
		else if(a.type != FLOAT && b.type == FLOAT){
	
            float result = b.getFloat() - static_cast<float>(a.getInt());
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "SUB (float): " << b.getFloat() << " - " << static_cast<float>(a.getInt())  << " = " << result << "	";
		}
		else if(a.type == FLOAT && b.type == FLOAT){

            float result = b.getFloat() - a.getFloat();
            stack.push(TaggedValue(result));  // Push result as float
            std::cout << "SUB (float): " << b.getFloat() << " - " << a.getFloat() << " = " << result << "	";
		}
		// Handle SUB as an INT
		else{
			//Perform addition as integers
			int result = b.as.intValue - a.as.intValue;
			stack.push(TaggedValue(result));  // Push result as int
			std::cout << "SUB (int): " << b.as.intValue << " - " << a.as.intValue << " = " << result << "	";
		}

	}
	else {
		throw ShortOnElementsOnStackException("SUB");
		status = ERROR;
	}

}

void StackVM::mul_operation(){
	if(stack.size() >= 2) {

		TaggedValue a = stack.top(); stack.pop();
		TaggedValue b = stack.top(); stack.pop();

		// Check if the values' types is acceptable for the MUL operation
		if(a.type == STRING || b.type == STRING) {
			throw InvalidTypeForOperationException("MUL", "STRING");
			status = ERROR;
		}
		else if(a.type == NIL || b.type == NIL) {
			throw InvalidTypeForOperationException("MUL", "NIL");
		}
		// Handle MUL as a DOUBLE
		else if(a.type == DOUBLE && b.type != DOUBLE){

            double result = a.getDouble() * static_cast<double>(b.type == FLOAT ? b.as.floatValue : b.as.intValue);
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "MUL (double): " << a.getDouble() << " * " << static_cast<double>(b.type == FLOAT ? b.as.floatValue : b.as.intValue) << " = " << result << "	";
		}
		else if(a.type != DOUBLE && b.type == DOUBLE){
			
            double result = static_cast<double>(a.type == FLOAT ? a.as.floatValue : a.as.intValue) * b.getDouble();
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "MUL (double): " << static_cast<double>(a.type == FLOAT ? a.as.floatValue : a.as.intValue) << " * " << b.getDouble() << " = " << result << "	";
		}
		else if(a.type == DOUBLE && b.type == DOUBLE){
			
            double result = a.getDouble() * b.getDouble();
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "MUL (double): " << a.getDouble() << " * " << b.getDouble() << " = " << result << "	";
		}
		// Handle MUL as a FLOAT
		else if(a.type == FLOAT && b.type != FLOAT){
			
            float result = a.getFloat() * static_cast<float>(b.getInt());
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "MUL (float): " << a.getFloat() << " * " << static_cast<float>(b.getInt()) << " = " << result <<  "	";
		}
		else if(a.type != FLOAT && b.type == FLOAT){
			
            float result = static_cast<float>(a.getInt()) * b.getFloat();
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "MUL (float): " << static_cast<float>(a.getInt())  << " * " << b.getFloat() << " = " << result << "	";
		}
		else if(a.type == FLOAT && b.type == FLOAT){
			
            float result = a.getFloat() * b.getFloat();
            stack.push(TaggedValue(result));  // Push result as float
            std::cout << "MUL (float): " << a.getFloat() << " * " << b.getFloat() << " = " << result << "	";
		}
		// Handle MUL as an INT
		else{
			
			int result = a.getInt() * b.getInt();
			stack.push(TaggedValue(result));  // Push result as int
			std::cout << "MUL (int): " << a.as.intValue << " * " << b.as.intValue << " = " << result << "	";
		}

	}
	else {
		throw ShortOnElementsOnStackException("MUL");
		status = ERROR;
	}

}

void StackVM::div_operation(){
	if(stack.size() >= 2) {

		TaggedValue a = stack.top(); stack.pop();
		TaggedValue b = stack.top(); stack.pop();

		// Check if the values' types is acceptable for the ADD operation
		if(a.type == STRING || b.type == STRING) {
			throw InvalidTypeForOperationException("DIV", "STRING");
			status = ERROR;
		} else if(a.type == NIL || b.type == NIL) {
			throw InvalidTypeForOperationException("DIV", "NIL");
			status = ERROR;
		}
		//Check for Divisions by 0
		else if((a.type == INT && a.as.intValue == 0) || 
				(a.type == FLOAT && a.as.floatValue == 0.0f) ||
				(a.type == DOUBLE && a.as.doubleValue == 0.0)){
			throw DivisionByZeroException();
		}
		// Handle DIV as a DOUBLE
		else if(a.type == DOUBLE && b.type != DOUBLE){

            double result = static_cast<double>(b.type == FLOAT ? b.as.floatValue : b.as.intValue) / a.getDouble();
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "DIV (double): " << static_cast<double>(b.type == FLOAT ? b.as.floatValue : b.as.intValue)  << " / " << a.getDouble() << " = " << result << "	";
		}
		else if(a.type != DOUBLE && b.type == DOUBLE){

            double result = b.getDouble() / static_cast<double>(a.type == FLOAT ? a.as.floatValue : a.as.intValue);
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "DIV (double): " << b.getDouble() << " / " << static_cast<double>(a.type == FLOAT ? a.as.floatValue : a.as.intValue) << " = " << result << "	";
		}
		else if(a.type == DOUBLE && b.type == DOUBLE){

            double result = b.getDouble() / a.getDouble();
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "DIV (double): " << b.getDouble() << " / " << a.getDouble() << " = " << result << "	";
		}
		// Handle DIV as a FLOAT
		else if(a.type == FLOAT && b.type != FLOAT){

            float result = static_cast<float>(b.getInt()) / a.getFloat();
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "DIV (float): " << static_cast<float>(b.getInt()) << " / " << a.getFloat() << " = " << result  << "	";
		}
		else if(a.type != FLOAT && b.type == FLOAT){
	
            float result = b.getFloat() / static_cast<float>(a.getInt());
            stack.push(TaggedValue(result));  // Push result as double
            std::cout << "DIV (float): " << b.getFloat() << " / " << static_cast<float>(a.getInt())  << " = " << result << "	";
		}
		else if(a.type == FLOAT && b.type == FLOAT){

            float result = b.getFloat() / a.getFloat();
            stack.push(TaggedValue(result));  // Push result as float
            std::cout << "DIV (float): " << b.getFloat() << " / " << a.getFloat() << " = " << result << "	";
		}
		// Handle ADD as an INT
		else{
			//Perform addition as integers
			int result = b.getInt() / a.getInt();
			stack.push(TaggedValue(result));  // Push result as int
			std::cout << "DIV (int): " << b.as.intValue << " / " << a.as.intValue << " = " << result << "	";
		}

	}
	else {
		throw ShortOnElementsOnStackException("DIV");
		status = ERROR;
	}

}

void StackVM::mod_operation() {
	if(stack.size() >= 2) {
		
		TaggedValue a = stack.top(); stack.pop();
		TaggedValue b = stack.top(); stack.pop();

		// Check if either of the values is not an integer
		if(a.type != INT || b.type != INT) {
			throw InvalidTypeForOperationException("MOD", "!= than INT. Only INT is supported");
			status = ERROR;
		}
		// Check for division by zero
		else if (a.as.intValue == 0) {
			throw DivisionByZeroException();
			status = ERROR;
		}
		else {
			// Perform modulo operation
			int result = b.getInt() % a.getInt();
			stack.push(TaggedValue(result));  // Push result as int
			std::cout << "MOD: " << b.getInt() << " % " << a.getInt() << " = " << result << "	";
		}
	}
	else {
		throw ShortOnElementsOnStackException("MOD");
		status = ERROR;
	}
}

void StackVM::eql_operation(){
	if(stack.size() >= 2) {
		
		TaggedValue a = stack.top(); stack.pop();
		TaggedValue b = stack.top(); stack.pop();

		// Check for comparisons with STRINGS
		if((a.type == STRING && b.type != STRING) || (a.type != STRING && b.type == STRING)){
			stack.push(0);
			//throw InvalidTypeForOperationException("EQL", "STRING");
		}	
		else if(a.type == STRING && b.type == STRING){
			stack.push(a.getString() == b.getString() ? 1:0);
			std::cout << "EQL (string): " << a.getString() << " == " << b.getString() << " ? " << (a.getString() == b.getString() ? 1:0) << "	";
		}
		// Check for comparisons with NULL
		else if((a.type == NIL && b.type != NIL) || (a.type != NIL && b.type == NIL)){
			stack.push(0);
			//throw InvalidTypeForOperationException("EQL", "NIL");
		}
		else if(a.type == NIL && b.type == NIL){
			stack.push(1);
			std::cout << "EQL (NIL): NULL == NULL 1" << "	";
		}

		// Check if they are equal as double
		else if(a.type == DOUBLE && b.type != DOUBLE){
			double conversion_value = static_cast<double>(b.type == FLOAT ? b.getFloat() : b.getInt());
			stack.push(a.getDouble() == conversion_value ? 1 : 0);
			std::cout << "EQL (double): " << a.getDouble() << " == " << conversion_value << " ? " << (a.getDouble() == conversion_value ? 1 : 0) << std::endl;
		}
		else if(a.type != DOUBLE && b.type == DOUBLE){
			double conversion_value = static_cast<double>(a.type == FLOAT ? a.as.floatValue : a.as.intValue);
			stack.push(conversion_value == b.getDouble()? 1 : 0);
			std::cout << "EQL (double): " << conversion_value << " == " << b.getDouble() << " == " << conversion_value << " ? " << (conversion_value == b.getDouble() ? 1 : 0) << std::endl;
		}
		else if(a.type == DOUBLE && b.type == DOUBLE){
			stack.push(a.getDouble() == b.getDouble() ? 1 : 0);
			std::cout << "EQL (double): " << a.getDouble() << " == " << b.getDouble() << " ? " << (a.getDouble() == b.getDouble() ? 1 : 0) << std::endl;
		}

		// Check if they are equal as floats
		else if(a.type == FLOAT && b.type != FLOAT){
			float conversion_value = static_cast<float>(b.as.intValue);
			stack.push(a.getFloat() == conversion_value ? 1 : 0);
			std::cout << "EQL (float): " << a.getFloat() << " == " << conversion_value << " ? " << (a.getFloat() == conversion_value ? 1 : 0) << std::endl;
		}
		else if(a.type != FLOAT && b.type == FLOAT){
			float conversion_value = static_cast<float>(a.as.intValue);
			stack.push(conversion_value == b.getFloat() ? 1 : 0);
			std::cout << "EQL (float): " << conversion_value << " == " << b.getFloat() << " ? " << (conversion_value == b.getFloat()? 1 : 0) << std::endl;
		}
		else if(a.type == FLOAT && b.type == FLOAT){
			stack.push(a.getFloat() == b.getFloat()? 1 : 0);
			std::cout << "EQL (float): " << a.getFloat() << " == " << b.getFloat() << " ? " << (a.getFloat() == b.getFloat()? 1 : 0) << std::endl;
		}

		else{
			stack.push(a.getInt() == b.getInt()? 1 : 0);
			std::cout << "EQL (int): " << a.getInt() << " == " << b.getInt() << " ? " << (a.getInt() == b.getInt()? 1 : 0) << std::endl;
		}

	}
	else {
		throw ShortOnElementsOnStackException("EQL");
		status = ERROR;
	}

			
}

void StackVM::neql_operation(){
	if(stack.size() >= 2) {
		
		TaggedValue a = stack.top(); stack.pop();
		TaggedValue b = stack.top(); stack.pop();

		// Check for comparisons with STRINGS
		if((a.type == STRING && b.type != STRING) || (a.type != STRING && b.type == STRING)){
			int val = 1;
			stack.push(TaggedValue(val));
			//throw InvalidTypeForOperationException("NEQL", "STRING");
		}
		else if(a.type == STRING && b.type == STRING){
			stack.push(a.getString() != b.getString() ? 1:0);
			std::cout << "NEQL (string): " << a.getString() << " != " << b.getString() << " ? " << (a.getString() != b.getString() ? 1:0) << std::endl;
		}
		//Check for comparisons with NULL
		else if((a.type == NIL && b.type != NIL) || (a.type != NIL && b.type == NIL)){
			stack.push(1);
			//throw InvalidTypeForOperationException("NEQL", "NIL");
		}
		else if(a.type == NIL && b.type == NIL){
			stack.push(0);
			std::cout << "NEQL (NIL): NULL != NULL 0" << std::endl;
		}

		// Check if they are equal as double
		else if(a.type == DOUBLE && b.type != DOUBLE){
			double conversion_value = static_cast<double>(b.type == FLOAT ? b.as.floatValue : b.as.intValue);
			stack.push(a.getDouble() != conversion_value ? 1 : 0);
			std::cout << "NEQL (double): " << a.getDouble() << " != " << conversion_value << " ? " << (a.getDouble() != conversion_value ? 1 : 0) << std::endl;
		}
		else if(a.type != DOUBLE && b.type == DOUBLE){
			double conversion_value = static_cast<double>(a.type == FLOAT ? a.as.floatValue : a.as.intValue);
			stack.push(conversion_value != b.getDouble()? 1 : 0);
			std::cout << "NEQL (double): " << conversion_value << " != " << b.getDouble() << " != " << conversion_value << " ? " << (conversion_value != b.getDouble() ? 1 : 0) << std::endl;
		}
		else if(a.type == DOUBLE && b.type == DOUBLE){
			stack.push(a.getDouble() != b.getDouble() ? 1 : 0);
			std::cout << "NEQL (double): " << a.getDouble() << " != " << b.getDouble() << " ? " << (a.getDouble() != b.getDouble() ? 1 : 0) << std::endl;
		}

		// Check if they are equal as floats
		else if(a.type == FLOAT && b.type != FLOAT){
			float conversion_value = static_cast<float>(b.as.intValue);
			stack.push(a.getFloat() != conversion_value ? 1 : 0);
			std::cout << "NEQL (float): " << a.getFloat() << " != " << conversion_value << " ? " << (a.getFloat() != conversion_value ? 1 : 0) << std::endl;
		}
		else if(a.type != FLOAT && b.type == FLOAT){
			float conversion_value = static_cast<float>(a.as.intValue);
			stack.push(conversion_value != b.getFloat() ? 1 : 0);
			std::cout << "NEQL (float): " << conversion_value << " != " << b.getFloat() << " ? " << (conversion_value != b.getFloat() ? 1 : 0) << std::endl;
		}
		else if(a.type == FLOAT && b.type == FLOAT){
			stack.push(a.getFloat() != b.getFloat()? 1 : 0);
			std::cout << "NEQL (float): " << a.getFloat() << " != " << b.getFloat() << " ? " << (a.getFloat() != b.getFloat() ? 1 : 0) << std::endl;
		}

		else{
			stack.push(a.getInt() != b.getInt()? 1 : 0);
			std::cout << "NEQL (int): " << a.getInt() << " != " << b.getInt() << " ? " << (a.getInt() != b.getInt()? 1 : 0)  << std::endl;
		}

	}
	else {
		throw ShortOnElementsOnStackException("NEQL");
		status = ERROR;
	}

			
}

void StackVM::gt_operation(){
	if(stack.size() >= 2) {
		
		TaggedValue a = stack.top(); stack.pop();
		TaggedValue b = stack.top(); stack.pop();

		// Check for comparisons with STRINGS
		if((a.type == STRING && b.type != STRING) || (a.type != STRING && b.type == STRING)){
			throw InvalidTypeForOperationException("GT", "STRING");
			status = ERROR;
		}
		else if(a.type == STRING && b.type == STRING){
			stack.push(a.getString() > b.getString() ? 1 : 0);
			std::cout << "GT (string): " << a.getString() << " > " << b.getString() << " ? " << (a.getString() > b.getString() ? 1:0) << std::endl;
		}
		// Check for comparisons with NULL
		else if((a.type == NIL && b.type != NIL) || (a.type != NIL && b.type == NIL)){
			throw InvalidTypeForOperationException("GT", "NIL");
			status = ERROR;
		}
		else if(a.type == NIL && b.type == NIL){
			stack.push(0);
			std::cout << "GT (NIL): NULL > NULL 0" << std::endl;
		}

		// Check if they are equal as double
		else if(a.type == DOUBLE && b.type != DOUBLE){
			double conversion_value = static_cast<double>(b.type == FLOAT ? b.as.floatValue : b.as.intValue);
			stack.push(a.getDouble() > conversion_value ? 1 : 0);
			std::cout << "GT (double): " << a.getDouble() << " > " << conversion_value << " ? " << (a.getDouble() > conversion_value ? 1 : 0) << std::endl;
		}
		else if(a.type != DOUBLE && b.type == DOUBLE){
			double conversion_value = static_cast<double>(a.type == FLOAT ? a.as.floatValue : a.as.intValue);
			stack.push(conversion_value > b.getDouble()? 1 : 0);
			std::cout << "GT (double): " << conversion_value << " > " << b.getDouble() << " > " << conversion_value << " ? " << (conversion_value > b.getDouble() ? 1 : 0) << std::endl;
		}
		else if(a.type == DOUBLE && b.type == DOUBLE){
			stack.push(a.getDouble() > b.getDouble() ? 1 : 0);
			std::cout << "GT (double): " << a.getDouble() << " > " << b.getDouble() << " ? " << (a.getDouble() > b.getDouble() ? 1 : 0) << std::endl;
		}

		// Check if they are equal as floats
		else if(a.type == FLOAT && b.type != FLOAT){
			float conversion_value = static_cast<float>(b.as.intValue);
			stack.push(a.getFloat() > conversion_value ? 1 : 0);
			std::cout << "GT (float): " << a.getFloat() << " > " << conversion_value << " ? " << (a.getFloat() > conversion_value ? 1 : 0) << std::endl;
		}
		else if(a.type != FLOAT && b.type == FLOAT){
			float conversion_value = static_cast<float>(a.as.intValue);
			stack.push(conversion_value > b.getFloat() ? 1 : 0);
			std::cout << "GT (float): " << conversion_value << " > " << b.getFloat() << " ? " << (conversion_value > b.getFloat() ? 1 : 0) << std::endl;
		}
		else if(a.type == FLOAT && b.type == FLOAT){
			stack.push(a.getFloat() > b.getFloat()? 1 : 0);
			std::cout << "GT (float): " << a.getFloat() << " > " << b.getFloat() << " ? " << (a.getFloat() > b.getFloat() ? 1 : 0) << std::endl;
		}

		else{
			stack.push(a.getInt() > b.getInt()? 1 : 0);
			std::cout << "GT (int): " << a.getInt() << " > " << b.getInt() << " ? " << (a.getInt() > b.getInt()? 1 : 0)  << std::endl;
		}

	}
	else {
		throw ShortOnElementsOnStackException("GT");
		status = ERROR;
	}

			
}

void StackVM::gte_operation(){
	if(stack.size() >= 2) {
		
		TaggedValue a = stack.top(); stack.pop();
		TaggedValue b = stack.top(); stack.pop();

		// Check for comparisons with STRINGS
		if((a.type == STRING && b.type != STRING) || (a.type != STRING && b.type == STRING)){
			throw InvalidTypeForOperationException("GTE", "STRING");
			status = ERROR;
		}
		else if(a.type == STRING && b.type == STRING){
			stack.push(a.getString() >= b.getString() ? 1 : 0);
			std::cout << "GTE (string): " << a.getString() << " >= " << b.getString() << " ? " << (a.getString() >= b.getString() ? 1:0) << std::endl;
		}
		// Check for comparisons with NULL
		else if((a.type == NIL && b.type != NIL) || (a.type != NIL && b.type == NIL)){
			throw InvalidTypeForOperationException("GTE", "NIL");
			status = ERROR;
		}
		else if(a.type == NIL && b.type == NIL){
			stack.push(1);
			std::cout << "GTE (NIL): NULL >= NULL 1" << std::endl;
		}

		// Check if they are equal as double
		else if(a.type == DOUBLE && b.type != DOUBLE){
			double conversion_value = static_cast<double>(b.type == FLOAT ? b.as.floatValue : b.as.intValue);
			stack.push(a.getDouble() >= conversion_value ? 1 : 0);
			std::cout << "GTE (double): " << a.getDouble() << " >= " << conversion_value << " ? " << (a.getDouble() >= conversion_value ? 1 : 0) << std::endl;
		}
		else if(a.type != DOUBLE && b.type == DOUBLE){
			double conversion_value = static_cast<double>(a.type == FLOAT ? a.as.floatValue : a.as.intValue);
			stack.push(conversion_value >= b.getDouble()? 1 : 0);
			std::cout << "GTE (double): " << conversion_value << " >= " << b.getDouble() << " ? " << (conversion_value >= b.getDouble() ? 1 : 0) << std::endl;
		}
		else if(a.type == DOUBLE && b.type == DOUBLE){
			stack.push(a.getDouble() >= b.getDouble() ? 1 : 0);
			std::cout << "GTE (double): " << a.getDouble() << " >= " << b.getDouble() << " ? " << (a.getDouble() >= b.getDouble() ? 1 : 0) << std::endl;
		}

		// Check if they are equal as floats
		else if(a.type == FLOAT && b.type != FLOAT){
			float conversion_value = static_cast<float>(b.as.intValue);
			stack.push(a.getFloat() >= conversion_value ? 1 : 0);
			std::cout << "GTE (float): " << a.getFloat() << " >= " << conversion_value << " ? " << (a.getFloat() >= conversion_value ? 1 : 0) << std::endl;
		}
		else if(a.type != FLOAT && b.type == FLOAT){
			float conversion_value = static_cast<float>(a.as.intValue);
			stack.push(conversion_value >= b.getFloat() ? 1 : 0);
			std::cout << "GTE (float): " << conversion_value << " >= " << b.getFloat() << " ? " << (conversion_value >= b.getFloat() ? 1 : 0) << std::endl;
		}
		else if(a.type == FLOAT && b.type == FLOAT){
			stack.push(a.getFloat() >= b.getFloat()? 1 : 0);
			std::cout << "GTE (float): " << a.getFloat() << " >= " << b.getFloat() << " ? " << (a.getFloat() >= b.getFloat() ? 1 : 0) << std::endl;
		}

		else{
			stack.push(a.getInt() >= b.getInt()? 1 : 0);
			std::cout << "GTE (int): " << a.getInt() << " >= " << b.getInt() << " ? " << (a.getInt() >= b.getInt()? 1 : 0)  << std::endl;
		}

	}
	else {
		throw ShortOnElementsOnStackException("GTE");
		status = ERROR;
	}

			
}

void StackVM::lt_operation(){
	if(stack.size() >= 2) {
		
		TaggedValue a = stack.top(); stack.pop();
		TaggedValue b = stack.top(); stack.pop();

		// Check for comparisons with STRINGS
		if((a.type == STRING && b.type != STRING) || (a.type != STRING && b.type == STRING)){
			throw InvalidTypeForOperationException("GLT", "STRING");
			status = ERROR;
		}
		else if(a.type == STRING && b.type == STRING){
			stack.push(a.getString() < b.getString() ? 1 : 0);
			std::cout << "LT (string): " << a.getString() << " < " << b.getString() << " ? " << (a.getString() < b.getString() ? 1:0) << std::endl;
		}
		// Check for comparisons with NULL
		else if((a.type == NIL && b.type != NIL) || (a.type != NIL && b.type == NIL)){
			throw InvalidTypeForOperationException("GTE", "NIL");
			status = ERROR;
		}
		else if(a.type == NIL && b.type == NIL){
			stack.push(0);
			std::cout << "LT (NIL): NULL < NULL 0" << std::endl;
		}

		// Check if they are equal as double
		else if(a.type == DOUBLE && b.type != DOUBLE){
			double conversion_value = static_cast<double>(b.type == FLOAT ? b.as.floatValue : b.as.intValue);
			stack.push(a.getDouble() < conversion_value ? 1 : 0);
			std::cout << "LT (double): " << a.getDouble() << " < " << conversion_value << " ? " << (a.getDouble() < conversion_value ? 1 : 0) << std::endl;
		}
		else if(a.type != DOUBLE && b.type == DOUBLE){
			double conversion_value = static_cast<double>(a.type == FLOAT ? a.as.floatValue : a.as.intValue);
			stack.push(conversion_value < b.getDouble()? 1 : 0);
			std::cout << "LT (double): " << conversion_value << " > " << b.getDouble() << " < " << conversion_value << " ? " << (conversion_value < b.getDouble() ? 1 : 0) << std::endl;
		}
		else if(a.type == DOUBLE && b.type == DOUBLE){
			stack.push(a.getDouble() < b.getDouble() ? 1 : 0);
			std::cout << "LT (double): " << a.getDouble() << " < " << b.getDouble() << " ? " << (a.getDouble() < b.getDouble() ? 1 : 0) << std::endl;
		}

		// Check if they are equal as floats
		else if(a.type == FLOAT && b.type != FLOAT){
			float conversion_value = static_cast<float>(b.as.intValue);
			stack.push(a.getFloat() < conversion_value ? 1 : 0);
			std::cout << "LT (float): " << a.getFloat() << " < " << conversion_value << " ? " << (a.getFloat() < conversion_value ? 1 : 0) << std::endl;
		}
		else if(a.type != FLOAT && b.type == FLOAT){
			float conversion_value = static_cast<float>(a.as.intValue);
			stack.push(conversion_value < b.getFloat() ? 1 : 0);
			std::cout << "LT (float): " << conversion_value << " < " << b.getFloat() << " ? " << (conversion_value < b.getFloat() ? 1 : 0) << std::endl;
		}
		else if(a.type == FLOAT && b.type == FLOAT){
			stack.push(a.getFloat() < b.getFloat()? 1 : 0);
			std::cout << "LT (float): " << a.getFloat() << " < " << b.getFloat() << " ? " << (a.getFloat() < b.getFloat() ? 1 : 0) << std::endl;
		}

		else{
			stack.push(a.getInt() < b.getInt()? 1 : 0);
			std::cout << "LT (int): " << a.getInt() << " < " << b.getInt() << " ? " << (a.getInt() < b.getInt()? 1 : 0)  << std::endl;
		}

	}
	else {
		throw ShortOnElementsOnStackException("LT");
		status = ERROR;
	}

			
}

void StackVM::lte_operation(){
	if(stack.size() >= 2) {
		
		TaggedValue a = stack.top(); stack.pop();
		TaggedValue b = stack.top(); stack.pop();

		// Check for comparisons with STRINGS
		if((a.getType() == STRING && b.getType() != STRING) || (a.getType() != STRING && b.getType() == STRING)){
			throw InvalidTypeForOperationException("LTE", "STRING");
			status = ERROR;
		}
		else if(a.type == STRING && b.type == STRING){
			stack.push(a.getString() <= b.getString() ? 1 : 0);
			std::cout << "LTE (string): " << a.getString() << " <= " << b.getString() << " ? " << (a.getString() <= b.getString() ? 1:0) << std::endl;
		}
		// Check for comparisons with NULL
		else if((a.getType() == NIL && b.getType() != NIL) || (a.getType() != NIL && b.getType() == NIL)){
			throw InvalidTypeForOperationException("GTE", "NIL");
			status = ERROR;
		}
		else if(a.getType() == NIL && b.getType() == NIL){
			stack.push(1);
			std::cout << "LTE (NIL): NULL >= NULL 1" << std::endl;
		}

		// Check if they are equal as double
		else if(a.getType() == DOUBLE && b.getType() != DOUBLE){
			double conversion_value = static_cast<double>(b.getType() == FLOAT ? b.getFloat() : b.getInt());
			stack.push(a.getDouble() <= conversion_value ? 1 : 0);
			std::cout << "LTE (double): " << a.getDouble() << " <= " << conversion_value << " ? " << (a.getDouble() <= conversion_value ? 1 : 0) << std::endl;
		}
		else if(a.type != DOUBLE && b.type == DOUBLE){
			double conversion_value = static_cast<double>(a.type == FLOAT ? a.as.floatValue : a.as.intValue);
			stack.push(conversion_value <= b.getDouble()? 1 : 0);
			std::cout << "LTE (double): " << conversion_value << " <= " << b.getDouble() << " ? " << (conversion_value <= b.getDouble() ? 1 : 0) << std::endl;
		}
		else if(a.type == DOUBLE && b.type == DOUBLE){
			stack.push(a.getDouble() <= b.getDouble() ? 1 : 0);
			std::cout << "LTE (double): " << a.getDouble() << " <= " << b.getDouble() << " ? " << (a.getDouble() <= b.getDouble() ? 1 : 0) << std::endl;
		}

		// Check if they are equal as floats
		else if(a.type == FLOAT && b.type != FLOAT){
			float conversion_value = static_cast<float>(b.as.intValue);
			stack.push(a.getFloat() <= conversion_value ? 1 : 0);
			std::cout << "LTE (float): " << a.getFloat() << " <= " << conversion_value << " ? " << (a.getFloat() <= conversion_value ? 1 : 0) << std::endl;
		}
		else if(a.type != FLOAT && b.type == FLOAT){
			float conversion_value = static_cast<float>(a.as.intValue);
			stack.push(conversion_value <= b.getFloat() ? 1 : 0);
			std::cout << "LTE (float): " << conversion_value << " <= " << b.getFloat() << " ? " << (conversion_value <= b.getFloat() ? 1 : 0) << std::endl;
		}
		else if(a.type == FLOAT && b.type == FLOAT){
			stack.push(a.getFloat() <= b.getFloat()? 1 : 0);
			std::cout << "LTE (float): " << a.getFloat() << " <= " << b.getFloat() << " ? " << (a.getFloat() <= b.getFloat() ? 1 : 0) << std::endl;
		}

		else{
			stack.push(a.getInt() <= b.getInt()? 1 : 0);
			std::cout << "LTE (int): " << a.getInt() << " <= " << b.getInt() << " ? " << (a.getInt() <= b.getInt()? 1 : 0)  << std::endl;
		}

	}
	else {
		throw ShortOnElementsOnStackException("LTE");
		status = ERROR;
	}

			
}

void StackVM::swap_operation(){
	if(stack.size() >= 2) {
		TaggedValue a = stack.top(); stack.pop();
		TaggedValue b = stack.top(); stack.pop();
		stack.push(a);
		stack.push(b);

		// Print the SWAP operation
		std::cout << "SWAP: ";

		if(a.getType() == STRING){
			std::cout << a.getString();
		}
		else if(a.getType() == DOUBLE){
			std::cout << a.getDouble();
		}
		else if(a.getType() == FLOAT){
			std::cout << a.getFloat();
		}
		else if(a.getType() == INT){
			std::cout << a.getInt();
		}
		else if(a.getType() == NIL){
			std::cout << "NULL";
		} 
		else {
			throw InvalidTypeException();
			status = ERROR;
		}

		std::cout << " <--> ";

		if(b.getType() == STRING){
			std::cout << b.getString();
		}
		else if(b.getType() == DOUBLE){
			std::cout << b.getDouble();
		}
		else if(b.getType() == FLOAT){
			std::cout << b.getFloat();
		}
		else if(b.getType() == INT){
			std::cout << b.getInt();
		}
		else if(b.getType() == NIL){
			std::cout << "NULL";
		}
		else {
			throw InvalidTypeException();
			status = ERROR;
		}

	}
	else {
		throw ShortOnElementsOnStackException("SWAP");
		status = ERROR;
	}
}

void StackVM::dup_operation(){
	if(!stack.empty()){
		std::cout << "DUP: ";
		TaggedValue a = stack.top(); 
		stack.push(a);

		if(a.getType() == STRING){
			std::cout << a.getString();
		}
		else if(a.getType() == DOUBLE){
			std::cout << a.getDouble();
		}
		else if(a.getType() == FLOAT){
			std::cout << a.getFloat();
		}
		else if(a.getType() == INT){
			std::cout << a.getInt();
		}
		else if(a.getType() == NIL){
			std::cout << "NULL";
		}
		else {
			throw InvalidTypeException();
			status = ERROR;
		}
	}
	else {
		throw ShortOnElementsOnStackException("DUP");
		status = ERROR;
	}
}

void StackVM::jump_operation(){
	tc++;
	if(policy[pc][tc].getType() == STRING){
		//if the string is a label we check that it's stored in the label map
		string key = policy[pc][tc].getString();
		
		if(key.find("label") != std::string::npos){
			
			if(labelMap.find(key) != labelMap.end()){
				
				int target_address = labelMap.at(key);
				int current_pc = pc;
				pc = target_address;

				std::cout << "Jumping to Label '" << key << "' @ PC Address: " << target_address << std::endl;
				//std::cout << "PC: " << pc << "	";
				step(policy[pc]);
				pc = current_pc;
			}
			else {
				throw AddressNotDecleredException();
				status = ERROR;
			}
		}
	}
}

void StackVM::jump_if_zero_operation(){

	ValueType type = stack.top().getType();

	switch (type)
	{
	case INT:
		if(stack.top().getInt() == 0){
			jump_operation();
		} else {
			throw TopStackNotZeroException();
			status = ERROR;
		}
		break;
	case FLOAT:
		if(stack.top().getFloat() == 0.0f){
			jump_operation();
		} else {
			throw TopStackNotZeroException();
			status = ERROR;
		}
		break;
	case DOUBLE:
		if(stack.top().getDouble() == static_cast<double>(0)){
			jump_operation();
		} else {
			throw TopStackNotZeroException();
			status = ERROR;
		}
		break;
	
	default:
		throw TopStackNotZeroException();
		status = ERROR;
		break;
	}
}

void StackVM::call_function(){
	tc++;
	std::string function_name = policy[pc][tc].getString();
	if(labelMap.find(function_name) != labelMap.end()){
		if(function_table[function_name].paramNames.size() > 0){
			for(int j = 0; j < function_table[function_name].paramNames.size(); j++){
				if(function_table[function_name].paramTypes[j] == INT){
					pushi_operation();
				} else if(function_table[function_name].paramTypes[j] == FLOAT){
					pushf_operation();
				} else if(function_table[function_name].paramTypes[j] == DOUBLE){
					pushd_operation();
				}
			}
		}

		stack.push(TaggedValue(pc+1));
		next_pc = pc + 1;
		stack.pop();
		int target_address = labelMap.at(function_name);
		std::cout << "CALL: Jumping to function " << function_name << "	";
		pc = target_address;
	} else {
		throw FunctionNotDefinedException(function_name);
	}
}

void StackVM::ret_operation(){
	if (!stack.empty()) {
        // Pop the return address from the stack
        TaggedValue return_address = stack.top();
        //stack.pop();

        // Set the program counter to the return address
        pc = next_pc - 1;
        std::cout << "RET: Returning to address " << pc << "	";
    } else {
        throw PopEmptyStackException();
		status = ERROR;
    }
}
void StackVM::doPrimitive(Op operation) {
	cout << "PC: " << pc << "	";
	switch (operation){
		
		case HALT: //case HALT: //halt
			std::cout << "HALT" << std::endl;
			running = 0;
			status = DONE;
			break;
	
		case PUSHNULL:
			pushNull_operation();
			break;
		
		case PUSHI: //case PUSH Integer:
			//std::cout << "PUSH integer: ";
 			// move to the next instruction that contains the value
			pushi_operation();
			break;
		
		case PUSHF: //case PUSH:
			//std::cout << "PUSH floar: ";
 			// move to the next instruction that contains the value
			pushf_operation();
			break;
		
		case PUSHD: //case PUSH:
			//std::cout << "PUSH floar: ";
 			// move to the next instruction that contains the value
			pushd_operation();
			break;

		case POP: //case POP: // pop
           	pop_operation();
           	break;


		case ADD: //case ADD: // add
			add_operation();
			break;

		case SUB: //case SUB: // sub
			sub_operation();
			break;

		case MUL: //case MUL: // mult
			mul_operation();
			break;

		case DIV: //case DIV: // div
			div_operation();
			break;
		
		case MOD: //case MOD: // mod
			mod_operation();
			break;
		
		case EQL: //case EQL: // equal
			eql_operation();
			break;
		
		case NEQL: //case NOT EQL: // not equal
			neql_operation();
			break;
		
		case GT: //case GT: // top of the stack is greater than the element below
			gt_operation();
			break;
		
		case GTE: //case GTE: // top of the stack is bigger than the element below
			gte_operation();
			break;

		case LT: //case LT: // top of the stack is less than the element below
			lt_operation();
			break;
		
		case LTE: //case LTE: // top of the stack is less than the element below
			lte_operation();
			break;

		case SWAP: //case SWAP: // swap (swap the top two stack elements)
            swap_operation();
			break;

		case DUP: //case DUP: // duplicate top of the stack
			dup_operation();
			break;

		case JMP: //case JMP: // jump
			jump_operation();
			break;

		case JZ: //case JZ: // jump if zero
			jump_if_zero_operation();
			break;

		case CALL:
            call_function();
            break;

        case RET:
            ret_operation();
            break;
		default:
            throw InvalidPrimitiveException("");
            exit(1);
			break;
	
	
	//BITWISE OPERATION
		// case 11: // AND
        //     		if (sp < 1) {
        //         		std::cerr << "Error: Not enough elements to AND" << std::endl;
        //         		break;
        //     		}
        //     		std::cout << "and " << memory[sp - 2] << " & " << memory[sp - 1] << std::endl;
        //     		memory[sp - 2] &= memory[sp - 1];
        //     		sp--;
        //     		break;
        // 	case 12: // OR
        //     		if (sp < 1) {
        //         		std::cerr << "Error: Not enough elements to OR" << std::endl;
        //         		break;
        //     		}
        //     		std::cout << "or " << memory[sp - 2] << " | " << memory[sp - 1] << std::endl;
        //     		memory[sp - 2] |= memory[sp - 1];
        //     		sp--;
        //     		break;
        // 	case 13: // XOR
        //     		if (sp < 1) {
        //         		std::cerr << "Error: Not enough elements to XOR" << std::endl;
        //         		break;
        //     		}
        //     		std::cout << "xor " << memory[sp - 2] << " ^ " << memory[sp - 1] << std::endl;
        //     		memory[sp - 2] ^= memory[sp - 1];
        //     		sp--;
        //     		break;
        // 	case 14: // NOT
        //     		if (sp < 0) {
        //         		std::cerr << "Error: Not enough elements to NOT" << std::endl;
        //         		break;
        //     		}
        //     		std::cout << "not " << memory[sp - 1] << std::endl;
        //     		memory[sp - 1] = ~memory[sp - 1];
        //     		break;
		
	}
}

void StackVM::printTaggedValue(TaggedValue data_ex){

	ValueType typ = data_ex.getType();
	switch(typ){
		case INT:
			std::cout << data_ex.getInt();
			break;
		case FLOAT:
			std::cout << data_ex.getFloat();
			break;
		case DOUBLE:
			std::cout << data_ex.getInt();
			break;
		case STRING:
			std::cout << data_ex.getString();
			break;
		default:
			std::cout << "NULL";
			break;

	}
		
}

// Implement printStack to display the tagged values
void StackVM::printStack() {
    if (!stack.empty()) {
        cout << "    Top of Stack: ";
        TaggedValue top = stack.top();
		printTaggedValue(top);
		cout << endl;
    } else {
        cout << "    Stack is empty." << endl;
    }
}

void StackVM::step(vector<TaggedValue> instruction_line){
	
	for(tc = 0; tc < instruction_line.size(); tc++){
		decode(instruction_line[tc]);
		execute(instruction_line[tc]);
	}
	printStack();
	fetch();
}

void StackVM::run() {
	//pc -= 1;
	while(running == 1 && status == READY) {
		step(policy[pc]);
	}	
}
