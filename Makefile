CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -O2 -Iinclude -MMD -MP

# Todos os .cpp de src/ menos o main entram tanto no programa quanto nos testes.
SRC_COMUM := $(filter-out src/main.cpp,$(wildcard src/*.cpp))
OBJ_COMUM := $(SRC_COMUM:src/%.cpp=build/%.o)

PROGRAMA         := bin/teste_vocacional
TESTES_ARVORE    := bin/testes_arvore
TESTES_PERGUNTAS := bin/testes_perguntas

.PHONY: all run testes clean

all: $(PROGRAMA)

$(PROGRAMA): build/main.o $(OBJ_COMUM) | bin
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TESTES_ARVORE): build/teste_arvore.o $(OBJ_COMUM) | bin
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TESTES_PERGUNTAS): build/test_perguntas.o $(OBJ_COMUM) | bin
	$(CXX) $(CXXFLAGS) $^ -o $@

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/teste_arvore.o: tests/teste_arvore.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/test_perguntas.o: tests/test_perguntas.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(PROGRAMA)
	./$(PROGRAMA)

testes: $(TESTES_ARVORE) $(TESTES_PERGUNTAS)
	./$(TESTES_ARVORE)
	./$(TESTES_PERGUNTAS)

bin build:
	mkdir -p $@

clean:
	rm -rf bin build

-include $(wildcard build/*.d)
