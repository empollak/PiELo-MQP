BUILDDIR := build
OBJECTS := $(patsubst %.o, $(BUILDDIR)/%.o, tokenizer.o parser.o codegen.o)
EXECUTABLE = parser

all: $(EXECUTABLE)



# $^ means "all of the prereqs" and $@ means "name of the target"
$(EXECUTABLE): $(OBJECTS) $(BUILDDIR)/main.o | $(BUILDDIR)
	g++ $^ -o $@

$(BUILDDIR)/main.o:	main.cpp tokenizer.h parser.h codegen.h | $(BUILDDIR)
	g++ -o $(BUILDDIR)/main.o -c main.cpp

$(BUILDDIR)/tokenizer.o: tokenizer.cpp tokenizer.h | $(BUILDDIR)
	g++ -o $(BUILDDIR)/tokenizer.o -c tokenizer.cpp

$(BUILDDIR)/parser.o: parser.cpp parser.h tokenizer.h | $(BUILDDIR)
	g++ -o $(BUILDDIR)/parser.o -c parser.cpp

$(BUILDDIR)/codegen.o: codegen.cpp codegen.h parser.h | $(BUILDDIR)
	g++ -o $(BUILDDIR)/codegen.o -c codegen.cpp

$(BUILDDIR): 
	mkdir $(BUILDDIR)

clean: 
	rm -f $(BUILDDIR)/*.o parser