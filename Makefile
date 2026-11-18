CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -O2 -Iinclude -MMD -MP

# Todos os .cpp de src/ menos o main entram tanto no programa quanto nos testes.
SRC_COMUM := $(filter-out src/main.cpp,$(wildcard src/*.cpp))
OBJ_COMUM := $(SRC_COMUM:src/%.cpp=build/%.o)

PROGRAMA         := bin/teste_vocacional
TESTES_ARVORE    := bin/testes_arvore
TESTES_PERGUNTAS := bin/testes_perguntas
TESTES_EXPERIMENTO := bin/testes_experimento
TESTES_CONSTRUTORES := bin/testes_construtores

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

$(TESTES_EXPERIMENTO): build/teste_experimento.o $(OBJ_COMUM) | bin
	$(CXX) $(CXXFLAGS) $^ -o $@

# Objetos dos testes: tests/<nome>.cpp -> build/<nome>.o
build/%.o: tests/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TESTES_CONSTRUTORES): build/teste_construtores.o $(OBJ_COMUM) | bin
	$(CXX) $(CXXFLAGS) $^ -o $@

run: $(PROGRAMA)
	./$(PROGRAMA)

testes: $(TESTES_ARVORE) $(TESTES_PERGUNTAS) $(TESTES_EXPERIMENTO) $(TESTES_CONSTRUTORES)
	./$(TESTES_ARVORE)
	./$(TESTES_EXPERIMENTO)
	./$(TESTES_PERGUNTAS)
	./$(TESTES_CONSTRUTORES)

bin build:
	mkdir -p $@

clean:
	rm -rf bin build

-include $(wildcard build/*.d)
