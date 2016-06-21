COMPILER=g++
ERRORSFLAGS=-Wall -Wextra -Werror -O2
STANDARDFLAGS=-std=c++11
LIBRARIES=-lm
CODE_DIRECTORY=code
_RANDOM_GENERATOR=random_generator.py
RANDOM_GENERATOR=$(CODE_DIRECTORY)/$(_RANDOM_GENERATOR)
_RANDOM_DATA=random_data.txt
RANDOM_DATA=$(CODE_DIRECTORY)/$(_RANDOM_DATA)
_DEPENDENCIES=debug.hpp SILT_key.hpp SILT_key.tpp Small_Index_Large_Table.hpp Small_Index_Large_Table.tpp Log_store.hpp Log_store.tpp Hash_store.hpp Hash_store.tpp Merge_heap.hpp Sorted_store.hpp Sorted_store.tpp Trie.hpp Trie.tpp
DEPENDENCIES=$(patsubst %,$(CODE_DIRECTORY)/%,$(_DEPENDENCIES))
_SOURCES=main.cpp Merge_heap.cpp
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

cpp: py $(EXECUTABLE)

py:
	if [ ! -f $(RANDOM_DATA) ]; then $(RANDOM_GENERATOR) > $(RANDOM_DATA); fi

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

.PHONY: aspell clean distclean

zip: clean
	zip -r $(TEXT).zip $(CODE_DIRECTORY) $(TEXT_DIRECTORY) Makefile

aspell:
	cat $(TEXT_DIRECTORY)/$(TEXT).tex | aspell -l pl_PL -t list | aspell -l en -t list | sort -u

clean:
	find . -name "*.o" -type f -exec rm '{}' \;
	find . -name "*~" -type f -exec rm '{}' \;
	find . -name "*.save" -type f -exec rm '{}' \;
	find . -name "*.log" -type f -exec rm '{}' \;
	find . -name "*.dat" -type f -exec rm '{}' \;
	- rm $(RANDOM_DATA)
	- rmdir $(OBJECTS_DIRECTORY)
	- rm $(EXECUTABLE)
	- rmdir $(BINARY_FILE_DIRECTORY)
	- rm $(AUXILARY_DIRECTORY)/*
	- rmdir $(AUXILARY_DIRECTORY)

distclean: clean
	- rm $(TEXT_DIRECTORY)/$(TEXT).pdf
