CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -O2 -Iinclude -MMD -MP

# Todos os .cpp de src/ menos o main entram tanto no programa quanto nos testes.
SRC_COMUM := $(filter-out src/main.cpp,$(wildcard src/*.cpp))
OBJ_COMUM := $(SRC_COMUM:src/%.cpp=build/%.o)

PROGRAMA := bin/teste_vocacional
TESTES   := bin/testes

.PHONY: all run testes clean

all: $(PROGRAMA)

$(PROGRAMA): build/main.o $(OBJ_COMUM) | bin
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TESTES): build/teste_arvore.o $(OBJ_COMUM) | bin
	$(CXX) $(CXXFLAGS) $^ -o $@

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/teste_arvore.o: tests/teste_arvore.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(PROGRAMA)
	./$(PROGRAMA)

testes: $(TESTES)
	./$(TESTES)

bin build:
	mkdir -p $@

clean:
	rm -rf bin build

-include $(wildcard build/*.d)
