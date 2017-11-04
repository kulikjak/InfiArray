CXX = g++ -o0 --std=c++11

all: performance

performance: performance.cc
	$(CXX) $^ -o $@

tests: tests.cc
	$(CXX) $^ -o $@

clean:
	rm -f performance tests

.PHONY: all performance tests clean
