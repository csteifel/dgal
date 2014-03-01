VPATH = ../src:../../shared/src
objects = main.o dgalnode.o
includes = -I../../shared/src
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread $(includes)

all: $(objects)
	$(CXX) $(CXXFLAGS) -o coordinator.out $(objects)

main.o: main.cpp main.h dgalutility.h
dgalnode.o: dgalnode.cpp dgalnode.h

clean:
	-rm -f *.o
