COMPILER := clang++ -shared -undefined dynamic_lookup --std=c++14
LLVM_FLAGS := `llvm-config --cxxflags`
CC := $(COMPILER) $(LLVM_FLAGS)

PLUGIN_SO := ./build/plugin/out.so


SRC := src/PrintFunctionNames.cpp

.PHONY: build

build: $(SRC)
	$(CC) $(SRC) -o $(PLUGIN_SO)
