VPATH = ../src
objects = main.o dgalnode.o
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread

all: $(objects)
	$(CXX) $(CXXFLAGS) -o coordinator.out $(objects)

main.o: main.cpp
dgalnode.o: dgalnode.cpp

clean:
	-rm -f *.o