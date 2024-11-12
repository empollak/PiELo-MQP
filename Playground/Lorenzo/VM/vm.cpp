#include "opcodes.h"
#include <vector>
#include <iostream>

int main(int argc, char **argv)
{

	PiELo_VM::VM vm;

	std::vector<int> code = {
		PiELo_VM::PUSH, 10,
		PiELo_VM::PUSH, 20,
		PiELo_VM::ADD,
		PiELo_VM::PRINT,
		PiELo_VM::HALT};

	vm.run(code);

	while (vm.get_status() == PiELo_VM::VMStatus::OK)
	{
		vm.step();
	}

	if (vm.get_status() == PiELo_VM::VMStatus::DONE)
	{
		std::cout << "Program finished successfully!" << std::endl;
	}
	else if (vm.get_status() == PiELo_VM::VMStatus::ERROR || vm.get_status() == PiELo_VM::VMStatus::EXPLODED)
	{
		std::cout << "Program encountered an error or exploded!" << std::endl;
	}

	return 0;
}