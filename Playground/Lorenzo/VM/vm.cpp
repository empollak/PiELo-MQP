#include "opcodes.h"
#include <vector>
#include <iostream>

int main(int argc, char** argv) {
	std::vector<int> code = {
		PUSH, 10,
		PUSH, 20,
		ADD,
		PRINT,
		HALT
	};


	VM vm;
	try {
		vm.run(code);
	} catch (const std::exception& e) {
		std::cerr << "Runtime Error: " << e.what() << std::endl;
		return 1;
	}
	
	return 0;

}
	
