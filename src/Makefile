CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2 -g

all: build

build: main.cpp
	$(CXX) $(CXXFLAGS) -o main main.cpp

run: build
	./main

clean:
	rm -f  main
