all: test_pair bench_pair

catch_main.o: catch.hpp catch_main.cpp
	g++ catch_main.cpp -c -std=c++17 -march=native -Wall -Wextra -Wconversion -Wshadow -Wcast-qual -pedantic -pedantic-errors

test_pair.o: test_pair.cpp pair.hpp pair_detail.hpp
	g++ test_pair.cpp -c -std=c++17 -march=native -Wall -Wextra -Wconversion -Wshadow -Wcast-qual -pedantic -pedantic-errors

test_pair: test_pair.o catch_main.o
	g++ test_pair.o catch_main.o -o test_pair -std=c++17 -march=native -Wall -Wextra -Wconversion -Wshadow -Wcast-qual -pedantic -pedantic-errors

bench_pair: bench_pair.cpp pair.hpp pair_detail.hpp
	g++ bench_pair.cpp -o bench_pair -O3 -std=c++17 -march=native -Wall -Wextra -Wconversion -Wshadow -Wcast-qual -pedantic -pedantic-errors

PHONY: clean
clean:
	rm -f catch_main.o test_pair.o test_pair bench_pair
