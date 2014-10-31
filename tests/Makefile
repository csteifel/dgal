VPATH=../build:../src:../shared/src
objects = main.o
includes = -I../src/ -I../shared/src
CXXFLAGS = -std=c++11 -Wall -Wno-reorder -Wextra -DDEBUG -pthread $(includes) $(libs)

all: $(objects)
	$(CXX) $(CXXFLAGS) -o test.out $(objects) $(LDLIBS)
	@echo "Tests built"

main.o: main.cpp population.h individual.h


jsonreadtest: jsonreadtest.cpp
	$(CXX) $(CXXFLAGS) -o jsonreadtest.out jsonreadtest.cpp $(LDLIBS)
	@echo "JSON Read test build"

farm.o: farm.cpp population.h

clean:
	-rm -f *.o
