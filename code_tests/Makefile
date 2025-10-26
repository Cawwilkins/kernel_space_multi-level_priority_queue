CXX = gcc
CXXFLAGS = -Wall

build_poc: poc.c 421proj3structs.h
	$(CXX) $(CXXFLAGS) poc.c -o poc -pthread

build_test: test.c 421proj3structs.h
	$(CXX) $(CXXFLAGS) test.c -o test -pthread

clean:
	@rm -f *~ *.o *.save poc

run:
	./poc

run_t:
	./test
