vm: instructions.o vm.o
	g++ -o vm vm.o instructions.o

instructions.o: instructions.cpp instructions.h
	g++ -o instructions.o -c instructions.cpp

vm.o: vm.cpp vm.h instructions.h
	g++ -o vm.o -c vm.cpp

clean: 
	rm -f *.o vm