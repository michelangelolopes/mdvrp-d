INCLUDE = include/clustering/* include/metaheuristics/* include/problem/* include/solution/* include/utils/*
SRC = src/clustering/* src/metaheuristics/* src/problem/* src/solution/* src/utils/*
BIN = bin/mdvrp-d
ARGS ?= mdvrp 01 192 3 10 10 2 10

compile:
	@echo "compiling..."
	@g++ -o $(BIN) $(INCLUDE) $(SRC) src/main.cpp

compile_opt:
	@echo "compiling with optimization..."
	@g++ -O3 -o $(BIN) $(INCLUDE) $(SRC) src/main.cpp

compile_debug:
	@echo "compiling in debug mode..."
	@g++ -g -DDEBUG -o $(BIN) $(INCLUDE) $(SRC) src/main.cpp

compile_opt_debug:
	@echo "compiling with optimization in debug mode..."
	@g++ -O3 -g -DDEBUG -o $(BIN) $(INCLUDE) $(SRC) src/main.cpp

run:
	@echo "running..."
	@$(BIN) $(ARGS)

run_gdb:
	@echo "running with gdb..."
	@gdb -ex=r --args $(BIN) $(ARGS)

run_valgrind:
	@echo "running with valgrind..."
	@valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes $(BIN) $(ARGS)

run_valgrind_gdb:
	@echo "running with valgrind and gdb"
	@valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes --vgdb-error=0 $(BIN) $(ARGS)