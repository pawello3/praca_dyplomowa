COMPILER=g++
ERRORSFLAGS=-Wall -Wextra -Werror -O2
STANDARDFLAGS=-std=c++11
LIBRARIES=-lm
CODE_DIRECTORY=code
_DEPENDENCIES=debug.hpp Log_store.hpp
DEPENDENCIES=$(patsubst %,$(CODE_DIRECTORY)/%,$(_DEPENDENCIES))
_SOURCES=main.cpp Log_store.cpp
SOURCES=$(patsubst %,$(CODE_DIRECTORY)/%,$(_SOURCES))
OBJECTS_DIRECTORY=objects
_OBJECTS=$(_SOURCES:.cpp=.o)
OBJECTS=$(patsubst %,$(OBJECTS_DIRECTORY)/%,$(_OBJECTS))
BINARY_FILE_DIRECTORY=bin
EXECUTABLE=$(BINARY_FILE_DIRECTORY)/testing_program
TEXT_DIRECTORY=text
TEXT=praca_licencjacka
AUXILARY_DIRECTORY=auxilary
_AUXILARY=$(TEXT).aux $(TEXT).log $(TEXT).out $(TEXT).toc
AUXILARY=$(patsubst %,$(AUXILARY_DIRECTORY)/%,$(_AUXILARY))

all: cpp tex

cpp: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	if [ ! -d $(BINARY_FILE_DIRECTORY) ]; then mkdir $(BINARY_FILE_DIRECTORY); fi;\
	$(COMPILER) $(STANDARDFLAGS) $(ERRORSFLAGS) $^ -o $@ $(LIBRARIES)

$(OBJECTS_DIRECTORY)/%.o: $(CODE_DIRECTORY)/%.cpp $(DEPENDENCIES)
	if [ ! -d $(OBJECTS_DIRECTORY) ]; then mkdir $(OBJECTS_DIRECTORY); fi;\
	$(COMPILER) $(STANDARDFLAGS) $(ERRORSFLAGS) -c $< -o $@ $(LIBRARIES)

tex: $(AUXILARY)

$(AUXILARY): $(TEXT_DIRECTORY)/$(TEXT).tex
	if [ ! -d $(AUXILARY_DIRECTORY) ]; then mkdir $(AUXILARY_DIRECTORY); fi;\
	cd $(TEXT_DIRECTORY);\
	for i in 1 2 ; do pdflatex -draftmode -output-directory=../$(AUXILARY_DIRECTORY) $(TEXT).tex; done;\
	pdflatex -output-directory=../$(AUXILARY_DIRECTORY) $(TEXT).tex;\
	cd ..;\
	mv $(AUXILARY_DIRECTORY)/$(TEXT).pdf $(TEXT_DIRECTORY);\

aspell:
	cat $(TEXT_DIRECTORY)/$(TEXT).tex | aspell -l pl_PL -t list | aspell -l en -t list | sort -u

.PHONY: aspell cleanfoo clean

clean:
	find . -name "*.o" -type f -exec rm '{}' \;
	find . -name "*~" -type f -exec rm '{}' \;
	find . -name "*.save" -type f -exec rm '{}' \;
	find . -name "*.log" -type f -exec rm '{}' \;
	- rmdir $(OBJECTS_DIRECTORY)
	- rm $(EXECUTABLE)
	- rmdir $(BINARY_FILE_DIRECTORY)
	- rm $(AUXILARY_DIRECTORY)/*
	- rmdir $(AUXILARY_DIRECTORY)

distclean: clean
	- rm $(TEXT_DIRECTORY)/$(TEXT).pdf
