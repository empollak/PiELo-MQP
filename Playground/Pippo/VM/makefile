CXX = g++
CFLAGS = -std=c++11

all: stack-vm

stack-vm: main.o stack-vm.o lexer.o
	$(CXX) $(CFLAGS) -o $@ main.o stack-vm.o lexer.o

main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

stack-vm.o: stack-vm.cpp stack-vm.h lexer.h exceptions.h
	$(CXX) $(CFLAGS) -c stack-vm.cpp

lexer.o: lexer.cpp lexer.h
	$(CXX) $(CFLAGS) -c lexer.cpp

clean:
	rm -f *.o stack-vm


