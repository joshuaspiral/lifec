CXX=clang++
CXXFLAGS=-Wall -Wextra -ggdb -O3 -lraylib
make all: main.cpp
	$(CXX) $(CXXFLAGS) -o lifec main.cpp -lm

lifec: main.cpp
	$(CXX) $(CXXFLAGS) -o lifec main.cpp -lm

clean:
	rm lifec
