vm: instructions.o vm.o parser.o
	g++ -o vm vm.o instructions.o parser.o

instructions.o: instructions.cpp instructions.h
	g++ -o instructions.o -c instructions.cpp

parser.o: parser.cpp parser.h
	g++ -o parser.o -c parser.cpp

vm.o: vm.cpp
	g++ -o vm.o -c vm.cpp

clean: 
	rm -f *.o vm