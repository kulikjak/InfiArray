CXX = g++ -o0 --std=c++11

all: performance

performance: performance.cc
	$(CXX) $^ -o $@

clean:
	rm -f performance

.PHONY: all performance clean
