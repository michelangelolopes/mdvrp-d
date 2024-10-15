compile:
	@echo "compiling..."
	@g++ ./bin/mdvrp-d include/clustering/* include/metaheuristics/* include/problem/*  include/utils/* src/clustering/* src/metaheuristics/* src/problem/* src/utils/* src/main.cpp

compile_debug:
	@echo "compiling in debug mode..."
	@g++ -g -DDEBUG -o ./bin/mdvrp-d include/clustering/* include/metaheuristics/* include/problem/*  include/utils/* src/clustering/* src/metaheuristics/* src/problem/* src/utils/* src/main.cpp

run:
	@echo "running..."
	@./bin/mdvrp-d p01-D.mdvrp-d

run_valgrind:
	@echo "running with valgrind..."
	@valgrind -s --leak-check=full --track-origins=yes ./bin/mdvrp-d p01-D.mdvrp-d