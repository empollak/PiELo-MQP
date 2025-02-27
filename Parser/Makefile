parser: tokenizer.o main.o parser.o codegen.o
	g++ -o parser tokenizer.o main.o parser.o codegen.o

main.o:	main.cpp tokenizer.h parser.h codegen.h
	g++ -o main.o -c main.cpp

tokenizer.o: tokenizer.cpp tokenizer.h
	g++ -o tokenizer.o -c tokenizer.cpp

parser.o: parser.cpp parser.h tokenizer.h
	g++ -o parser.o -c parser.cpp

codegen.o: codegen.cpp codegen.h parser.h
	g++ -o codegen.o -c codegen.cpp

clean: 
	rm -f *.o parser