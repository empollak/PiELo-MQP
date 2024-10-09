#include "opcodes.h"
#include <vector>
#include <iostream>

int main(int argc, char **argv)
{
	std::vector<int> code = {
		PiELo_VM::PUSH, 10,
		PiELo_VM::PUSH, 20,
		PiELo_VM::ADD,
		PiELo_VM::PRINT,
		PiELo_VM::HALT};

	PiELo_VM::VM vm;
	try
	{
		vm.run(code);
	}
	catch (const std::exception &e)
	{
		std::cerr << "Runtime Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
