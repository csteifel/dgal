VPATH=../src
objects = messenger.o individual.o
CXXFLAGS = -std=c++11 -Wall -Wextra
LDLIBS = -pthread


all: $(objects)
	ar cr libdgal.a $(objects)
	@echo "Library built"


messenger.o: messenger.cpp messenger.h
individual.o: individual.cpp individual.h


clean:
	-rm -f *.o
	-rm -f libdgal.a
