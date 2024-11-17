#include "stack-vm.h"
#include "lexer.h"
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[]) {
	// check for input errors
	if (argc != 2 ) {
		cout << "Usage: " << argv[0] << "sasm-filename" << endl;
		exit(1);
	}

	// read input file
	ifstream infile;
	infile.open(argv[1]);
	if(!infile.is_open()) {
		cout << "Error: could not open [" << argv[1] << "]" << endl;
		exit(1);
	}
	string line;
	string contents;
	while(getline(infile, line)) {
		contents += line + "\n";
	}
	infile.close();

	// parse file
	Lexer lexer;
	strings lexemes = lexer.lex(contents);

	std::cout << std::endl;
	std::cout << "Step 1: PARSING..." << std::endl;

	for (int j = 0; j < lexemes.size(); j++){
		std::cout << "Line " << j << ": " << lexemes[j] << std::endl;
	}
	
	StackVM vm;

	vm = StackVM();

	// compile to binary
	std::cout << std::endl;
	std::cout << "Step 2: COMPILING..." << std::endl;
	vm.loadInstructions(lexemes);

	std::cout << std::endl;

	// std::cout << std::endl;
	// std::cout << "Step 3: EXECUTING..." << std::endl;
	// vm.run();

	return 0;
}


