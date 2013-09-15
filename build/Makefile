VPATH=../src
objects = messenger.o population.o individual.o nodemanager.o
CXXFLAGS = -std=c++11 -Wall -Wextra
LDLIBS = -pthread


all: $(objects)
	ar cr libdgal.a $(objects)
	@echo "Library built"


messenger.o: messenger.cpp messenger.h
population.o: population.cpp population.h
individual.o: individual.cpp individual.h
nodemanager.o: nodemanager.cpp nodemanager.h


clean:
	-rm -f *.o
	-rm -f libdgal.a
