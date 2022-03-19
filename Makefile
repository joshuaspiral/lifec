CXX=clang++
CXXFLAGS=-Wall -Wextra -ggdb -O3 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

make all: main.cpp
	$(CXX) $(CXXFLAGS) -o lifec main.cpp -lm

lifec: main.cpp
	$(CXX) $(CXXFLAGS) -o lifec main.cpp -lm

clean:
	rm lifec
