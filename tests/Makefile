VPATH=../build:../src:../shared/src
objects = farm.o
includes = -I../src/ -I../shared/src
CXXFLAGS = -std=c++11 -Wall -Wextra -DDEBUG -pthread $(includes) $(libs)
CXX = g++

all: $(objects)
	$(CXX) $(CXXFLAGS) -o test.out $(objects) $(LDLIBS)
	@echo "Tests built"

main.o: main.cpp population.h

farm.o: farm.cpp population.h

clean:
	-rm -f *.o
