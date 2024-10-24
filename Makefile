INCLUDE = include/clustering/* include/metaheuristics/* include/problem/* include/solution/* include/utils/*
SRC = src/clustering/* src/metaheuristics/* src/problem/* src/solution/* src/utils/*
ARGS ?= mdvrp 01

compile:
	@echo "compiling..."
	@g++ -o ./bin/mdvrp-d $(INCLUDE) $(SRC) src/main.cpp

compile_opt:
	@echo "compiling with optimization..."
	@g++ -O3 -o ./bin/mdvrp-d $(INCLUDE) $(SRC) src/main.cpp

compile_debug:
	@echo "compiling in debug mode..."
	@g++ -g -DDEBUG -o ./bin/mdvrp-d $(INCLUDE) $(SRC) src/main.cpp

compile_opt_debug:
	@echo "compiling with optimization in debug mode..."
	@g++ -O3 -g -DDEBUG -o ./bin/mdvrp-d $(INCLUDE) $(SRC) src/main.cpp

run:
	@echo "running..."
	@./bin/mdvrp-d $(ARGS)

run_gdb:
	@echo "running with gdb..."
	@gdb -ex=r --args ./bin/mdvrp-d $(ARGS)

run_valgrind:
	@echo "running with valgrind..."
	@valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./bin/mdvrp-d $(ARGS)