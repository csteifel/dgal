VPATH=../build:../src
objects = main.o
includes = -I../src/
LDLIBS = -L../build -ldgal
CXXFLAGS = -std=c++11 -Wall -Wextra -DDEBUG -pthread $(includes) $(libs)

all: $(objects)
	$(CXX) $(CXXFLAGS) -o test.out $(objects) $(LDLIBS)
	@echo "Tests built"

main.o: main.cpp nodemanager.h population.h

clean:
	-rm -f *.o