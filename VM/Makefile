INSDIR = instructions
BUILDDIR = build

EXECUTABLE = VM
# SOURCES = $(wildcard *.cpp,$(INSDIR)/*.cpp)
INSOBJECTS = $(patsubst %.o, $(BUILDDIR)/$(INSDIR)/%.o, arithmetic.o closureInstructions.o comparisonJumps.o)
OBJECTS = $(INSOBJECTS) $(patsubst %.o, $(BUILDDIR)/%.o, instructionHandler.o parser.o) 

all: dir $(BUILDDIR)/$(EXECUTABLE)
	echo "$(OBJECTS)"

dir:
	mkdir -p $(BUILDDIR)
	mkdir -p $(BUILDDIR)/$(INSDIR)

# $^ means "all of the prereqs" and $@ means "name of the target"
$(BUILDDIR)/$(EXECUTABLE): $(OBJECTS)
	g++ $^ -o $@

# see https://www.gnu.org/software/make/manual/html_node/Static-Usage.html for details
$(OBJECTS): $(BUILDDIR)/%.o : %.cpp %.h
	g++ -c $< -o $@


instructions.o: 