parser: tokenizer.o main.o
	g++ -o parser tokenizer.o main.o

main.o:	main.cpp tokenizer.h
	g++ -o main.o -c main.cpp

tokenizer.o: tokenizer.cpp tokenizer.h
	g++ -o tokenizer.o -c tokenizer.cpp

clean: 
	rm -f *.o parser